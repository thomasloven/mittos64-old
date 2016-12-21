#include <syscall.h>
#include <scheduler.h>
#include <debug.h>


SYSCALL_DEF(brk)
{
  SYSCALL_INIT(uint64_t, addr);
  procmm_area_t *brk = get_current_process()->mmap->brk;

  if(addr)
  {
    size_t len = addr - brk->start;
    procmm_resize(get_current_process(), brk, len);
    return brk->end;
  } else {
    return brk->end;
  }
}
