#include <scheduler.h>
#include <list.h>
#include <debug.h>
#include <sse.h>
#include <apic.h>
#include <sync.h>
#include <debug.h>

LIST(thread_t, ready_queue);

thread_t *scheduler_th;
#define get_last_thread() (get_cpu()->last_thread)
#define set_last_thread(new) (get_cpu()->last_thread = (new))

int scheduler_started = 0;
lock_t scheduler_lock = 0;

void scheduler_insert(thread_t *th)
{
  // Append thread to the ready queue and prepare it for running
  spin_lock(&scheduler_lock);
  LIST_APPEND(ready_queue, th, ready_queue);
  th->state = THREAD_STATE_READY;
  spin_unlock(&scheduler_lock);
}

void scheduler_remove(thread_t *th)
{
  // Remove thread from the ready queue
  spin_lock(&scheduler_lock);
  LIST_REMOVE(ready_queue, th, ready_queue);
  spin_unlock(&scheduler_lock);
}

thread_t *scheduler_next()
{
  // Get the next thread from the ready queue
  spin_lock(&scheduler_lock);
  if(!LIST_EMPTY(ready_queue))
  {
    thread_t *th = LIST_FIRST(ready_queue);
    LIST_REMOVE(ready_queue, th, ready_queue);
    spin_unlock(&scheduler_lock);
    return th;
  }
  spin_unlock(&scheduler_lock);
  return 0;
}

void scheduler()
{
  while(1)
  {
    apic_timer_stop();
    thread_t *old = 0, *new = 0;
    if((old = get_last_thread()))
    {
      if(old->state != THREAD_STATE_FINISHED && process_alive(old->process))
      {
        old->state = THREAD_STATE_READY;
        scheduler_insert(old);
      } else {
        old->state = THREAD_STATE_FINISHED;
      }
    }

    while(!(new = scheduler_next()))asm("sti");
    asm("cli");

    if(!process_alive(new->process))
    {
      new->state = THREAD_STATE_FINISHED;
      continue;
    }
    new->state = THREAD_STATE_RUNNING;
    set_last_thread(new);
    switch_process(new->process);
    sse_restore(new->sse_registers);
    apic_timer(1000000);
    switch_thread(get_cpu()->scheduler, new);
  }
}

void schedule()
{
  // This function handles swithing to the next thread in the ready queue

  thread_t *old = get_current_thread();
  if(old == get_cpu()->scheduler)
    return;

  if(old)
  {
    sse_save(old->sse_registers);
  }

  switch_process(0);
  switch_thread(old, get_cpu()->scheduler);
}

void scheduler_init()
{
  if(!scheduler_started)
  {
    LIST_HEAD_INIT(ready_queue);
    scheduler_started = 1;
  }
  get_cpu()->scheduler = new_thread(scheduler);
}
