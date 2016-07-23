#pragma once

#define KERNEL_OFFSET 0xFFFFFF8000000000

#ifdef __ASSEMBLER__
  #define V2P(a) ((a) - KERNEL_OFFSET)
  #define P2V(a) ((a) + KERNEL_OFFSET)
  #define P1_OFFSET(p) (((p) >> 12) & 0x1FF)
  #define P2_OFFSET(p) (((p) >> 21) & 0x1FF)
  #define P3_OFFSET(p) (((p) >> 30) & 0x1FF)
  #define P4_OFFSET(p) (((p) >> 39) & 0x1FF)
#else
  #include <stdint.h>
  #define V2P(a) ((uintptr_t)(a) - KERNEL_OFFSET)
  #define P2V(a) ((void *)((uintptr_t)(a) + KERNEL_OFFSET))
#endif

// Paging
#define   PAGE_PRESENT        0x001
#define   PAGE_WRITE          0x002
#define   PAGE_USER           0x004
#define   PAGE_WRITETHROUGH   0x008
#define   PAGE_NOCACHE        0x010
#define   PAGE_ACCESSED       0x020
#define   PAGE_DIRTY          0x040
#define   PAGE_HUGE           0x080
#define   PAGE_GLOBAL         0x100
