# 03 exercises

### exercise 1

In the code of shellcode injection, change the shellcode payload to a simple "No-Operation" (`NOP/0x90`) sled and verify that the program exits without crashing.       

### exercise 2

In the source code of shellcode injection, update the code to launch two separate threads injections, each running a different injection (e.g., one for `mspaint.exe` and one for `notepad.exe`).      

### exercise 3

In the DLL injector (`02-dll/hack.c`) the target process ID is passed as a raw integer argument (`hack.exe 1234`). Modify the injector to accept a **process name** instead (`hack.exe notepad.exe`). Use `CreateToolhelp32Snapshot`, `Process32First`, and `Process32Next` to resolve the name to a PID at runtime. If the process is not found, print an error and exit. Compile and test against a running `notepad.exe`.

### exercise 4

In the shellcode injection code (`01-shellcode/hack.c`), remote memory is allocated with `PAGE_EXECUTE_READWRITE` - a region that is simultaneously writable **and** executable. This single flag is a strong indicator for AV/EDR engines. Change the code to use the **W^X** (Write XOR Execute) pattern:

1. Allocate the remote buffer with `PAGE_READWRITE` (no execute permission yet).
2. Copy the shellcode with `WriteProcessMemory`.
3. Call `VirtualProtectEx` to flip the protection to `PAGE_EXECUTE_READ` (no write, execute allowed).
4. Only then call `CreateRemoteThread` to run the payload.

