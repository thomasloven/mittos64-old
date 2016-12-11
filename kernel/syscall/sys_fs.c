#include <syscall.h>
#include <stdint.h>
#include <stddef.h>
#include <debug.h>
#include <mem.h>
#include <string.h>

SYSCALL_DEF(write)
{
  SYSCALL_INIT(int, fd, void *, buffer, size_t, nbyte);


  if(fd == 1)
  {
    debugn(buffer, nbyte);
    return nbyte;
  }

  return 0;
}
