#include <apic.h>
#include <mem.h>
#include <debug.h>

#define IOAPIC_BASE 0xFEC00000

#define IOAPIC_DEST_PHYSICAL   0x0
#define IOAPIC_DEST_LOGICAL    0x8
#define IOAPIC_DELIVERY_FIXED  0x0
#define IOAPIC_DELIVERY_LOWEST 0x1


volatile uint32_t *ioregsel = (void *)P2V(IOAPIC_BASE);
volatile uint32_t *iowin = (void *)P2V(IOAPIC_BASE + 0x10);

uint32_t ioapic_read(uint32_t reg)
{
  // Read value from APIC register
  *ioregsel = reg;
  return *iowin;
}
void ioapic_write(uint32_t reg, uint32_t value)
{
  // Read value from APIC register
  *ioregsel = reg;
  *iowin = value;
}

struct ioapic_redirection
{
  union
  {
    struct
    {
      uint32_t data[2];
    }__attribute__((packed));
    struct
    {
      uint8_t vector;
      uint8_t mode;
      uint8_t mask;
      uint32_t reserved;
      uint8_t target;
    }__attribute__((packed));
  };
};

void ioapic_readv(uint8_t irq, struct ioapic_redirection *v)
{
  v->data[0] = ioapic_read(0x10 + 2*irq);
  v->data[1] = ioapic_read(0x11 + 2*irq);
}
void ioapic_writev(uint8_t irq, struct ioapic_redirection *v)
{
  ioapic_write(0x10 + 2*irq, v->data[0]);
  ioapic_write(0x11 + 2*irq, v->data[1]);
}

void ioapic_route(uint8_t irq, uint8_t mode, uint8_t target, uint8_t vector)
{
  struct ioapic_redirection v;
  ioapic_readv(irq, &v);
  v.target = target;
  v.vector = vector;
  v.mode = mode;
  ioapic_writev(irq, &v);
}

void ioapic_mask(uint8_t irq)
{
  struct ioapic_redirection v;
  ioapic_readv(irq, &v);
  v.mask |= 1;
  ioapic_writev(irq, &v);
}
void ioapic_unmask(uint8_t irq)
{
  struct ioapic_redirection v;
  ioapic_readv(irq, &v);
  v.mask &= 0xFE;
  ioapic_writev(irq, &v);
}

void ioapic_init()
{
  vmm_set_page(0, (uintptr_t)P2V(IOAPIC_BASE), IOAPIC_BASE, PAGE_PRESENT | PAGE_WRITE);
  for(int i = 0; i < 24; i++)
    ioapic_route(i, IOAPIC_DEST_LOGICAL | IOAPIC_DELIVERY_LOWEST, 0xFF, INT_IRQ0+i);
}
