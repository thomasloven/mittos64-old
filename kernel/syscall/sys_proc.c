#include <syscall.h>
#include <process.h>
#include <thread.h>
#include <scheduler.h>
#include <string.h>
#include <mem.h>
#include <debug.h>
#include <list.h>
#include <vfs.h>

SYSCALL_DEF(fork)
{
  SYSCALL_INIT();

  // Copy process and memory space
  process_t *p = get_current_process();
  process_t *new = process_spawn(p);

  // Copy thread
  thread_t *th = new_thread(0, 1);
  memcpy(&th->r, &get_current_thread()->r, sizeof(registers_t));

  // Make new thread return 0
  th->r.rax = 0;
  process_attach(new, th);
  scheduler_insert(th);

  // Return new pid
  return new->pid;
}

__attribute__((noreturn)) SYSCALL_DEF(exit)
{
  SYSCALL_INIT(int, status);

  process_t *proc = get_current_process();

  process_exit(proc, status);

  schedule();
  debug_error("PANIC - This line should be unreachable (%s:%d)\n", __FILE__, __LINE__);
  for(;;);
}

SYSCALL_DEF(wait)
{
  SYSCALL_INIT(int *, status);

  process_t *proc = get_current_process();
  int pid = 0;
  while(!pid)
  {
    LIST_FOREACH(proc->children, process_t, p, siblings)
    {
      if(p->state == PROC_STATE_ZOMBIE)
      {
        pid = p->pid;
        *status = p->status;
        // Destroy the process
        process_free(p);
        break;
      }
    }
    schedule();
  }
  return pid;
}
