#pragma once

#define SEG_KCODE 0x08
#define SEG_KDATA 0x10
#define SEG_UDATA 0x18
#define SEG_UCODE 0x20
#define   SEG_TSS        0x28

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

#define   TSS_BLO(base)  ((((base)&0xFFFF)<<16) | ((((base)>>16)&0xFF)<<32) | ((((base)>>24)&0xFF)<<56))
#define   TSS_BHI(base)  (((base)>>32)&0xFFFFFFFF)
#define   TSS_LIM(lim)   (((lim)&0xFFFF) | ((((lim)>>16)&0xF)<<48))

#include <stdint.h>

extern uint64_t BootGDT;

struct gdtp_st
{
  uint16_t len;
  uint64_t addr;
  uint16_t pad1;
  uint32_t pad2;
}__attribute__((packed));

typedef struct
{
  uint32_t r1;
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t r2;
  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;
  uint64_t r3;
  uint16_t r4;
  uint16_t io_mba;
}__attribute__((packed)) tss_t;

void load_gdt(struct gdtp_st *);
void gdt_init();

void load_tr(uint32_t segment);
#endif
