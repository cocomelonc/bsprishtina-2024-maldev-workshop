#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include <stdlib.h>

int sendFileWithKeyboard(const char* filePath) {
  const char* chatId = "5547299598";
  const char* replyMarkup = "{\"keyboard\":[[\"🐱 meow\",\"🐭 squeek\"]],\"resize_keyboard\":true,\"one_time_keyboard\":true}";
  const wchar_t* host = L"api.telegram.org";
  const wchar_t* endpoint = L"/bot8077364032:AAHTJbcbULWSH7dXAX3l8xpmUZzP6EeL7VQ/sendDocument"; // замените токен

  // читаем файл
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

  // создаем multipart тело
  const char* boundary = "----meowFormBoundary";
  char header[2048];
  char footer[128];

  snprintf(header, sizeof(header),
    "--%s\r\n"
    "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n"
    "%s\r\n"
    "--%s\r\n"
    "Content-Disposition: form-data; name=\"reply_markup\"\r\n\r\n"
    "%s\r\n"
    "--%s\r\n"
    "Content-Disposition: form-data; name=\"document\"; filename=\"%s\"\r\n"
    "Content-Type: application/octet-stream\r\n\r\n",
    boundary, chatId, boundary, replyMarkup, boundary, filePath);

  snprintf(footer, sizeof(footer), "\r\n--%s--\r\n", boundary);

  DWORD totalSize = strlen(header) + fileSize + strlen(footer);
  BYTE* postData = malloc(totalSize);
  memcpy(postData, header, strlen(header));
  memcpy(postData + strlen(header), fileContent, fileSize);
  memcpy(postData + strlen(header) + fileSize, footer, strlen(footer));
  free(fileContent);

  // отправка через WinHTTP
  HINTERNET hSession = WinHttpOpen(L"TelegramUploader", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
  if (!hSession) return 1;

  HINTERNET hConnect = WinHttpConnect(hSession, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
  if (!hConnect) {
    WinHttpCloseHandle(hSession);
    return 1;
  }

  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", endpoint, NULL, WINHTTP_NO_REFERER, NULL, WINHTTP_FLAG_SECURE);
  if (!hRequest) {
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  wchar_t contentHeader[256];
  swprintf(contentHeader, 256, L"Content-Type: multipart/form-data; boundary=%hs", boundary);

  BOOL sent = WinHttpSendRequest(
    hRequest,
    contentHeader,
    -1,
    postData,
    totalSize,
    totalSize,
    0);

  if (!sent) {
    fprintf(stderr, "WinHttpSendRequest failed: %lu\n", GetLastError());
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    free(postData);
    return 1;
  }

  WinHttpReceiveResponse(hRequest, NULL);

  // Очистка
  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hSession);
  free(postData);

  printf("file successfully sent. =^..^=\n");
  return 0;
}

int main() {
  sendFileWithKeyboard("cat1.png");
  return 0;
}
