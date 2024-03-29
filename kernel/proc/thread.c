#include <thread.h>
#include <stdint.h>
#include <process.h>
#include <mem.h>
#include <list.h>
#include <debug.h>
#include <registers.h>

uint64_t tid = 1;

thread_t *new_thread(void (*func)(void), int user)
{
  // Set up original stack of thread
  thread_stack_t *stack = kcalloc(1, sizeof(thread_stack_t));
  thread_t *th = &stack->tcb;

  stack->thread = (uint64_t)th;
  stack->RBP = (uint64_t)&stack->zero_frame;

  th->r.rflags = RFLAGS_IF;
  if(user)
  {
    stack->function_address = (uint64_t)isr_return;
    th->r.rip = (uint64_t)func;
    th->r.cs = SEG_UCODE|3;
    th->r.ss = SEG_UDATA|3;
    th->r.rflags |= RFLAGS_IOPL3;
    th->r.rsp = USERSPACE_TOP;
  } else {
    stack->function_address = (uint64_t)func;
    th->r.cs = SEG_KCODE;
    th->r.ss = SEG_KDATA;
    th->r.rsp = (uint64_t)&th->stack_pointer;
  }

  th->tid = tid++;
  th->state = THREAD_STATE_READY;
  th->stack_pointer = (uint64_t)&stack->RBP;

  LIST_INIT(th, ready_queue);
  LIST_INIT(th, process_threads);

  return th;
}

void switch_thread(thread_t *old, thread_t *new)
{
  set_current_thread(new);
  get_cpu()->tss.rsp0 = (uint64_t)&new->stack_pointer;
  get_cpu()->kernel_stack = &new->stack_pointer;

  uint64_t *old_stack = (old)?&old->stack_pointer:0;
  swtch(old_stack, &new->stack_pointer);
}

void free_thread(thread_t *th)
{
  if(th->state == THREAD_STATE_RUNNING || th->state == THREAD_STATE_READY)
  {
    debug_error("Trying to free a live thread!\n");
    for(;;);
  }
  if(!LIST_EMPTY(th->process_threads))
    LIST_REMOVE(th->process->threads, th, process_threads);
  thread_stack_t *stack = incptr(th, -offsetof(thread_stack_t, tcb));
  kfree(stack);
}
