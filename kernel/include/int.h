#pragma once

#define   IDT_CALL        0x0C
#define   IDT_INTERRUPT   0x0E
#define   IDT_TRAP        0x0F
#define   IDT_DPL0        0x00
#define   IDT_DPL3        0x60
#define   IDT_PRESENT     0x80

#define   NUM_INTERRUPTS   256

#define   INT_DE    0x00 // Divide by zero exception
#define   INT_DB    0x01 // Debug exception
#define   INT_NMI   0x02 // Non-maskable Interrupt exception
#define   INT_BP    0x03 // Beakpoint exception
#define   INT_OF    0x04 // Overflow exception
#define   INT_BR    0x05 // Bound Range exception
#define   INT_UD    0x06 // Invalid Opcode exception
#define   INT_NM    0x07 // Device not Available exception
#define   INT_DF    0x08 // Double Fault exception
#define   INT_CSO   0x09 // Coprocessor Segment Overrun exception
#define   INT_TS    0x0A // Invalid TSS exception
#define   INT_NP    0x0B // Segment not Present excepton
#define   INT_SS    0x0C // Stack exception
#define   INT_GP    0x0D // General Protection Fault exception
#define   INT_PF    0x0E // Page Fault exception
// Unused exception 0x0F
#define   INT_MF    0x10 // Floating Point exception pending
#define   INT_AC    0x11 // Alignment Check exception
#define   INT_MC    0x12 // Machine Check exception
#define   INT_XF    0x13 // SIMD Floating Point exception
// Unused exception 0x14 - 0x1D
#define   INT_SX    0x1E // Security exception
// Unused exception 0x1F
// User interrupts  0x20 and forward
#define   INT_IRQ0      0x20
#define   INT_IRQ1      0x21
#define   INT_IRQ2      0x22
#define   INT_IRQ3      0x23
#define   INT_IRQ4      0x24
#define   INT_IRQ5      0x25
#define   INT_IRQ6      0x26
#define   INT_IRQ7      0x27
#define   INT_IRQ8      0x28
#define   INT_IRQ9      0x29
#define   INT_IRQ10     0x2A
#define   INT_IRQ11     0x2B
#define   INT_IRQ12     0x2C
#define   INT_IRQ13     0x2D
#define   INT_IRQ14     0x2E
#define   INT_IRQ15     0x2F
#define   INT_IRQ16     0x30
#define   INT_IRQ17     0x31
#define   INT_IRQ18     0x32
#define   INT_IRQ19     0x33
#define   INT_IRQ20     0x34
#define   INT_IRQ21     0x35
#define   INT_IRQ22     0x36
#define   INT_IRQ23     0x37

#define   INT_APIC_TIMER     0x40
#define   INT_APIC_THERMAL   0x41
#define   INT_APIC_PERF      0x42
#define   INT_APIC_LINT0     0x43
#define   INT_APIC_LINT1     0x44
#define   INT_APIC_ERROR     0x45
#define   INT_APIC_SPUR      0xFF

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <apic.h>

struct int_gate_descriptor
{
  uint16_t base_l;
  uint16_t cs;
  uint8_t ist;
  uint8_t flags;
  uint16_t base_m;
  uint32_t base_h;
  uint32_t ignored;
} __attribute__ ((packed));

struct idtr
{
  uint16_t len;
  uint64_t addr;
} __attribute__ ((packed));

void pic_init();
extern unsigned int irq_map[24];
#define IRQ_INT(irq) (INT_IRQ0 + IRQ(irq))
#define IRQ(irq) (irq_map[irq])
#define IRQ_MASK(irq) ioapic_mask(IRQ(irq))
#define IRQ_UNMASK(irq) ioapic_unmask(IRQ(irq))
#define IRQ_ACK(irq) apic_ack(IRQ(irq))

#define IRQ_TIMER 0
#define IRQ_KEYBOARD 1

typedef struct registers_st
{
  uint64_t rax;
  uint64_t rbx;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t rbp;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;

  uint64_t int_no;
  uint64_t err_code;
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
}registers_t;

typedef registers_t *(*int_handler_t)(registers_t *);

void interrupt_init();
int_handler_t register_int_handler(uint32_t num, int_handler_t h);
void isr_return(registers_t *r);

#endif
