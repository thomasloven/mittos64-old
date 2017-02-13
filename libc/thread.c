#include "syscalls.h"

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
