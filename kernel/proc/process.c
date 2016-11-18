#include <process.h>
#include <thread.h>
#include <debug.h>
#include <scheduler.h>
#include <sync.h>

uint64_t pid = 1;
process_t *init_proc = 0;

process_t *process_spawn(process_t *parent)
{
  process_t *proc = kcalloc(1, sizeof(process_t));
  proc->pid = pid++;
  proc->parent =  parent;
  proc->P4 = vmm_new_P4();
  proc->state = PROC_STATE_READY;

  LIST_INIT(proc, children);
  LIST_INIT(proc, siblings);
  LIST_INIT(proc, threads);

  if(parent)
  {
    spin_lock(&parent->lock);
    LIST_APPEND(parent->children, proc, siblings);
    spin_unlock(&parent->lock);
  }

  if(proc->pid == 1)
    init_proc = proc;

  return proc;
}

void process_attach(process_t *proc, thread_t *th)
{
  spin_lock(&proc->lock);
  LIST_APPEND(proc->threads, th, process_threads);
  th->process = proc;
  spin_unlock(&proc->lock);
}

void switch_process(process_t *proc)
{
  process_t *old = get_current_process();
  if(old)
  {
    if(old == proc)
      return;
    if(old->state == PROC_STATE_RUNNING)
      old->state = PROC_STATE_READY;
  }
  set_current_process(proc);

  if(proc)
  {
    proc->state = PROC_STATE_RUNNING;
    vmm_set_P4(proc->P4);
  }
  else
  {
    vmm_set_P4(0);
  }
}

void process_exit(process_t *proc, uint64_t status)
{
  spin_lock(&proc->lock);
  proc->status = status;
  LIST_FOREACH(proc->children, process_t, c, siblings)
  {
    // Make init process adopt orphaned children
    LIST_REMOVE(proc->children, c, siblings);
    c->parent = init_proc;
    LIST_APPEND(init_proc->children, c, siblings);
  }
  proc->state = PROC_STATE_ZOMBIE;
  spin_unlock(&proc->lock);
}

void process_free(process_t *proc)
{
  if(proc->state != PROC_STATE_ZOMBIE)
  {
    debug_error("Trying to free an unfinished process!\n");
    for(;;);
  }
  LIST_FOREACH(proc->threads, thread_t, th, process_threads)
  {
    while(th->state == THREAD_STATE_RUNNING || th->state == THREAD_STATE_READY) schedule();
    free_thread(th);
  }
  LIST_REMOVE(proc->parent->children, proc, siblings);
  // Free page directory
  vmm_free_P4(proc->P4);
  // Free process structure
  kfree(proc);
}
