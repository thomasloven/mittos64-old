#pragma once

#define MAX_NUMCPU 64

#define GS_OFFSET_CPU 0

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <gdt.h>
#include <process.h>
#include <thread.h>

typedef struct cpu_t
{
  uint64_t cpu;
  uint64_t id;
  uint64_t apic_id;
  uint64_t apic_ticks_per_us;
  uint64_t is_bsp;
  thread_t *current_thread;
  thread_t *last_thread;
  process_t *current_process;
  uint64_t gdt[5];
  struct gdtp_st gdt_p;
  thread_t *scheduler;
}__attribute__((packed)) cpu_t;

extern cpu_t cpus[];
extern unsigned int num_cpu;

void acpi_init();
void cpu_add(uint64_t id, uint64_t apic);
void cpu_init();
cpu_t *get_cpu();
#endif


