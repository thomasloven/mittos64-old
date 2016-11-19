#pragma once
typedef struct process_st process_t;
#include <stdint.h>
#include <thread.h>
#include <mem.h>
#include <cpu.h>

typedef struct process_st
{
  uint64_t pid;
  uint64_t state;
  uint64_t status;
  struct process_st *parent;
  page_table *P4;
  LIST(struct process_st, children);
  LIST(struct process_st, siblings);
  LIST(thread_t, threads);
} process_t;

#define PROC_STATE_READY 1
#define PROC_STATE_RUNNING 2
#define PROC_STATE_ZOMBIE 3
#define PROC_STATE_DONE 4

#define process_alive(proc) ((proc)->state == PROC_STATE_READY || (proc)->state == PROC_STATE_RUNNING)

#define get_current_process() (get_cpu()->current_process)
#define set_current_process(proc) (get_cpu()->current_process = (proc))

process_t *process_spawn(process_t *parent);
void process_attach(process_t *proc, thread_t *th);
void switch_process(process_t *proc);
void process_exit(process_t *proc, uint64_t status);
void process_free(process_t *proc);
