; exercise 4 solution - assembly stubs
; hack4.asm
; author: @cocomelonc
;
; base: 05-syscall-2/syscall.asm  (only had NtAllocateVirtualMemory)
;
; additions: myNtWriteVirtualMemory stub (SSN 0x3A on Windows 10 20H2+)
;
; x64 Windows syscall calling convention:
;   rcx = arg1, rdx = arg2, r8 = arg3, r9 = arg4
;   stack args start at [rsp+28h] for arg5+
;   "mov r10, rcx" is required because the kernel uses r10 for the
;   return address during a syscall; rcx is clobbered by syscall itself.
;
; SSN values are OS/build-specific.  Reference (Windows 10 20H2 x64):
;   NtAllocateVirtualMemory  0x18
;   NtWriteVirtualMemory     0x3A
;
; compile:
;   nasm -f win64 hack4.asm -o hack4_asm.o
; link with hack4.c:
;   x86_64-w64-mingw32-gcc hack4.c hack4_asm.o -o hack4.exe

section .text

; ── original stub from 05-syscall-2/syscall.asm ───────────────────────
global myNtAllocateVirtualMemory
myNtAllocateVirtualMemory:
    mov  r10, rcx
    mov  eax, 0x18          ; SSN: NtAllocateVirtualMemory (Win10 20H2)
    syscall
    ret

; ── exercise 4 addition ───────────────────────────────────────────────
; NtWriteVirtualMemory(
;   HANDLE  ProcessHandle,      rcx
;   PVOID   BaseAddress,        rdx
;   PVOID   Buffer,             r8
;   ULONG   NumberOfBytesToWrite, r9
;   PULONG  NumberOfBytesWritten  [rsp+28h]
; )
global myNtWriteVirtualMemory
myNtWriteVirtualMemory:
    mov  r10, rcx
    mov  eax, 0x3a          ; SSN: NtWriteVirtualMemory (Win10 20H2)
    syscall
    ret
