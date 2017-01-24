#pragma once
#include <stdint.h>

void apic_init();
void apic_interrupt(uint8_t destination, uint8_t level, uint8_t type, uint8_t vector);
void apic_ack();
void apic_timer(uint64_t us);
void apic_timer_stop();
#define APIC_INT_LEVEL_ASSERT 0x1
#define APIC_INT_LEVEL_DEASSERT 0x0
#define APIC_INT_TYPE_FIXED 0x0
#define APIC_INT_TYPE_INIT 0x5
#define APIC_INT_TYPE_STARTUP 0x6


void ioapic_init();
void ioapic_mask(uint8_t irq);
void ioapic_unmask(uint8_t irq);
