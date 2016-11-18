#include <apic.h>
#include <pit.h>
#include <msr.h>
#include <debug.h>
#include <mem.h>
#include <scheduler.h>

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

#define TIMER_LVT_ONESHOT  0x00000
#define TIMER_LVT_PERIODIC 0x20000
#define TIMER_LVT_DEADLINE 0x40000

#define R_TIMER_INIT  0x380
#define R_TIMER_CURR  0x390
#define R_TIMER_DIV   0x3E0
#define TIMER_DIV1    0xB
#define TIMER_DIV2    0x0
#define TIMER_DIV4    0x1
#define TIMER_DIV8    0x2
#define TIMER_DIV16   0x3
#define TIMER_DIV32   0x8
#define TIMER_DIV64   0x9
#define TIMER_DIV128  0xA


#define APIC(reg) apic[reg/4]
uint32_t volatile *apic = P2V(APIC_BASE);

uint32_t apic_ticks_per_us;

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
registers_t *apic_timer_handler(registers_t *r)
{
  // APIC timer timeout occurred
  APIC(R_EOI) = 0;
  schedule();
  return r;
}
void apic_timer(uint64_t us)
{
  APIC(R_TIMER_DIV) = TIMER_DIV1;
  APIC(R_TIMER_INIT) = us*apic_ticks_per_us;
  APIC(R_TIMER_LVT) = TIMER_LVT_ONESHOT | INT_APIC_TIMER;
}
void apic_timer_stop()
{
  APIC(R_TIMER_LVT) = LVT_MASKED | INT_APIC_TIMER;
}

uint32_t calibrate_apic_timer(uint32_t resolution)
{
  // Use the PIT to calibrate the APIC timer
  // Returns number of apic timer ticks per ms
  APIC(R_TIMER_DIV) = TIMER_DIV1;
  APIC(R_TIMER_LVT) = LVT_MASKED | INT_APIC_TIMER;
  APIC(R_TIMER_INIT) = 0xFFFFFFFF;
  pit_delay(resolution);
  uint32_t ticks = APIC(R_TIMER_CURR);
  return (0xFFFFFFFF-ticks)/resolution/1000;
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

  // Calibrate timer
  apic_ticks_per_us = calibrate_apic_timer(100);
  debug_info("APIC - ticks per us:%d\n", apic_ticks_per_us);
  debug("  corresponds to processor frequency: %d MHz\n", apic_ticks_per_us);

  // Register temporary timer handler to go off every 10 ms
  register_int_handler(INT_APIC_TIMER, apic_timer_handler);
}
