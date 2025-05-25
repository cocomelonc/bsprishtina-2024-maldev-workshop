.global _main
.align 2

.text
_main:
  // set x0 to the address of 'foo'
  adrp x0, foo@PAGE
  add  x0, x0, foo@PAGEOFF

  // store 123 into [foo]
  mov  x1, 123
  str  x1, [x0]

  // load from [foo] into x2
  ldr  x2, [x0]

  // exit with value in x2
  mov  x0, x2
  mov  x16, 1
  svc  0

.data
foo: .zero 8
