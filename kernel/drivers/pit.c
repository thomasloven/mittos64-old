#include <timer.h>
#include <int.h>
#include <pit.h>
#include <debug.h>
#include <ports.h>
#include <apic.h>
#include <stdint.h>

#define PIT_CH0 0x40
#define PIT_CH2 0x42
#define PIT_CMD 0x43

#define PIT_USE_CH0 0x00
#define PIT_USE_CH2 0x80
#define PIT_RW_LOHI 0x30
#define PIT_MODE(M) ((M)<<1)

#define PIT_FREQ 1193182

#define CH2_PORT 0x61
#define CH2_ENABLE 0x01
#define CH2_OUT 0x20


void delay(uint8_t ms)
{
  // Turn off channel 2
  uint8_t ch2 = inb(CH2_PORT);
  ch2 &= ~CH2_ENABLE;
  outb(CH2_PORT, ch2);

  // Set up a countdown on channel 2
  outb(PIT_CMD, PIT_USE_CH2 | PIT_RW_LOHI | PIT_MODE(0));
  uint16_t count = PIT_FREQ * ms/1000;
  outb(PIT_CH2, count & 0xFF);
  outb(PIT_CH2, (count >> 8) & 0xFF);

  // Turn on channel 2
  ch2 |= CH2_ENABLE;
  outb(CH2_PORT, ch2);

  // Busy wait (Ultimately, we'll only use this a few times during boot)
  while(!(inb(CH2_PORT) & CH2_OUT));
}

void pit_delay(uint32_t ms)
{
  while(ms > 50)
  {
    delay(50);
    ms -= 50;
  }
  if(ms)
    delay(ms);
}

uint64_t system_ticks = 0;
registers_t *timer_tick(registers_t *r)
{
  apic_ack();
  system_ticks ++;
  return r;
}

uint64_t get_ticks()
{
  return system_ticks;
}

void pit_init()
{
  register_int_handler(IRQ_INT(IRQ_TIMER), timer_tick);
  outb(PIT_CMD, PIT_USE_CH0 | PIT_RW_LOHI | PIT_MODE(3));
  uint16_t count = PIT_FREQ/TICK_FREQ;
  outb(PIT_CH0, (count & 0xFF));
  outb(PIT_CH0, (count >> 8));
  IRQ_UNMASK(IRQ_TIMER);
}
