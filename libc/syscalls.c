#include <stdarg.h>
#include <stdio.h>
#include "syscalls.h"

long kernel_syscall(int num, ...)
{
  va_list varg;
  va_start(varg, num);
  long a1 = va_arg(varg, long);
  long a2 = va_arg(varg, long);
  long a3 = va_arg(varg, long);
  register long r10 __asm__("r10") = va_arg(varg, long);
  register long r8 __asm__("r8") = va_arg(varg, long);
  register long r9 __asm__("r9") = va_arg(varg, long);
  va_end(varg);

  long ret;
  __asm__ __volatile__("syscall" : "=a" (ret) : "a" (num), "D" (a1), "S" (a2), "d" (a3), "r" (r10), "r" (r8), "r" (r9) : "rcx", "r11", "memory");
  return ret;
}

char debug_buffer[256];
void kernel_debug(char *fmt, ...)
{
  va_list varg;
  va_start(varg, fmt);
  vsprintf(debug_buffer, fmt, varg);
  va_end(varg);
  kernel_syscall(SYS_DEBUG, (long)debug_buffer);
}

long __syscall_common(long num, ...)
{
  va_list varg;
  va_start(varg, num);
  long a1 = va_arg(varg, long);
  long a2 = va_arg(varg, long);
  long a3 = va_arg(varg, long);
  long a4 = va_arg(varg, long);
  long a5 = va_arg(varg, long);
  long a6 = va_arg(varg, long);
  va_end(varg);

  kernel_debug("==> SYSCALL %ld (%lx, %lx, %lx, %lx, %lx, %lx)\n", num, a1, a2, a3, a4, a5, a6);

  while(1);
  return 0;
}

struct pthread *__pthread_self()
{
  return 0;
}
