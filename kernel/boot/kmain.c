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

  // Write thread code to address
  procmm_map(p1->mmap, addr, addr+PAGE_SIZE, 0);
  vmm_p4_memcpy(p1->mmap->P4, (void *)addr, 0, thread_function, PAGE_SIZE);
  procmm_setup(p1);

  procmm_print_map(p1->mmap);

  process_attach(p1, th);
  scheduler_insert(th);

  asm("sti");
  debug_info("BOOT COMPLETE\n");
  schedule();
  debug_error("PANIC - This line should be unreachable (%s:%d)\n", __FILE__, __LINE__);
  for(;;)asm("hlt");
}
