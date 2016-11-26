#include <debug.h>
#include <multiboot.h>
#include <mem.h>
#include <scheduler.h>
#include <thread.h>
#include <process.h>
#include <cpu.h>
#include <timer.h>
#include <string.h>

void thread_function()
{
  int i = 0;
  while(1)
  {
    i++;
  }
}

int kmain(uint64_t multiboot_magic, void *multiboot_data)
{
  debug_init();
  debug_ok("MITTOS64 kernel booted\n");
  debug_build_time();
  debug_git_info();

  interrupt_init();
  multiboot_init(multiboot_magic, P2V(multiboot_data));
  vmm_init();
  pmm_init();
  cpu_init();
  pit_init();

  process_t *p1 = process_spawn(0);
  uint64_t addr = 0x200000;
  thread_t *th = new_thread((void *)addr, 1);

  uintptr_t page = pmm_alloc();
  // Write thread code to address
  vmm_set_page(p1->P4, addr, page, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
  memcpy(P2V(page), thread_function, PAGE_SIZE);
  // Map a user stack space
  vmm_set_page(p1->P4, USERSPACE_TOP-PAGE_SIZE, pmm_alloc(), PAGE_PRESENT | PAGE_WRITE | PAGE_USER);

  process_attach(p1, th);
  scheduler_insert(th);

  asm("sti");
  debug_info("BOOT COMPLETE\n");
  schedule();
  debug_error("PANIC - This line should be unreachable (%s:%d)\n", __FILE__, __LINE__);
  for(;;)asm("hlt");
}
