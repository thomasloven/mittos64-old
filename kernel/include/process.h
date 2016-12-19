#pragma once
typedef struct process_st process_t;
#include <stdint.h>
#include <thread.h>
#include <mem.h>
#include <cpu.h>
#include <sync.h>
#include <mem.h>

struct procmm_mmap_st;
typedef struct process_st
{
  uint64_t pid;
  uint64_t state;
  uint64_t status;
  lock_t lock;
  struct process_st *parent;
  struct procmm_mmap_st *mmap;
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

typedef struct procmm_area_st
{
  uintptr_t start;
  uintptr_t end;
  uint64_t flags;
  struct procmm_mmap_st *map;
  LIST(struct procmm_area_st, areas);
} procmm_area_t;

typedef struct procmm_mmap_st
{
  process_t *proc;
  page_table *P4;
  LIST(struct procmm_area_st, areas);
  procmm_area_t *brk;
  procmm_area_t *stack;
} procmm_mmap_t;


process_t *process_spawn(process_t *parent);
void process_attach(process_t *proc, thread_t *th);
void switch_process(process_t *proc);
void process_exit(process_t *proc, uint64_t status);
void process_free(process_t *proc);

procmm_mmap_t *procmm_new_map(process_t *proc, procmm_mmap_t *src);
void procmm_free_map(process_t *proc);
void procmm_print_map(procmm_mmap_t *map);
procmm_area_t *procmm_map(procmm_mmap_t *map, uintptr_t start, uintptr_t end, uint64_t flags);
void procmm_unmap(procmm_area_t *a);
uintptr_t procmm_setup(process_t *proc);
