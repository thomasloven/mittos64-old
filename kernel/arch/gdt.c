#include <gdt.h>
#include <cpu.h>
#include <stdint.h>
#include <debug.h>
#include <cpu.h>

#define GDT cpu->gdt
#define GDTP cpu->gdt_p


void gdt_init()
{
  cpu_t *cpu = get_cpu();
  cpu->tss.io_mba = sizeof(tss_t);

  GDT[0] = 0;
  GDT[SEG_KCODE/8] = (uint64_t)(GDT_PRESENT | GDT_CODEDATA | GDT_WRITE | GDT_EXECUTE | GDT_64BIT);
  GDT[SEG_KDATA/8] = (GDT_PRESENT | GDT_CODEDATA | GDT_WRITE);
  GDT[SEG_UCODE/8] = (GDT_PRESENT | GDT_CODEDATA | GDT_WRITE | GDT_EXECUTE | GDT_64BIT | GDT_RING3);
  GDT[SEG_UDATA/8] = (GDT_PRESENT | GDT_CODEDATA | GDT_WRITE | GDT_RING3);
  GDT[SEG_TSS/8] = (GDT_PRESENT | GDT_TSS | TSS_BLO((uint64_t)&cpu->tss) | TSS_LIM(sizeof(tss_t)));
  GDT[SEG_TSS/8 + 1] = (TSS_BHI((uint64_t)&cpu->tss));

  GDTP.len = 7*8-1;
  GDTP.addr = (uint64_t)&GDT[0];

  load_gdt(&GDTP);
  load_tr(SEG_TSS);
}
