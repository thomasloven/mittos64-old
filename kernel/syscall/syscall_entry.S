#include <mem.h>
#include <gdt.h>
#include <int.h>
#include <cpu.h>
.intel_syntax noprefix


.global syscall_entry
.global syscall_return
.extern syscall_handler

syscall_entry:

  swapgs
  mov [gs:GS_OFFSET_SCTEMP], rsp
  mov rsp, [gs:GS_OFFSET_STACK]

  // Prepare a fake interrupt stack
  pushq SEG_UDATA // SS
  push [gs:GS_OFFSET_SCTEMP] // RSP
  push r11 // RFLAGS
  pushq SEG_UCODE // CS
  push rcx // RIP
  pushq 0 // Error code
  pushq INT_SYSCALL // Interrupt id
  swapgs

  jmp isr_common
syscall_return:

  pop rax
  add rsp, 16
  pop rcx
  add rsp, 8
  pop r11
  pop rsp

  sysretq
