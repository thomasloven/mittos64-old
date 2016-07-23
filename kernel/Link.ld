/* Guidance from http://wiki.osdev.org/Bare_Bones */
ENTRY(_start)

KERNEL_OFFSET = 0xFFFFFF8000000000;
KERNEL_START = 0x100000;

SECTIONS
{
  . = KERNEL_START + KERNEL_OFFSET;

  .text : AT(ADDR(.text) - KERNEL_OFFSET)
  {
    *(.multiboot) /* Multiboot header must be in first 4k of elf */
    *(.text)
    . = ALIGN(4096);
  }

  .rodata BLOCK(4K) :AT(ADDR(.rodata) - KERNEL_OFFSET)
  {
    *(.rodata*)
  }

  .data BLOCK(4K) :AT(ADDR(.data) - KERNEL_OFFSET)
  {
    *(.data)
  }

  .bss BLOCK(4K) :AT(ADDR(.bss) - KERNEL_OFFSET)
  {
    *(.COMMON)
    *(.bss)
  }
}
