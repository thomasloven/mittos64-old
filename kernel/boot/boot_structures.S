.intel_syntax noprefix
#include <mem.h>
#include <gdt.h>


// Hardcode the initial page mapping
// Identity map the first GiB of memory
// Also map the same memory at KERNEL_OFFSET
.section .data
.align 0x1000
.global BootP4
BootP4:
  .quad V2P(BootP3) + (PAGE_PRESENT | PAGE_WRITE)
  .rept P4_OFFSET(KERNEL_OFFSET)-1
    .quad 0x0
  .endr
  .quad V2P(BootP3) + (PAGE_PRESENT | PAGE_WRITE)
  .rept 510 - P4_OFFSET(KERNEL_OFFSET) + 1
    .quad 0x0
  .endr
BootP3:
  .quad V2P(BootP2) + (PAGE_PRESENT | PAGE_WRITE)
  .rept 511
    .quad 0x0
  .endr
BootP2:
  .set i, 0
  .rept 512
    .quad (i << 21) + (PAGE_PRESENT | PAGE_WRITE | PAGE_HUGE)
    .set i, (i+1)
  .endr


// Hardcode initial GDT segments
// Only kernel code (0x8) and data (0x10) for now
.section .rodata
.align 0x1000
.global BootGDT
.global BootGDTp
BootGDT:
  .quad 0
  .quad (GDT_PRESENT | GDT_CODEDATA | GDT_WRITE | GDT_EXECUTE | GDT_64BIT)
  .quad (GDT_PRESENT | GDT_CODEDATA | GDT_WRITE)
BootGDTp: // We can't put the gdt pointer in the gdt, it's too big
  .short 3*8-1
  .quad V2P(BootGDT)


// Reserved space for the stack
.section .bss
.global BootStack
.align 0x1000
  .skip 0x1000
BootStack:
