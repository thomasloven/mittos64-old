#include <cpu.h>
#include <debug.h>
#include <apic.h>
#include <msr.h>
#include <sse.h>
#include <scheduler.h>
#include <mem.h>
#include <gdt.h>
#include <string.h>

cpu_t cpus[MAX_NUMCPU];
unsigned int num_cpu = 0;
unsigned int all_ap_started = 0;

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

  cpu->current_thread = 0;
  cpu->current_state = (num_cpu)?CPU_STOPPED:CPU_STARTED;
  cpu->kernel_stack = (num_cpu)?kmalloc(PAGE_SIZE) + PAGE_SIZE:0;

  num_cpu++;
}

void init_cpu()
{
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

  uintptr_t page = pmm_alloc();
  vmm_set_page(0, TRAMPOLINE_ADDR, TRAMPOLINE_ADDR, PAGE_PRESENT | PAGE_WRITE);
  vmm_set_page(0, TRAMPOLINE_GDT, page, PAGE_PRESENT | PAGE_WRITE);
  memcpy((void *)TRAMPOLINE_ADDR, &trampoline, PAGE_SIZE);
  memcpy((void *)TRAMPOLINE_GDT, &trampoline_GDT, PAGE_SIZE);

  for(unsigned int i = 0; i < num_cpu; i++)
    ap_init(&cpus[i]);

  all_ap_started = 1;

  vmm_set_page(0, TRAMPOLINE_ADDR, 0, 0);
  vmm_set_page(0, TRAMPOLINE_GDT, 0, 0);
  pmm_free(page);

  debug_info("CPU - Status\n");
  for(unsigned int i = 0; i < num_cpu; i++)
  {
    debug("  CPU id:%x lapic:%x\n", cpus[i].id, cpus[i].apic_id);
  }

}
