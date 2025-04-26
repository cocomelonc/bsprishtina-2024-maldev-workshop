.section .text
.global _start

_start:

create:
    adr r0, filename
    mov r1, #1 //WRITE ONLY
    mov r7, #0x8 //CREAT SYS CALL
    svc #0

    //The file descriptor(fd) is returned into the r0 variable.
    //Store the file descriptor to the stack.
    //This way, we can reuse r0 for other functions and when the fd is needed,
    //we simply pop from the stack
    push {r0}

write:
    mov r0, r0 //r0 already contains the file descriptor
    adr r1, toWrite //buffer
    mov r2, #1 //write only 1 byte
    mov r7, #0x04 //syscall for write
    svc #0

close:
    pop {r0} //pop the file descriptor back
    mov r7, #0x06 //syscall for close
    svc #0

branch:
    //end of this function, lets branch back
    bx lr


filename:
    .asciz "is_admin"

toWrite:
    .asciz "1"
