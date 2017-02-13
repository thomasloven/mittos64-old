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

  pop rsi
  pop rdx
  movabs rax, offset main
  mov rdi, rax

  call __libc_start_main

  call _fini

  jmp $

.size _start, . - _start
