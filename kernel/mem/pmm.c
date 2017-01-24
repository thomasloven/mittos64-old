#include <mem.h>
#include <multiboot.h>
#include <debug.h>


int pmm_running = 0;
void *pmm_pos = &kernel_end;
void **pmm_stack = 0;

uintptr_t pmm_alloc()
{
  void **p;
  if(!pmm_running)
  {
    p = (void **)pmm_pos;
    pmm_pos = incptr(pmm_pos, PAGE_SIZE);
  } else {
    p = pmm_stack;
    pmm_stack = *p;
  }
  return (uintptr_t)V2P(p);
}

void pmm_free(uintptr_t page)
{
  void **p  = (void **)P2V(page & ~PAGE_FLAGS_MASK);
  *p = pmm_stack;
  pmm_stack = p;
  pmm_running = 1;
}


void pmm_init()
{
  uintptr_t start, end;
  uint32_t type;

  while(multiboot_get_memory_area(&start, &end, &type))
  {
    debug_info("PMM - %x->%x%s\n", start, end, (type==MBOOT_MMAP_FREE)?" (free)":" (reserved)");
    for(uintptr_t p = start; (p+PAGE_SIZE) < end; p += PAGE_SIZE)
    {
      vmm_set_page(0, (uintptr_t)P2V(p), p, PAGE_PRESENT | PAGE_WRITE);
      if((p >= (uintptr_t)V2P(&kernel_start)) && (p < (uintptr_t)V2P(pmm_pos)))
        continue;
      if(multiboot_page_used(p))
        continue;
      if(type == MBOOT_MMAP_FREE)
        pmm_free(p);
    }
  }

}
