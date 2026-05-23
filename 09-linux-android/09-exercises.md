# 09 exercises

### exercise 1

In `01-linux-hacking-kernel/hack2.c` the kernel module launches `/bin/sh` via `call_usermodehelper`. An interactive shell started from kernel space does not attach to a terminal, so it exits immediately without being useful.

Change the module to run a **non-interactive command** instead: use `/bin/sh -c "id > /tmp/pwned.txt"` to write the effective UID/GID to a file, proving kernel-level code execution.

Required changes:
1. Update `argv[]` to `{"/bin/sh", "-c", "id > /tmp/pwned.txt", NULL}` (three elements before `NULL`).
2. Check for the output file after loading the module: `cat /tmp/pwned.txt`.
3. Update the module description string.

Build and test:
```
make
sudo insmod hack1.ko
cat /tmp/pwned.txt
sudo rmmod hack1
dmesg | tail
```

### exercise 2

In `02-hacking-process-enum/hack.c`, `find_process_by_name` returns only the **PID**. Extend it to also print the **full path** of the executable by reading the symlink `/proc/<pid>/exe` using `readlink(2)`.

Required changes:
1. After a successful PID match, call `readlink("/proc/<pid>/exe", buf, sizeof(buf))`.
2. Print both the PID and the resolved executable path.
3. Handle the case where `readlink` fails (process may have exited between the `/proc` scan and the readlink call).

Expected output:
```
found pid: 1234  exe: /usr/bin/python3
```

### exercise 3

Combine `02-hacking-process-enum/hack.c` with `03-hacking-injection/hack.c`. The current injector (`hack.c`) requires a numeric PID as argument. Change it to accept a **process name** (e.g., `./hack3 meow`) and resolve it to a PID automatically using the `/proc` scan.

Additionally, replace the `execve /bin/sh` payload with a minimal **exit(0)** shellcode so the target process terminates cleanly instead of being replaced by a shell. Write the shellcode in NASM x86-64 assembly (`hack3.asm`) and embed the resulting bytes in `hack3.c`.

Build and test:
```
# compile shellcode (for byte extraction / verification)
nasm -f elf64 hack3.asm -o hack3.o

# compile injector
gcc hack3.c -o hack3

# in terminal 1: start victim
./meow &

# in terminal 2: inject by name
./hack3 meow
```

### exercise 4

In `03-hacking-injection/hack.c`, after `PTRACE_GETREGS` the registers are used directly without being shown to the user. Add a `dump_regs(struct user_regs_struct *regs)` helper that prints the most relevant general-purpose registers **before** the payload is written:

- `RIP` - instruction pointer (where the payload will land)
- `RSP` - stack pointer
- `RAX`, `RBX`, `RCX`, `RDX` - general purpose
- `RDI`, `RSI` - argument registers

Call `dump_regs` right after `PTRACE_GETREGS` succeeds. This lets students observe the exact CPU state of the victim at the moment of injection.

Expected output (example):
```
attaching to process 5678
reading process registers
=== register dump (before injection) ===
  RIP = 0x00007f3a1c2b4e20
  RSP = 0x00007ffe1234abc0
  RAX = 0x00000000000000e7
  ...
injecting payload
```
