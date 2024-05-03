/*
 * Malware Persistence 101
 * hack.c
 * evil app for windows
 * persistence via powershell profile
 * author: @cocomelonc
*/
#include <windows.h>
#pragma comment (lib, "user32.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  MessageBox(NULL, "Hello, Prishtina!", "=^..^=", MB_OK);
  return 0;
}

