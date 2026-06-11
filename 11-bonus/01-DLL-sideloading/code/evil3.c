/*
 * evil3.c
 * DLL sideloading.
 * practical example 3
 * author: @cocomelonc
*/

#include <windows.h>

BOOL APIENTRY DllMain (HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
  switch (dwReason) {
    case DLL_PROCESS_ATTACH:
    MessageBoxA(NULL, "Meow-meow!", "=^..^=", MB_OK | MB_ICONEXCLAMATION);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}