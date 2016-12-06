#include <int.h>
.intel_syntax noprefix

.extern int_handler
.global isr_common
.global isr_return

isr_common:
  // Push all registers
  push r15
  push r14
  push r13
  push r12
  push r11
  push r10
  push r9
  push r8
  push rbp
  push rdi
  push rsi
  push rdx
  push rcx
  push rbx
  push rax

  # Clear RFLAGS
  pushq 0
  popf

  # Check stored RFLAGS to see if we came here from user mode
  mov rax, [rsp + REG_OFFSET_RFLAGS]
  and rax, RFLAGS_IOPL
  jz .kernel_mode
  swapgs

.kernel_mode:
  mov rdi, rsp
  call int_handler
  mov rdi, rax

isr_return:
  mov rsp, rdi

  # Check stored RFLAGS to see if we are going into user mode
  mov rax, [rsp + REG_OFFSET_RFLAGS]
  and rax, RFLAGS_IOPL
  jz .kernel_return
  swapgs

.kernel_return:
  // Return and restore stack
  pop rax
  pop rbx
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  pop rbp
  pop r8
  pop r9
  pop r10
  pop r11
  pop r12
  pop r13
  pop r14
  pop r15

  // Jump to syscall_return if we're in a syscall
  push rax
  mov rax, [rsp + 8]
  cmp rax, INT_SYSCALL
  .extern syscall_return
  jz syscall_return
  pop rax

  // Pop error code and fault number
  add rsp, 16
  iretq

