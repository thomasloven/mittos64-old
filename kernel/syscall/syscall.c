#include <syscall.h>
#include <debug.h>
#include <int.h>
#include <msr.h>
#include <gdt.h>
#include <registers.h>
#include "../../libc/syscall_num.h"

extern void syscall_entry();

syscall_handler_t syscall_handlers[1024];

SYSCALL_DECL(debug);

registers_t *syscall_handler(registers_t *r)
{
  // Syscall number: RAX
  // Order of arguments:
  // RDI
  // RSI
  // RDX
  // R10
  // R8
  // R9
  // stack
  //
  // Return value:
  // RAX

  if(syscall_handlers[r->rax])
  {
    r->rax = syscall_handlers[r->rax](r->rax, r->rdi, r->rsi, r->rdx, r->r10, r->r8, r->r9);
    return r;
  }

  debug_error("Unknown syscall, No:%d\n", r->rax);
  debug("syscall_%d(%x, %x, %x, %x, %x, %x)\n", r->rax, r->rdi, r->rsi, r->rdx, r->r10, r->r8, r->r9);
  for(;;);
}

int syscall_installed = 0;

void syscall_init()
{
  msr_write(MSR_REG_STAR, (((uint64_t)SEG_KDATA | 0x3)<<48 | ((uint64_t)SEG_KCODE)<<32));
  msr_write(MSR_REG_LSTAR, (uint64_t)syscall_entry);
  msr_write(MSR_REG_EFER, msr_read(MSR_REG_EFER)| 1);
  msr_write(MSR_REG_FMASK, RFLAGS_IF);

  if(!syscall_installed)
  {
    register_int_handler(INT_SYSCALL, syscall_handler);
    syscall_installed = 1;
    memset(syscall_handlers, 0, 1024*sizeof(syscall_handler_t));

    SYSCALL_REGISTER(debug, SYS_DEBUG);
  }

}

SYSCALL_DEF(debug)
{
  SYSCALL_INIT(char*, message);
  debug_puts(message);
  return 0;
}