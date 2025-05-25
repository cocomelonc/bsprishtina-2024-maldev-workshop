.global _main
.align 2

.text
_main:
  // prepare syscall for write()
  mov  x0, 1              // stdout
  mov  x2, 14             // length
  adrp x1, hello_txt@PAGE // message address
  add  x1, x1, hello_txt@PAGEOFF
  mov  x16, 4             // syscall: write
  svc  0

  // Exit syscall
  mov  x16, 1
  svc  0

.data
hello_txt: .ascii "Hello, World!\n"