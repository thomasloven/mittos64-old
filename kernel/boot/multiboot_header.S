.section .multiboot
#include <multiboot.h>


.align 0x1000
MultiBootHeader1:
  .long MBOOT1_MAGIC
  .long MBOOT1_HEADER_FLAGS
  .long MBOOT1_HEADER_CHECKSUM


.align 0x1000
MultiBootHeader:
  .long MBOOT2_MAGIC
  .long MBOOT2_ARCH
  .long MBOOT2_LENGTH
  .long MBOOT2_CHECKSUM
// Multiboot tags
  // End tag
  .short 0
  .short 0
  .long 8
MultiBootHeaderEnd:
