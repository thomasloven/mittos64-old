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
