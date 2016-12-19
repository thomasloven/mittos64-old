#include <mem.h>
#include <string.h>
#include <debug.h>
#include <int.h>
#include <registers.h>
#include <thread.h>
#include <scheduler.h>


#define STRIP_FLAGS(addr) ((void *)(((uintptr_t)(addr)) & ~PAGE_FLAGS_MASK))
#define ADD_FLAGS(addr, flags) ((void *)(((uintptr_t)(addr)) | ((flags) & PAGE_FLAGS_MASK)))


extern page_table BootP4;

page_table *vmm_new_P4()
{
  page_table *P4 = (page_table *)pmm_alloc();
  memset(P2V(P4), 0, PAGE_SIZE);
  ((page_table *)P2V(P4))[511] = ((page_table *)&BootP4)[511];
  return P4;
}

void vmm_set_P4(page_table *P4)
{
  if(P4)
    write_cr3((uint64_t)P4);
  else
    write_cr3(V2P(&BootP4));
}

void vmm_free_P4(page_table *P4)
{
  P4 = P2V(P4);
  for(int i4=0; i4 < (int)P4_OFFSET(KERNEL_OFFSET); i4++)
  {
    if(P4[i4])
    {
      page_table *P3 = P2V(STRIP_FLAGS(P4[i4]));
      for(int i3=0; i3 < 0x200; i3++)
      {
        if(P3[i3])
        {
          page_table *P2 = P2V(STRIP_FLAGS(P3[i3]));
          for(int i2=0; i2 < 0x200; i2++)
          {
            if(P2[i2])
            {
              page_table *P1 = P2V(STRIP_FLAGS(P2[i2]));
              for(int i1=0; i1 < 0x200; i1++)
              {
                if(P1[i1])
                {
                  pmm_free((uintptr_t)STRIP_FLAGS(P1[i1]));
                  P1[i1] = 0;
                }
              }
              pmm_free((uintptr_t)STRIP_FLAGS(P2[i2]));
              P2[i2] = 0;
            }
          }
          pmm_free((uintptr_t)STRIP_FLAGS(P3[i3]));
          P3[i3] = 0;
        }
      }
      pmm_free((uintptr_t)STRIP_FLAGS(P4[i4]));
      P4[i4] = 0;
    }
  }
  pmm_free(V2P(P4));
}

void *vmm_find_page(page_table *P4, uintptr_t addr, uint32_t touch)
{
  addr &= ~PAGE_FLAGS_MASK;

  page_table *P3, *P2, *P1;
  P4 = P2V(P4);
  if(!(P3 = P4[P4_OFFSET(addr)]))
  {
    if(!touch) return 0;
    uintptr_t p = pmm_alloc();
    memset(P2V(p), 0, PAGE_SIZE);
    P4[P4_OFFSET(addr)] = P3 = ADD_FLAGS(p, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
  }
  P3 = P2V(STRIP_FLAGS(P3));

  if(!(P2 = P3[P3_OFFSET(addr)]))
  {
    if(!touch) return 0;
    uintptr_t p = pmm_alloc();
    memset(P2V(p), 0, PAGE_SIZE);
    P3[P3_OFFSET(addr)] = P2 = ADD_FLAGS(p, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
  }
  P2 = P2V(STRIP_FLAGS(P2));

  if(!(P1 = P2[P2_OFFSET(addr)]))
  {
    if(!touch) return 0;
    uintptr_t p = pmm_alloc();
    memset(P2V(p), 0, PAGE_SIZE);
    P2[P2_OFFSET(addr)] = P1 = ADD_FLAGS(p, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
  }
  return P2V(STRIP_FLAGS(P1));
}

uintptr_t vmm_get_page(page_table *P4, uintptr_t addr)
{
  page_table *P1;
  if(!P4) P4=GET_P4();
  if(!(P1 = vmm_find_page(P4, addr, 0))) return 0;
  return (uintptr_t)(P1[P1_OFFSET(addr)]);
}

uintptr_t vmm_set_page(page_table *P4, uintptr_t addr, uintptr_t phys, uint32_t flags)
{
  page_table *P1;
  if(!P4) P4=GET_P4();
  if(!(P1 = vmm_find_page(P4, addr, 1))) return 0;
  P1[P1_OFFSET(addr)] = ADD_FLAGS(phys, flags);
  return (uintptr_t)P1[P1_OFFSET(addr)];
}


size_t vmm_p4_memcpy(page_table *dst_P4, void *dst, page_table *src_P4, void *src, size_t n)
{
  size_t copied = 0;
  while(n)
  {
    size_t d_offset = (uintptr_t)dst % PAGE_SIZE;
    size_t s_offset = (uintptr_t)src % PAGE_SIZE;
    size_t d_bytes = PAGE_SIZE - d_offset;
    size_t s_bytes = PAGE_SIZE - s_offset;
    size_t bytes = (d_bytes < s_bytes)?d_bytes:s_bytes;
    bytes = (bytes < n)?bytes:n;
    void *d_page = STRIP_FLAGS(vmm_get_page(dst_P4, (uintptr_t)dst));
    void *s_page = STRIP_FLAGS(vmm_get_page(src_P4, (uintptr_t)src));
    if (!d_page || !s_page)
      return 0;
    memcpy(incptr(P2V(d_page), d_offset), incptr(P2V(s_page), s_offset), bytes);

    copied += bytes;
    n -= bytes;
    dst = incptr(dst, bytes);
    src = incptr(src, bytes);
  }
  return copied;
}

size_t vmm_p4_memset(page_table *P4, void *s, int c, size_t n)
{
  size_t set = 0;
  while(n)
  {
    size_t offset = (uintptr_t)s % PAGE_SIZE;
    size_t bytes = PAGE_SIZE - offset;
    bytes = (bytes < n)?bytes:n;
    void *page = STRIP_FLAGS(vmm_get_page(P4, (uintptr_t)s));
    if(!page)
      return 0;
    memset(incptr(P2V(page), offset), c, bytes);

    set += bytes;
    n -= bytes;
    s = incptr(s, bytes);
  }
  return set;
}


registers_t *page_fault_handler(registers_t *r)
{
  if(get_current_process())
  {
    registers_t *ret = procmm_page_fault(r);
    if(ret)
      return ret;
  }

  debug("\n ===== PAGE FAULT =====\n");
  debug("Instruction at:%x tried to", r->rip);
  if(r->err_code & 0x2)
    debug(" read from");
  else if(r->err_code & 0xE)
    debug(" execute code from");
  else
    debug(" write to");
  debug(" memory \nat:%x in",  read_cr2());
  if(r->err_code & 0x8)
    debug(" user mode");
  else
    debug(" kernel mode");
  debug("\n");
  debug("This caused a");
  if(r->err_code & 0x1)
    debug(" protection fault");
  else
    debug(" page miss");
  debug(".\n ======================\n");
  print_registers(r);

#ifndef NDEBUG
  thread_t *th = get_current_thread();
  (void)th;
  process_t *proc = get_current_process();
  (void)proc;
  asm("page_fault_breakpoint:");
#endif
  for(;;);
}

void vmm_init()
{
  register_int_handler(0xE, page_fault_handler);
}
