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

unsigned int irq_map[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};

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
