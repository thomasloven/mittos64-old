#include <gdt.h>
#include <stdint.h>
#include <debug.h>

uint64_t gdt[5];
struct gdtp_st gdt_p;
#define GDT gdt
#define GDTP gdt_p


void gdt_init()
{

  GDT[0] = 0;
  GDT[SEG_KCODE/8] = (uint64_t)(GDT_PRESENT | GDT_CODEDATA | GDT_WRITE | GDT_EXECUTE | GDT_64BIT);
  GDT[SEG_KDATA/8] = (GDT_PRESENT | GDT_CODEDATA | GDT_WRITE);
  GDT[SEG_UCODE/8] = (GDT_PRESENT | GDT_CODEDATA | GDT_WRITE | GDT_EXECUTE | GDT_64BIT | GDT_RING3);
  GDT[SEG_UDATA/8] = (GDT_PRESENT | GDT_CODEDATA | GDT_WRITE | GDT_RING3);

  GDTP.len = 5*8-1;
  GDTP.addr = (uint64_t)&GDT[0];

  load_gdt(&GDTP);

}
