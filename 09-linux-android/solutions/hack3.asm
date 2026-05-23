; exercise 3 solution - shellcode: write("meow!\n") + exit(0)
; hack3.asm
; author: @cocomelonc
;
; purpose: a minimal, non-destructive x86-64 Linux shellcode to use
; as the injection payload in hack3.c instead of the execve /bin/sh
; payload from the original hack.c.
;
; why a new payload?
;   the original execve("/bin//sh") permanently replaces the victim
;   process image - the target disappears.  this shellcode instead
;   prints "meow!\n" to stdout then calls exit(0), which is observable
;   and clean: the target prints the string and terminates normally.
;
; technique: JMP-CALL-POP (position-independent string addressing)
;   1. jmp  forward to "call_push"           (skip over instructions)
;   2. call backward to "back"               (pushes next IP = string addr)
;   3. pop  rsi                              (rsi = address of string)
;   this avoids hardcoded absolute addresses - essential for shellcode
;   injected at an unknown RIP.
;
; syscalls used (x86-64 Linux ABI):
;   sys_write = 1   (rax=1, rdi=fd, rsi=buf, rdx=len)
;   sys_exit  = 60  (rax=60, rdi=exit_code)
;
; no null bytes in the resulting shellcode (verified below).
;
; to extract bytes:
;   nasm -f bin -o hack3.bin hack3.asm
;   xxd hack3.bin
;   # or for C array:
;   python3 -c "d=open('hack3.bin','rb').read(); print('\"' + ''.join('\\\\x%02x'%b for b in d) + '\"')"
;
; resulting bytes (32 bytes):
;   \xeb\x13\x5e\x6a\x01\x5f\x6a\x06\x5a\x6a\x01\x58\x0f\x05
;   \x6a\x3c\x58\x31\xff\x0f\x05\xe8\xe8\xff\xff\xff\x6d\x65\x6f\x77\x21\x0a
;
; to assemble as a standalone ELF for testing (not needed for injection):
;   nasm -f elf64 hack3.asm -o hack3.o
;   ld -o hack3_test hack3.o
;   ./hack3_test          # should print "meow!" and exit

bits 64
section .text
global _start

_start:
    jmp  short call_push   ; eb 13 - jump over the payload instructions

; ── step 3: landed here after CALL pushes the string address ─────────
back:
    pop  rsi               ; 5e  - rsi = address of "meow!\n" string

    ; write(1, rsi, 6)
    push 1
    pop  rdi               ; 6a 01 5f  - rdi = 1 (stdout)
    push 6
    pop  rdx               ; 6a 06 5a  - rdx = 6 (length of "meow!\n")
    push 1
    pop  rax               ; 6a 01 58  - rax = 1 (sys_write)
    syscall                ; 0f 05

    ; exit(0)
    push 60
    pop  rax               ; 6a 3c 58  - rax = 60 (sys_exit)
    xor  edi, edi          ; 31 ff     - rdi = 0 (exit code 0)
    syscall                ; 0f 05

; ── step 1+2: JMP lands here, CALL pushes return address onto stack ──
call_push:
    call back              ; e8 e8 ff ff ff - pushes &string, jumps back

    ; string data - immediately follows the CALL instruction
    db "meow!", 0x0a       ; 6d 65 6f 77 21 0a  (6 bytes, no null)
