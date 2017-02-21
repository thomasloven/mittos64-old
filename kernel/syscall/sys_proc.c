#include <syscall.h>
#include <process.h>
#include <thread.h>
#include <scheduler.h>
#include <string.h>

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
