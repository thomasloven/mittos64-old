#include <cpu.h>
#include <debug.h>
#include <apic.h>
#include <msr.h>
#include <sse.h>
#include <scheduler.h>

cpu_t cpus[MAX_NUMCPU];
unsigned int num_cpu = 0;

void cpu_add(uint64_t id, uint64_t apic)
{
  if(num_cpu >= MAX_NUMCPU)
    return;

  cpu_t *cpu = &cpus[num_cpu];
  memset(cpu, 0, sizeof(cpu_t));
  cpu->cpu = (uint64_t)cpu;
  cpu->id = id;
  cpu->apic_id = apic;
  cpu->is_bsp = (num_cpu)?0:1;
  num_cpu++;
}

void init_cpu()
{
  cpu_t *cpu = get_cpu();
  debug_info("CPU - Initializing CPU %x\n", cpu->id);

  interrupt_init();
  apic_init();
  gdt_init();
  sse_init();
  scheduler_init();
}

void cpu_init()
{
  acpi_init();
  pic_init();
  ioapic_init();

  msr_write(MSR_REG_KERNEL_GS, cpus[0].cpu);
  asm("swapgs");
  init_cpu();

  debug_info("CPU - Status\n");
  for(unsigned int i = 0; i < num_cpu; i++)
  {
    debug("  CPU id:%x lapic:%x\n", cpus[i].id, cpus[i].apic_id);
  }
}
