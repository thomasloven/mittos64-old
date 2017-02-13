#include <scheduler.h>
#include <list.h>
#include <debug.h>
#include <int.h>
#include <cpu.h>
#include <registers.h>

procmm_mmap_t *procmm_new_map(process_t *proc, procmm_mmap_t *src)
{
  procmm_mmap_t *map = kcalloc(1, sizeof(procmm_mmap_t));
  map->proc = proc;
  map->P4 = vmm_new_P4();
  LIST_INIT(map, areas);

  if(src)
  {
    LIST_FOREACH(src->areas, procmm_area_t, o, areas)
    {
      procmm_map(map, o->start, o->end, o->flags);
      vmm_p4_memcpy(map->P4, (void *)o->start, src->P4, (void *)o->start, o->end-o->start);
    }
  }

  return map;
}

void procmm_free_map(process_t *proc)
{
  procmm_mmap_t *map = proc->mmap;
  procmm_area_t *o = map->areas.next;
  while(o)
  {
    procmm_area_t *next = o->areas.next;
    procmm_unmap(o);
    o = next;
  }
  map->brk = 0;
  map->stack = 0;
  vmm_free_P4(map->P4);
  kfree(map);
}

void procmm_print_map(procmm_mmap_t *map)
{
  debug_info("Procmm map (process:%d)\n", map->proc->pid);
  LIST_FOREACH(map->areas, procmm_area_t, o, areas)
  {
    debug("  %x->%x (%x)", o->start, o->end, o->flags);
    if(o == map->brk)
      debug(" BRK");
    if(o == map->stack)
      debug(" STACK");
    debug("\n");
  }
}

procmm_area_t *procmm_map(procmm_mmap_t *map, uintptr_t start, uintptr_t end, uint64_t flags)
{
  procmm_area_t *a = kcalloc(1, sizeof(procmm_area_t));
  a->start = start;
  a->end = end;
  a->flags = flags;
  a->map = map;
  LIST_INIT(a, areas);

  LIST_FOREACH(map->areas, procmm_area_t, o, areas)
  {
    if(!((o->end < start) || (o->start > a->end)))
    {
      // Overlapping areas
      debug_error("PROCMM - Overlapping areas (unsupported for now)\n");
      kfree(a);
      return 0;
    }
    if(o->start > a->end)
    {
      LIST_INSERT_BEFORE(map->areas, a, areas, o);
    }
  }
  if(LIST_EMPTY(a->areas))
    LIST_APPEND(map->areas, a, areas);

  uintptr_t p = a->start;
  while(p < a->end)
  {
    vmm_set_page(map->P4, p, pmm_alloc(), PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
    p += PAGE_SIZE;
  }

  return a;
}
void procmm_unmap(procmm_area_t *a)
{
  uintptr_t p = a->start;
  while(p < a->end)
  {
    uintptr_t ph = vmm_get_page(a->map->P4, p);
    pmm_free(ph);
    vmm_set_page(a->map->P4, p, 0, 0);
    p += PAGE_SIZE;
  }
  LIST_REMOVE(a->map->areas, a, areas);
  kfree(a);
}

int procmm_setup(process_t *proc, size_t brk_size)
{
  procmm_mmap_t *map = proc->mmap;
  procmm_area_t *last_a = map->areas.prev;

  uintptr_t brk_start = (last_a->end + PAGE_SIZE) & ~(PAGE_SIZE-1);
  map->brk = procmm_map(map, brk_start, brk_start + brk_size, 0);

  map->stack = procmm_map(map, USERSPACE_TOP, USERSPACE_TOP, 0);

  return brk_start;
}

registers_t *procmm_page_fault(registers_t *r)
{
  procmm_mmap_t *map = get_current_process()->mmap;
  uintptr_t addr = read_cr2();

  // We don't want to handle stuff in the kernel
  if(addr >= KERNEL_OFFSET)
    return 0;

  // Check if we're close to the stack
  if(map->stack)
  {
    if(map->stack->start <= addr + PAGE_SIZE)
    {
      debug("Increase user stack\n");
      map->stack->start = map->stack->start - PAGE_SIZE;
      vmm_set_page(map->P4, map->stack->start, pmm_alloc(), PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
      return r;
    }
  }

  // If we reach this point, procmm couldn't handle the page fault
  return 0;
}
