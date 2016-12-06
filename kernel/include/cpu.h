#pragma once

#define MAX_NUMCPU 64

#define CPU_STOPPED 0
#define CPU_SIPI_SENT 1
#define CPU_SIPI_REC 2
#define CPU_STARTED 3
#define CPU_FAILED 4

#define TRAMPOLINE_ADDR 0x1000
#define TRAMPOLINE_GDT 0x2000

#define GS_OFFSET_CPU 0
#define GS_OFFSET_STATE 40
#define GS_OFFSET_STACK 48
#define GS_OFFSET_SCTEMP 56

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <gdt.h>
#include <process.h>
#include <thread.h>
#include <scheduler.h>

typedef struct cpu_t
{
  uint64_t cpu;
  uint64_t id;
  uint64_t apic_id;
  uint64_t apic_ticks_per_us;
  uint64_t is_bsp;
  uint64_t current_state; // 40
  void *kernel_stack; // 48
  uint64_t syscall_temp; // 56
  thread_t *current_thread;
  thread_t *last_thread;
  process_t *current_process;
  uint64_t gdt[7];
  struct gdtp_st gdt_p;
  thread_t *scheduler;
  tss_t tss;
}__attribute__((packed)) cpu_t;

extern cpu_t cpus[];
extern unsigned int num_cpu;
extern unsigned int all_ap_started;

void acpi_init();
void cpu_add(uint64_t id, uint64_t apic);
void cpu_init();
void init_cpu();
cpu_t *get_cpu();
void ap_init(cpu_t *cpu);

void trampoline();
void trampoline_GDT();

#endif


