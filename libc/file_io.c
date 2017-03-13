#include "syscalls.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/uio.h>


SYSCALL_DEF(open)
{
  SYSCALL_INIT(char *, path, int, flags, int, mode);
  return kernel_syscall(SYS_OPEN, path, flags, mode);
}
SYSCALL_DEF(close)
{
  SYSCALL_INIT(int, fd);
  return kernel_syscall(SYS_CLOSE, fd);
}
SYSCALL_DEF(read)
{
  SYSCALL_INIT(int, fd, void *, buffer, size_t, nbyte);

  return kernel_syscall(SYS_READ, fd, buffer, nbyte);
}
SYSCALL_DEF(write)
{
  SYSCALL_INIT(int, fd, void *, buffer, size_t, nbyte);

  return kernel_syscall(SYS_WRITE, fd, buffer, nbyte);
}
SYSCALL_DEF(lseek)
{
  SYSCALL_INIT(int, fd, long, offset, int, whence);
  return kernel_syscall(SYS_SEEK, fd, offset, whence);
}

SYSCALL_DEF(readv)
{
  SYSCALL_INIT(int, fd, const struct iovec *, iov, int, iovcnt);

  size_t len = 0;

  for(int i=0; i < iovcnt; i++)
  {
    len += read(fd, iov[i].iov_base, iov[i].iov_len);
  }

  return (long)len;
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
SYSCALL_DEF(ioctl)
{
  SYSCALL_INIT(int, fd, unsigned long, request);

  if(request == TIOCGWINSZ)
    return !kernel_syscall(SYS_ISATTY, fd);

  kernel_debug("==> IOCTL - unsupported request:%lx\n", request);
  return -1;
}
