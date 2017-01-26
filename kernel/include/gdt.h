#pragma once

#define SEG_KCODE 0x08
#define SEG_KDATA 0x10
#define SEG_UDATA 0x18
#define SEG_UCODE 0x20

#ifdef __ASSEMBLER__
#define   GDT_WRITE      (1<<41)
#define   GDT_EXECUTE    (1<<43)
#define   GDT_CODEDATA   (1<<44)
#define   GDT_PRESENT    (1<<47)
#define   GDT_64BIT      (1<<53)
#else
#define   GDT_WRITE      (1LL<<41)
#define   GDT_EXECUTE    (1LL<<43)
#define   GDT_CODEDATA   (1LL<<44)
#define   GDT_PRESENT    (1LL<<47)
#define   GDT_64BIT      (1LL<<53)
#define   GDT_RING3      (3LL<<45)
#define   GDT_TSS        (9LL<<40)

#include <stdint.h>

extern uint64_t BootGDT;

struct gdtp_st
{
  uint16_t len;
  uint64_t addr;
  uint16_t pad1;
  uint32_t pad2;
}__attribute__((packed));

void load_gdt(struct gdtp_st *);
void gdt_init();
#endif
