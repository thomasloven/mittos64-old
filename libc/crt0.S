 .intel_syntax noprefix
 .section .text
 .global _start

 _start:
  movq rbp, 0
  push rbp
  push rbp
  mov rbp, rsp

  push rsi
  push rdi

  call _init

  pop rdi
  pop rsi

  call main

  call _fini

  jmp $

.size _start, . - _start
