#include <stdint.h>
#include <int.h>
#include <mem.h>
#include <debug.h>
#include <cpu.h>
#include <string.h>
#include <apic.h>
#include <pit.h>

extern uint64_t smp_stack_top;
extern uint64_t ap_gs_base;

void ap_init(cpu_t *cpu)
{
  debug_info("SMP - Starting up processor %x\n", cpu->id);
  if(!cpu->current_state == CPU_STOPPED)
    return;
  // Send INIT command
  apic_interrupt(cpu->id, APIC_INT_LEVEL_ASSERT, APIC_INT_TYPE_INIT, 0);
  // Wait 10 ms
  pit_delay(10);

  // Write cpu data position to trampoline
  ap_gs_base = (uint64_t) cpu;

  cpu->current_state = CPU_SIPI_SENT;
  apic_interrupt(cpu->id, APIC_INT_LEVEL_ASSERT, APIC_INT_TYPE_STARTUP, TRAMPOLINE_ADDR/PAGE_SIZE);
  pit_delay(10);

  if(cpu->current_state != CPU_SIPI_REC)
  {
    cpu->current_state = CPU_SIPI_SENT;
    apic_interrupt(cpu->id, APIC_INT_LEVEL_ASSERT, APIC_INT_TYPE_STARTUP, TRAMPOLINE_ADDR/PAGE_SIZE);
    pit_delay(100);
  }

  if(cpu->current_state != CPU_SIPI_REC)
  {
    debug_error("SMP - Starting cpu %x failed\n", cpu->id);
    cpu->current_state = CPU_FAILED;
    return;
  }
  cpu->current_state = CPU_STARTED;
}

void ap_start()
{
  debug_ok("STARTED CPU:%x\n", get_cpu()->id);
  for(;;)asm("hlt");
}
