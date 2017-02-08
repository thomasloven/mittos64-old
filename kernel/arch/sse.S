#include <registers.h>

.intel_syntax noprefix

.global sse_init
sse_init:
  mov rax, cr0
  and ax, 0xFFFF-CR0_EM
  or ax, CR0_MP
  mov cr0, rax

  mov rax, cr4
  or ax, CR4_OSFXSR
  or ax, CR4_OSXMMEXCPT
  mov cr4, rax

  ret

.global sse_save
sse_save:
  movabs rax, offset _sse_buffer
  fxsave [rax]
  movabs rsi, offset _sse_buffer
  mov rdx, 512
  call memcpy
  ret

.global sse_restore
sse_restore:
  mov rsi, rdi
  movabs rdi, offset _sse_buffer
  mov rdx, 512
  call memcpy
  movabs rax, offset _sse_buffer
  fxrstor [rax]
  ret


.section .data
.align 16
_sse_buffer:
 .rept 512
 .byte 0
 .endr

