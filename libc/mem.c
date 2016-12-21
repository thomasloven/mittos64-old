#include "syscalls.h"

long kernel_syscall(int num, ...);

SYSCALL_DEF(brk)
{
  SYSCALL_INIT(unsigned long, brk);

  return kernel_syscall(SYS_BRK, brk);
}
