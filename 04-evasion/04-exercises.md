# 04 exercises

### exercise 1

In `01-encryption/hack2.c` the payload is XOR-encrypted with the key `"secret"`. Change the key to `"meow"`. Re-encrypt the raw payload (`hello.bin`) using the helper script `xor.py` (update `my_secret_key = "meow"`) to obtain the new ciphertext bytes, replace the `payload[]` array in the C source with the new bytes, and update `secretKey[]` to `"meow"`. Compile and run - the MessageBox must still appear.

### exercise 2

In `02-func-call-obfuscation/hack2.c`, only `VirtualAlloc` is resolved through an XOR-obfuscated name. Apply the same technique to `VirtualProtect`:

1. Declare a matching function pointer type for `VirtualProtect`.
2. XOR-encrypt the string `"VirtualProtect"` with the same key `"secret123"`.
3. Decrypt the name at runtime, resolve it via `GetProcAddress`, and call it through the pointer instead of the direct `VirtualProtect(...)` call.

### exercise 3

In `04-syscall/hack2.c`, the function `printSyscallStub` dumps 23 raw bytes and the student has to visually locate the SSN. Add a helper `parseSyscallNumber(char* funcName)` that:

1. Loads `ntdll.dll` from disk (same `DONT_RESOLVE_DLL_REFERENCES` trick).
2. Reads `bytes[4]` from the stub - this is the low byte of the `mov eax, <SSN>` instruction.
3. Prints the function name and the parsed SSN in decimal and hex.

Call it for at least three Nt functions: `NtAllocateVirtualMemory`, `NtWriteVirtualMemory`, `NtCreateThreadEx`.

### exercise 4

Based on `05-syscall-2/syscall.asm` and `hack.c`. The existing assembly stub calls `NtAllocateVirtualMemory` (SSN `0x18`). Add a second stub `myNtWriteVirtualMemory` (SSN `0x3A` on Windows 10 20H2+) to the same `.asm` file. Then write `hack4.c` that:

1. Allocates a buffer in the current process with `VirtualAlloc` filled with `'A'` bytes.
2. Calls `myNtWriteVirtualMemory(GetCurrentProcess(), buffer, src, size, NULL)` to overwrite it with a different string.
3. Prints the buffer before and after to prove the write worked.

Compile: `nasm -f win64 hack4.asm -o hack4_asm.o && x86_64-w64-mingw32-gcc hack4.c hack4_asm.o -o hack4.exe`

### exercise 5

`03-winapi-hashing/hack2.c` is written in C++ (it uses `nullptr`, C++ cast syntax, and is typically compiled with `g++`). Port it to **pure C**:

1. Replace `nullptr` with `NULL`.
2. Add a `printHash(char* name)` helper that calls `calcMyHash` and prints the result - so students see exactly where the magic constant `17036696` comes from, without needing an external script.
3. Call `printHash("MessageBoxA")` at the top of `main` before using the hash.
4. Ensure the file compiles cleanly with `gcc` (not `g++`).
