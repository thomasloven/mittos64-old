#include <thread.h>
#include <stdint.h>
#include <mem.h>
#include <list.h>
#include <debug.h>

thread_t *current_thread = 0;

uint64_t tid = 1;


thread_t *new_thread(void (*func)(void))
{
  // Set up original stack of thread
  thread_stack_t *stack = kcalloc(1, sizeof(thread_stack_t));
  thread_t *th = &stack->tcb;

  stack->function_address = (uint64_t)func;
  stack->RBP = (uint64_t)&stack->zero_frame;

  th->tid = tid++;
  th->state = THREAD_STATE_READY;
  th->stack_pointer = (uint64_t)&stack->RBP;

  LIST_INIT(th, ready_queue);

  return th;
}

void switch_thread(thread_t *old, thread_t *new)
{
  set_current_thread(new);

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
  thread_stack_t *stack = incptr(th, -offsetof(thread_stack_t, tcb));
  kfree(stack);
}
