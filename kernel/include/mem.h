#pragma once

#define KERNEL_OFFSET 0xFFFFFF8000000000
#define KERNEL_HEAP_S 0xFFFFFFC000000000

#ifdef __ASSEMBLER__
  #define V2P(a) ((a) - KERNEL_OFFSET)
  #define P2V(a) ((a) + KERNEL_OFFSET)
  #define PAGE_OFFSET(p) ((p) & 0xFFF)
  #define P1_OFFSET(p) (((p) >> 12) & 0x1FF)
  #define P2_OFFSET(p) (((p) >> 21) & 0x1FF)
  #define P3_OFFSET(p) (((p) >> 30) & 0x1FF)
  #define P4_OFFSET(p) (((p) >> 39) & 0x1FF)
#else
  #include <stdint.h>
  #define V2P(a) ((uintptr_t)(a) - KERNEL_OFFSET)
  #define P2V(a) ((void *)((uintptr_t)(a) + KERNEL_OFFSET))
  // returns an address n bytes after the target of pointer p
  #define incptr(p, n) ((void *)(((uintptr_t)(p)) + (n)))
  #define PAGE_OFFSET(p) (((uintptr_t)(p)) & 0xFFF)
  #define P1_OFFSET(p) ((((uintptr_t)(p)) >> 12) & 0x1FF)
  #define P2_OFFSET(p) ((((uintptr_t)(p)) >> 21) & 0x1FF)
  #define P3_OFFSET(p) ((((uintptr_t)(p)) >> 30) & 0x1FF)
  #define P4_OFFSET(p) ((((uintptr_t)(p)) >> 39) & 0x1FF)
#endif

#define PAGE_SIZE 0x1000
#define PAGE_FLAGS_MASK 0xFFF
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

#ifndef __ASSEMBLER__
#include <stddef.h>

typedef void * page_table;
extern page_table BootP4;

extern int kernel_start, kernel_end;

void pmm_init();
uintptr_t pmm_alloc();
void pmm_free(uintptr_t page);

#define GET_P4() ((page_table)V2P(&BootP4))

void vmm_init();
page_table *vmm_new_P4();
void vmm_set_P4(page_table *P4);
void vmm_free_P4(page_table *P4);
uintptr_t vmm_get_page(page_table *P4, uintptr_t addr);
uintptr_t vmm_set_page(page_table *P4, uintptr_t addr, uintptr_t phys, uint32_t flags);

void kfree(void *a);
void *kmalloc(size_t size);
void *kcalloc(size_t count, size_t size);
void *krealloc(void *ptr, size_t size);
void heap_print();
#endif
