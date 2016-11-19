#include <cpu.h>
.intel_syntax noprefix
.global get_cpu
get_cpu:
  mov rax, gs:GS_OFFSET_CPU
  ret
