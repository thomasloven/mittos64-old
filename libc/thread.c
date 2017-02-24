#include "syscalls.h"
#include "syscall_num.h"
#include <pthread.h>
#include <signal.h>
long kernel_syscall(int num, ...);

struct pthread *__pt;

struct pthread *__pthread_self()
{
  return __pt;
}

SYSCALL_DEF(set_thread_area)
{
  SYSCALL_INIT(void *, ptr);
  __pt = ptr;
  return 0;
}

SYSCALL_DEF(set_tid_address)
{
  SYSCALL_INIT();
  return 0;
}

SYSCALL_DEF(rt_sigprocmask)
{
  SYSCALL_INIT(int, how, const sigset_t *, set, sigset_t *, oldset);

  return 0;
}

SYSCALL_DEF(fork)
{
  SYSCALL_INIT();
  return kernel_syscall(SYS_FORK);
}

SYSCALL_DEF(gettid)
{
  SYSCALL_INIT();
  return 0;
}

SYSCALL_DEF(exit_group)
{
  SYSCALL_INIT(int, result);
  return kernel_syscall(SYS_EXIT, result);
}

SYSCALL_DEF(wait4)
{
  SYSCALL_INIT(int, pid, int *, result, int, options, void *, rusage);
  return kernel_syscall(SYS_WAIT, result);
}
