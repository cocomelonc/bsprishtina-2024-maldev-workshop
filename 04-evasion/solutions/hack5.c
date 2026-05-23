/*
 * exercise 5 solution
 * AV evasion: port API-hashing example from C++ to pure C
 * hack5.c
 * author: @cocomelonc
 *
 * base: 03-winapi-hashing/hack2.c  (compiled with g++, uses nullptr)
 *
 * changes vs original:
 *   1. nullptr  -> NULL   (the only true C++ keyword in the original)
 *   2. added printHash() helper so the magic constant 17036696 is
 *      self-documenting - students see the hash computed at runtime
 *      instead of having to trust a hard-coded number.
 *   3. function pointer cast style updated to the explicit form
 *      accepted by both gcc and g++.
 *   4. compiles cleanly with gcc -std=c99 (no g++ required).
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack5.c -o hack5.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
 */
#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef UINT (CALLBACK* fnMessageBoxA)(
  HWND   hWnd,
  LPCSTR lpText,
  LPCSTR lpCaption,
  UINT   uType
);

// hash function - identical to original hack2.c
DWORD calcMyHash(char* data) {
  DWORD hash = 0x35;
  for (int i = 0; i < (int)strlen(data); i++) {
    hash += data[i] + (hash << 1);
  }
  return hash;
}

/*
 * exercise 5 addition: printHash()
 * prints the hash of a function name so students understand where
 * the magic number used in getAPIAddr() comes from.
 * call printHash("MessageBoxA") at startup to see 17036696.
 */
void printHash(char* name) {
  DWORD h = calcMyHash(name);
  printf("[*] hash(\"%s\") = %lu  (0x%08lX)\n", name, h, h);
}

// PE export-table walk to find a function by hash - identical to original
// exercise 5 change: nullptr -> NULL  (line marked below)
static LPVOID getAPIAddr(HMODULE h, DWORD myHash) {
  PIMAGE_DOS_HEADER      img_dos  = (PIMAGE_DOS_HEADER)h;
  PIMAGE_NT_HEADERS      img_nt   = (PIMAGE_NT_HEADERS)((LPBYTE)h + img_dos->e_lfanew);
  PIMAGE_EXPORT_DIRECTORY img_exp = (PIMAGE_EXPORT_DIRECTORY)(
    (LPBYTE)h + img_nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

  PDWORD fAddr  = (PDWORD)((LPBYTE)h + img_exp->AddressOfFunctions);
  PDWORD fNames = (PDWORD)((LPBYTE)h + img_exp->AddressOfNames);
  PWORD  fOrd   = (PWORD) ((LPBYTE)h + img_exp->AddressOfNameOrdinals);

  for (DWORD i = 0; i < img_exp->AddressOfFunctions; i++) {
    LPSTR pFuncName = (LPSTR)((LPBYTE)h + fNames[i]);
    if (calcMyHash(pFuncName) == myHash) {
      printf("[+] found: %s  hash=%lu\n", pFuncName, myHash);
      return (LPVOID)((LPBYTE)h + fAddr[fOrd[i]]);
    }
  }
  return NULL;   // exercise 5 change: was "nullptr" in the original C++ file
}

int main(void) {
  /*
   * exercise 5 addition: print the hash before using it.
   * output: [*] hash("MessageBoxA") = 17036696  (0x01040598)
   * this makes the magic number self-explanatory.
   */
  printHash("MessageBoxA");

  HMODULE mod = LoadLibrary("user32.dll");
  if (!mod) {
    printf("[-] LoadLibrary(user32.dll) failed\n");
    return 1;
  }

  // 17036696 is calcMyHash("MessageBoxA") with seed 0x35
  LPVOID addr = getAPIAddr(mod, 17036696);
  if (!addr) {
    printf("[-] getAPIAddr failed\n");
    return 1;
  }

  printf("[*] MessageBoxA @ %p\n", addr);

  /*
   * exercise 5 change: function pointer assignment.
   * original (C++ style): fnMessageBoxA myMessageBoxA = (fnMessageBoxA)addr;
   * pure C: use a local variable with an explicit cast.
   * (gcc accepts this with -Wpedantic as a common extension.)
   */
  fnMessageBoxA myMessageBoxA = (fnMessageBoxA)(ULONG_PTR)addr;
  myMessageBoxA(NULL, "Hello, Prishtina!", "=^..^=", MB_OK);

  return 0;
}
