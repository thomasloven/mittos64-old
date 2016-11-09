#include <stdint.h>
#include <ports.h>
#include <debug.h>
#include <int.h>

#define MPIC_CMD  0x0020
#define MPIC_DATA 0x0021
#define SPIC_CMD  0x00A0
#define SPIC_DATA 0x00A1

#define PIC_CMD_INIT 0x10
#define PIC_CMD_EXTRA 0x01
#define PIC_CMD_8086 0x01
#define PIC_CMD_EOI 0x20


void pic_init()
{

  // Send initialize command
  outb(MPIC_CMD, PIC_CMD_INIT | PIC_CMD_EXTRA);
  outb(SPIC_CMD, PIC_CMD_INIT | PIC_CMD_EXTRA);

  // Remap offsets
  outb(MPIC_DATA, INT_IRQ0);
  outb(SPIC_DATA, INT_IRQ8);

  // Setup slave PIC
  outb(MPIC_DATA, 0x04);
  outb(SPIC_DATA, 0x02);

  // Extra environment data
  outb(MPIC_DATA, PIC_CMD_8086);
  outb(SPIC_DATA, PIC_CMD_8086);

  // Mask all interrupts
  outb(MPIC_DATA, 0xFF);
  outb(SPIC_DATA, 0xFF);
}

void pic_ack(uint8_t irq)
{
  if(irq >= 8)
    outb(SPIC_CMD, PIC_CMD_EOI);
  outb(MPIC_CMD, PIC_CMD_EOI);
}

void pic_mask(uint8_t irq)
{
  if(irq >= 8)
  {
    outb(SPIC_DATA, inb(SPIC_DATA) | 1 << (irq-8));
  } else {
    outb(MPIC_DATA, inb(MPIC_DATA) | 1 << irq);
  }
}
void pic_unmask(uint8_t irq)
{
  if(irq >= 8)
  {
    outb(SPIC_DATA, inb(SPIC_DATA) & ~(1 << (irq-8)));
  } else {
    outb(MPIC_DATA, inb(MPIC_DATA) & ~(1 << irq));
  }
}
