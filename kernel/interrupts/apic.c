#include <apic.h>
#include <int.h>
#include <msr.h>
#include <debug.h>
#include <mem.h>

#define APIC_MSR_ENABLE 0x800

#define APIC_BASE 0xFEE00000

#define R_ID          0x020
#define R_VERSION     0x030
#define R_EOI         0x0B0
#define R_LDR         0x0D0
#define R_SPURIOUS    0x0F0
#define R_INT_CMD_LO  0x300
#define R_INT_CMD_HI  0x310
#define R_TIMER_LVT   0x320
#define R_THERMAL_LVT 0x330
#define R_PERF_LVT    0x340
#define R_LINT0_LVT   0x350
#define R_LINT1_LVT   0x360
#define R_ERROR_LVT   0x370
#define LVT_MASKED    0x10000

#define SPURIOUS_LVT_ENABLED  0x100

#define APIC(reg) apic[reg/4]
uint32_t volatile *apic = P2V(APIC_BASE);

void apic_interrupt(uint8_t destination, uint8_t level, uint8_t type, uint8_t vector)
{
  uint64_t data = ((level & 0x1)<<14) | ((type & 0x7)<<8) | vector;
  APIC(R_INT_CMD_HI) = destination << 24;
  APIC(R_INT_CMD_LO) = data;
}

void apic_ack()
{
  APIC(R_EOI) = 0;
}

void apic_init()
{
  debug_info("APIC - APIC_BASE MSR: %x\n", msr_read(MSR_APIC_BASE));
  // Enable APIC by setting the enable bit in the APIC MSR
  msr_write(MSR_APIC_BASE, msr_read(MSR_APIC_BASE) | APIC_MSR_ENABLE);

  // Make sure the APIC base addres is mapped in kernel memory
  vmm_set_page(0, (uintptr_t)P2V(APIC_BASE), APIC_BASE, PAGE_PRESENT | PAGE_WRITE);

  uint8_t id = APIC(R_ID) >> 24;
  if(id <= 0)
    APIC(R_LDR) = 1 << (24 + id);

  APIC(R_SPURIOUS) = SPURIOUS_LVT_ENABLED | INT_APIC_SPUR;
  APIC(R_EOI) = 0;
}
