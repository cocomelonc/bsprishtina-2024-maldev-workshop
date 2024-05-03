/*
 * malware AV evasion
 * hack.c - without hashing WINAPI functions.
 * author: @cocomelonc
*/
#include <windows.h>

int main() {
  MessageBoxA(NULL, "Meow-meow!!", "=^..^=", MB_OK);
  return 0;
}