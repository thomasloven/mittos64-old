#include <debug.h>
#include <int.h>
#include <multiboot.h>
#include <mem.h>
#include <gdt.h>
#include <scheduler.h>

void thread_function()
{
  while(1)
  {
    debug("%d", get_current_thread()->tid);
    schedule();
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
  gdt_init();
  scheduler_init();


  scheduler_insert(new_thread(thread_function));
  scheduler_insert(new_thread(thread_function));
  scheduler_insert(new_thread(thread_function));
  scheduler_insert(new_thread(thread_function));
  scheduler_insert(new_thread(thread_function));

  debug_info("BOOT COMPLETE\n");
  schedule();
  debug_error("PANIC - This line should be unreachable (%s:%d)\n", __FILE__, __LINE__);
  for(;;)asm("hlt");
}
