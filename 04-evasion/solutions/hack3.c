/*
 * exercise 3 solution
 * AV evasion: automatic SSN (syscall number) parsing from ntdll stub
 * hack3.c
 * author: @cocomelonc
 *
 * base: 04-syscall/hack2.c  (printSyscallStub - manual byte reading)
 *
 * change: add parseSyscallNumber() that reads bytes[4] of the stub to
 * extract the SSN automatically.
 *
 * x64 syscall stub layout (unhooked ntdll):
 *   offset  bytes   meaning
 *   0       4C 8B D1   mov r10, rcx
 *   3       B8         mov eax, <imm32>   (opcode)
 *   4       xx 00 00 00  <SSN as 32-bit LE>  ← we read bytes[4]
 *   8       F6 04 25 08  test byte ptr [...]  (syscall guard)
 *   ...
 *   or (older pattern):
 *   8       0F 05      syscall
 *   10      C3         ret
 *
 * note: this approach reads from a freshly loaded (not-yet-hooked)
 * copy of ntdll loaded with DONT_RESOLVE_DLL_REFERENCES, the same
 * trick used in the original hack2.c.
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack3.c -o hack3.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
 */
#include <windows.h>
#include <stdio.h>

// original from hack2.c - unchanged
void printSyscallStub(char* funcName) {
  HMODULE ntdll = LoadLibraryExA("ntdll.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
  if (ntdll == NULL) { printf("[-] failed to load ntdll.dll\n"); return; }

  FARPROC funcAddress = GetProcAddress(ntdll, funcName);
  if (funcAddress == NULL) {
    printf("[-] failed to get address of %s\n", funcName);
    FreeLibrary(ntdll);
    return;
  }

  printf("[*] %s @ 0x%p : ", funcName, funcAddress);
  BYTE *bytes = (BYTE*)funcAddress;
  for (int i = 0; i < 23; i++) printf("%02X ", bytes[i]);
  printf("\n");
  FreeLibrary(ntdll);
}

/*
 * exercise 3 addition: parseSyscallNumber()
 *
 * the standard x64 stub starts with:
 *   4C 8B D1  (mov r10, rcx)   -- 3 bytes
 *   B8        (mov eax opcode) -- 1 byte
 *   <SSN>     (32-bit imm)     -- 4 bytes, low byte at offset 4
 *
 * so: SSN = (DWORD)bytes[4]
 * (for typical Windows 10/11 syscall numbers the value fits in one byte,
 *  but we read the full DWORD to be safe with larger SSNs.)
 */
void parseSyscallNumber(char* funcName) {
  HMODULE ntdll = LoadLibraryExA("ntdll.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
  if (ntdll == NULL) { printf("[-] failed to load ntdll.dll\n"); return; }

  FARPROC funcAddress = GetProcAddress(ntdll, funcName);
  if (funcAddress == NULL) {
    printf("[-] %s not found\n", funcName);
    FreeLibrary(ntdll);
    return;
  }

  BYTE *bytes = (BYTE*)funcAddress;

  /*
   * sanity check: verify the expected stub prologue
   *   byte[0..2] = 4C 8B D1  (mov r10, rcx)
   *   byte[3]    = B8         (mov eax, imm32 opcode)
   * if these differ, the stub may be hooked or the pattern changed.
   */
  if (bytes[0] != 0x4C || bytes[1] != 0x8B || bytes[2] != 0xD1 || bytes[3] != 0xB8) {
    printf("[!] %s: unexpected prologue - stub may be hooked or format changed\n", funcName);
    FreeLibrary(ntdll);
    return;
  }

  // read the 32-bit SSN stored at bytes[4..7] (little-endian)
  DWORD ssn = *(DWORD*)(bytes + 4);

  printf("[+] %-32s SSN = %4u  (0x%02X)\n", funcName, ssn, ssn);
  FreeLibrary(ntdll);
}

int main(void) {
  printf("=== raw stub dump (original printSyscallStub) ===\n");
  printSyscallStub("NtAllocateVirtualMemory");
  printSyscallStub("NtWriteVirtualMemory");
  printSyscallStub("NtCreateThreadEx");
  printf("\n");

  printf("=== exercise 3: automatic SSN extraction ===\n");
  parseSyscallNumber("NtAllocateVirtualMemory");
  parseSyscallNumber("NtWriteVirtualMemory");
  parseSyscallNumber("NtCreateThreadEx");

  // bonus: a few more for reference
  parseSyscallNumber("NtOpenProcess");
  parseSyscallNumber("NtReadVirtualMemory");
  parseSyscallNumber("NtProtectVirtualMemory");

  return 0;
}
