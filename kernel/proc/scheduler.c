#include <scheduler.h>
#include <list.h>
#include <debug.h>

LIST(thread_t, ready_queue);

thread_t *scheduler_th;
thread_t *last_thread = 0;
#define get_last_thread() (last_thread)
#define set_last_thread(new) (last_thread = (new))

void scheduler_insert(thread_t *th)
{
  // Append thread to the ready queue and prepare it for running
  LIST_APPEND(ready_queue, th, ready_queue);
  th->state = THREAD_STATE_READY;
}

void scheduler_remove(thread_t *th)
{
  // Remove thread from the ready queue
  LIST_REMOVE(ready_queue, th, ready_queue);
}

thread_t *scheduler_next()
{
  // Get the next thread from the ready queue
  if(!LIST_EMPTY(ready_queue))
  {
    thread_t *th = LIST_FIRST(ready_queue);
    scheduler_remove(th);
    return th;
  }
  return 0;
}

void scheduler()
{
  while(1)
  {
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

    while(!(new = scheduler_next()));

    if(!process_alive(new->process))
    {
      new->state = THREAD_STATE_FINISHED;
      continue;
    }
    new->state = THREAD_STATE_RUNNING;
    set_last_thread(new);
    switch_process(new->process);
    switch_thread(scheduler_th, new);
  }
}

void schedule()
{
  // This function handles swithing to the next thread in the ready queue

  thread_t *old = get_current_thread();

  switch_process(0);
  switch_thread(old, scheduler_th);
}

void scheduler_init()
{
  LIST_HEAD_INIT(ready_queue);
  scheduler_th = new_thread(scheduler);
}
