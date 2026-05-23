/*
 * exercise 4 solution
 * AV evasion: direct syscall via assembly stub - NtWriteVirtualMemory
 * hack4.c
 * author: @cocomelonc
 *
 * base: 05-syscall-2/hack.c  (only used NtAllocateVirtualMemory stub)
 *
 * new: uses myNtWriteVirtualMemory (SSN 0x3A, defined in hack4.asm)
 * to write into the current process's own allocated memory without
 * calling WriteProcessMemory (a common EDR hook target).
 *
 * flow:
 *   1. VirtualAlloc  -> buf (filled with 'A' * 32 to show before state)
 *   2. myNtWriteVirtualMemory(GetCurrentProcess(), buf, src, len, NULL)
 *      overwrites buf with a different string
 *   3. printf(buf) proves the write succeeded
 *
 * why this matters:
 *   WriteProcessMemory calls NtWriteVirtualMemory internally.
 *   EDR hooks are placed on the Win32 layer (WriteProcessMemory) or
 *   the ntdll wrapper.  A direct syscall jumps straight to the kernel,
 *   bypassing both hook layers entirely.
 *
 * compile:
 *   nasm -f win64 hack4.asm -o hack4_asm.o
 *   x86_64-w64-mingw32-gcc hack4.c hack4_asm.o -o hack4.exe \
 *     -I/usr/share/mingw-w64/include/ -s -ffunction-sections \
 *     -fdata-sections -Wno-write-strings -fno-exceptions \
 *     -fmerge-all-constants -static-libstdc++ -static-libgcc
 *
 * note: SSN 0x3A is correct for Windows 10 20H2 (build 19042).
 * verify your target build with hack3.exe (exercise 3) before using.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// ── external assembly stubs ─────────────────────────────────────────────
// defined in hack4.asm, called with the standard x64 Windows ABI

// original stub (NtAllocateVirtualMemory) - kept for reference
extern NTSTATUS myNtAllocateVirtualMemory(
    HANDLE  ProcessHandle,
    PVOID  *BaseAddress,
    ULONG   ZeroBits,
    PULONG  RegionSize,
    ULONG   AllocationType,
    ULONG   Protect
);

/*
 * exercise 4 addition: declaration for the new stub in hack4.asm.
 * prototype matches the NT API signature:
 *   NTSTATUS NtWriteVirtualMemory(
 *     HANDLE  ProcessHandle,
 *     PVOID   BaseAddress,
 *     PVOID   Buffer,
 *     ULONG   NumberOfBytesToWrite,
 *     PULONG  NumberOfBytesWritten   -- may be NULL
 *   );
 */
extern NTSTATUS myNtWriteVirtualMemory(
    HANDLE  ProcessHandle,
    PVOID   BaseAddress,
    PVOID   Buffer,
    ULONG   NumberOfBytesToWrite,
    PULONG  NumberOfBytesWritten
);

int main(void) {
  NTSTATUS status;
  PVOID    buf    = NULL;
  SIZE_T   size   = 64;

  // ── step 1: allocate a buffer in current process ──────────────────
  // using VirtualAlloc here; could also use myNtAllocateVirtualMemory
  buf = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!buf) {
    printf("[-] VirtualAlloc failed: %lu\n", GetLastError());
    return 1;
  }
  memset(buf, 'A', size - 1);   // fill with 'A' so the before state is obvious
  ((char*)buf)[size - 1] = '\0';

  printf("[*] buffer @ %p\n", buf);
  printf("[*] before: %s\n", (char*)buf);

  // ── step 2: overwrite with myNtWriteVirtualMemory ─────────────────
  char src[] = "Hello from a direct syscall! =^..^=";
  ULONG written = 0;

  /*
   * exercise 4 key call:
   * GetCurrentProcess() returns the pseudo-handle (-1 / 0xFFFFFFFF)
   * which is accepted by all Nt* functions for self-targeting.
   *
   * the syscall jumps directly to the kernel NtWriteVirtualMemory
   * without touching WriteProcessMemory or the ntdll wrapper.
   */
  status = myNtWriteVirtualMemory(
    GetCurrentProcess(),    // target = this process
    buf,                    // destination inside this process
    src,                    // source buffer
    (ULONG)strlen(src) + 1, // byte count (include null)
    &written                // optional: bytes actually written
  );

  if (status != 0) {
    printf("[-] myNtWriteVirtualMemory failed, NTSTATUS = 0x%08lX\n", status);
    VirtualFree(buf, 0, MEM_RELEASE);
    return 1;
  }

  // ── step 3: verify ────────────────────────────────────────────────
  printf("[+] after  (%lu bytes written): %s\n", written, (char*)buf);

  VirtualFree(buf, 0, MEM_RELEASE);
  return 0;
}
