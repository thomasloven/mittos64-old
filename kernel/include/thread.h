#pragma once
typedef struct thread_st thread_t;
#include <stdint.h>
#include <list.h>
#include <process.h>
#include <int.h>
#include <cpu.h>

#define THREAD_STACK_SIZE 0x1000-sizeof(thread_t)

typedef struct thread_st
{
  registers_t r;
  uint64_t stack_pointer; // Top of the kernel stack for thread
  uint64_t tid;
  uint64_t state;
  uint8_t sse_registers[512];
  process_t *process;
  LIST(struct thread_st, process_threads);
  LIST(struct thread_st, ready_queue);
} thread_t;

#define THREAD_STATE_WAITING  0x1
#define THREAD_STATE_READY    0x2
#define THREAD_STATE_RUNNING  0x3
#define THREAD_STATE_FINISHED 0x4

typedef struct thread_stack_st
{
  uint8_t stack[THREAD_STACK_SIZE-8*8];
  // Stack layout of a new thread
  uint64_t RBP;
  uint64_t RBX;
  uint64_t R12;
  uint64_t R13;
  uint64_t R14;
  uint64_t R15;
  uint64_t thread;
  uint64_t zero_frame;
  uint64_t function_address;
  thread_t tcb;
} thread_stack_t;

#define get_current_thread() (get_cpu()->current_thread)
#define set_current_thread(new) (get_cpu()->current_thread = (new))

thread_t *new_thread(void (*func)(void));
void switch_thread(thread_t *old, thread_t *new);
void free_thread(thread_t *th);

void swtch(uint64_t *old, uint64_t *new);
