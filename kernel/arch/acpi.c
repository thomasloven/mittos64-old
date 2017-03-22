#include <acpi.h>
#include <stdint.h>
#include <debug.h>
#include <mem.h>
#include <string.h>
#include <multiboot.h>
#include <int.h>

RSDP_st *find_rsdp()
{
  // The Root System Description Pointer (RSDP) should be found either
  // - in the first 1 KB of the EBDA
  // - between 0xE0000 and 0xFFFFF
  // We'll only try the second alternative for now

  uintptr_t start = KERNEL_OFFSET + 0xE0000;
  uintptr_t end = KERNEL_OFFSET + 0xFFFFF;

  uint8_t *p = (void *)start;
  while((uintptr_t)p < end)
  {
    // The RSDP is identified by a signature string
    if(!(memcmp(p, "RSD PTR ", 8)))
    {
      // Make sure we got it right by calculating the checksum
      uint8_t checksum = 0;
      for(int i = 0; i < 20; i++)
        checksum += p[i];
      if(!checksum)
        break;
    }
    // The RSDP must be 16-byte aligned
    p = incptr(p, 16);
  }
  if((uintptr_t)p >= end)
    return 0;
  return (void *)p;
}

SDT_header *find_SDT(RSDP_st *rsdp, char *signature)
{
  // Scan through the RSDT or XSDT for a header with the wanted signature

  // Read RSDT if revision <= 1.0
  // else read XSDT
  unsigned int bpp = (!rsdp->revision)?4:8;
  SDT_header *rsdt = (bpp == 4) ? P2V(rsdp->RSDT_p) : P2V(rsdp->XSDT_p);

  int entries = (rsdt->length - sizeof(SDT_header))/bpp;
  for(int i = 0; i < entries; i++)
  {
    // 32 bit pointers if revision <= 1.0
    // otherwise 64 bit pointers
    SDT_header *h = (bpp == 4) ?
      P2V(*(uint32_t*)&rsdt->data[i*bpp]) :
      P2V(*(uint64_t*)&rsdt->data[i*bpp]);

    // Return pointer if signature is correct
    if(!memcmp(h->signature, signature, 4))
      return h;
  }
  return 0;
}

void parse_MADT(SDT_header *header)
{
  MADT_header *madt = (MADT_header *)header->data;
  // Save the memory mapping of the local APIC

  // TODO: In bochs, there are MADT entries with 0 length. I assume parsing should stop here.
  // TODO: Look this up
  for(MADT_field *f = (void*)madt->fields;
      (uintptr_t)f < (uintptr_t)madt + header->length && f->length;
      f = incptr(f, f->length))
  {
    switch(f->type)
    {
      case 0: // Processor Local APIC entry
        debug_info("ACPI - Processor id:%x LAPIC:%x", f->proc.proc_ID, f->proc.APIC_ID);
        if(f->proc.flags & 0x1)
        {
          debug(" enabled");
        }
        debug("\n");
        break;
      case 1: // I/O APIC entry
        debug_info("ACPI - IOAPIC id:%x address:%x\n", f->IOAPIC.APIC_ID, f->IOAPIC.address);
        break;
      case 2: // Interrupt Source Override
        debug_info("ACPI - Interrupt source override %x->%x\n", f->ISO.irq, f->ISO.interrupt);
        irq_map[f->ISO.irq] = f->ISO.interrupt;
        break;
      default:
        break;
    }
  }
}

void acpi_init()
{
  RSDP_st *rsdp;
  if(!(rsdp = mboot_data.rsdp))
  {
    if(!(rsdp = find_rsdp()))
    {
    debug_error("PANIC: ACPI - No RSDP found!");
    for(;;);
    }
  }

  debug_info("ACPI - ACPI version %s\n", (rsdp->revision)?"2.0":"1.0");

  // Scan for MADT
  SDT_header *madt = find_SDT(rsdp, "APIC");
  // Read processor info from MADT
  parse_MADT(madt);
  // Ignore all other tables for now

}
