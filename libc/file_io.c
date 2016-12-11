#include "syscalls.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/uio.h>

SYSCALL_DEF(ioctl)
{
  SYSCALL_INIT(int, fd, unsigned long, request);

  if(fd == 1 && request == TIOCGWINSZ)
  {
    return 0;
  }
  kernel_debug("==> IOCTL - unsupported request:%lx\n", request);
  return -1;
}

SYSCALL_DEF(writev)
{
  SYSCALL_INIT(int, fd, const struct iovec *, iov, int, iovcnt);

  size_t len = 0;

  for(int i=0; i < iovcnt; i++)
  {
    len += write(fd, iov[i].iov_base, iov[i].iov_len);
  }

  return (long)len;
}

SYSCALL_DEF(write)
{
  SYSCALL_INIT(int, fd, void *, buffer, size_t, nbyte);

  return kernel_syscall(SYS_WRITE, fd, buffer, nbyte);
}
