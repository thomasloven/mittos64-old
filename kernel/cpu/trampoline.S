#include <mem.h>
#include <cpu.h>
#include <msr.h>
#define TRAMPOLINE_OFFSET TRAMPOLINE_ADDR - trampoline

.intel_syntax noprefix
.global trampoline
.section .text
.code16
trampoline:

  cli
  mov ax, 0x0
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  lidt [ap_boot_idt + TRAMPOLINE_OFFSET]
  lgdt [ap_boot_gdtp + TRAMPOLINE_OFFSET]

  mov eax, cr0
  or al, 0x1
  mov cr0, eax

  jmp 0x8:start_32 + TRAMPOLINE_OFFSET

.code32
start_32:
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  mov eax, cr4
  or eax, 1<<5
  mov cr4, eax

  mov eax, offset V2P(BootP4)
  mov cr3, eax

  mov ecx, 0xC0000080
  rdmsr
  or eax, 1<<8
  wrmsr

  mov eax, cr0
  or eax, 1<<31
  mov cr0, eax

  lgdt V2P(BootGDTp)

  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov ss, ax

  jmp 0x8:start_64 + TRAMPOLINE_OFFSET
.code64
start_64:
  movabs rax, offset start_longmode
  jmp rax
start_longmode:

  movabs rax, ap_gs_base
  mov rdx, rax
  shr rdx, 32
  mov ecx, MSR_REG_KERNEL_GS
  wrmsr
  swapgs

  movq [gs:GS_OFFSET_STATE], CPU_SIPI_REC
.ap_sync_loop:
  cmpq [gs:GS_OFFSET_STATE], CPU_SIPI_REC
  je .ap_sync_loop

  mov rsp, gs:GS_OFFSET_STACK
  push 0
  mov rbp, rsp
  push 0
  popf

  .extern ap_start
  call ap_start


.align 16
ap_boot_gdt:
  .long 0x00000000, 0x00000000
  .long 0x0000FFFF, 0x00CF9A00
  .long 0x0000FFFF, 0x00CF9200
.align 16
ap_boot_gdtp:
  .short 3*8-1
  .long ap_boot_gdt + TRAMPOLINE_OFFSET

.align 16
ap_boot_idt:
  .short 0
  .long 0
  .long 0

.global ap_gs_base
ap_gs_base:
  .long 0x00000000, 0x00000000
trampoline_end:
  nop
