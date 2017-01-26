.intel_syntax noprefix
#include <gdt.h>

.global load_idt
load_idt:
  lidt [rdi]
  ret

.global read_cr0
read_cr0:
  mov rax, cr0
  ret
.global read_cr2
read_cr2:
  mov rax, cr2
  ret
.global read_cr3
read_cr3:
  mov rax, cr3
  ret
.global write_cr3
write_cr3:
  mov cr3, rdi
  ret
.global read_cr4
read_cr4:
  mov rax, cr4
  ret

.global load_gdt
load_gdt:
  lgdt [rdi]
  mov ax, SEG_KDATA
  mov ss, ax
  mov ds, ax
  mov es, ax
  movabs rax, offset .load_gdt
  pushq SEG_KCODE
  push rax
  retfq # perform long jump to SEG_KCODE:.load_gdt
.load_gdt:
  ret

