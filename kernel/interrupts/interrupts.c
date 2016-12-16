#include <int.h>
#include <debug.h>
#include <registers.h>
#include <string.h>
#include <stdint.h>
#include <thread.h>

struct int_gate_descriptor idt[NUM_INTERRUPTS];
struct idtr idtr;
int_handler_t int_handlers[NUM_INTERRUPTS];

void idt_set(uint32_t num, void *vector, uint16_t cs, uint8_t ist, uint8_t flags)
{
  uintptr_t v = (uintptr_t)vector;
  idt[num].base_l = v & 0xFFFF;
  idt[num].base_m = (v >> 16) & 0xFFFF;
  idt[num].base_h = (v >> 32) & 0xFFFFFFFF;
  idt[num].cs = cs;
  idt[num].ist = ist;
  idt[num].flags = flags;
}

void interrupt_init()
{
  // Clear IDT and interrupt handler list
  memset(idt, 0, sizeof(idt));
  memset(int_handlers, 0, sizeof(int_handlers));

  // Register all vectors in the IDT
  extern void *isr_table[];
  for(int i=0; i < NUM_INTERRUPTS; i++)
  {
    idt_set(i, isr_table[i], 0x8, 0,
        (IDT_PRESENT | IDT_DPL0 | IDT_INTERRUPT));
  }

  // Setup pointer and load IDT
  idtr.addr = (uint64_t)idt;
  idtr.len = sizeof(idt)-1;
  load_idt(&idtr);
}

int_handler_t register_int_handler(uint32_t num, int_handler_t h)
{
  // Replace the handler for an interrupt
  // Return the old handler
  int_handler_t old = int_handlers[num];
  int_handlers[num] = h;
  return old;
}


registers_t *int_handler(registers_t *r)
{
  // If a handler is registered, pass everything onto it
  if(int_handlers[r->int_no])
    return int_handlers[r->int_no](r);

  debug("\n===============================================================================\n");
  debug_error("Unhandled interrupt!\n");
  print_registers(r);

#ifndef NDEBUG
  thread_t *th = get_current_thread();
  (void)th;
  process_t *proc = get_current_process();
  (void)proc;
  asm("int_handler_breakpoint:");
#endif

  for(;;);
}
