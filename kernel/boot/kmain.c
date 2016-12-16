#include <debug.h>
#include <int.h>
#include <multiboot.h>
#include <mem.h>
#include <gdt.h>
#include <scheduler.h>
#include <thread.h>
#include <process.h>

void thread_function()
{
  char *str = (char *)0x10000;
  str[0] = (char)((unsigned int)'0' + get_current_process()->pid);
  str[1] = (char)((unsigned int)'0' + get_current_thread()->tid);
  str[3] = '-';
  str[2] = '\0';
  while(1)
  {
    debug((char *)0x10000);
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

  process_t *p1 = process_spawn(0);
  process_t *p2 = process_spawn(p1);

  thread_t *t1 = new_thread(thread_function);
  thread_t *t2 = new_thread(thread_function);
  thread_t *t3 = new_thread(thread_function);

  process_attach(p1, t1);
  process_attach(p2, t2);
  process_attach(p1, t3);

  vmm_set_page(p1->P4, 0x10000, pmm_alloc(), PAGE_PRESENT | PAGE_WRITE);
  vmm_set_page(p2->P4, 0x10000, pmm_alloc(), PAGE_PRESENT | PAGE_WRITE);

  scheduler_insert(t1);
  scheduler_insert(t2);
  scheduler_insert(t3);

  debug_info("BOOT COMPLETE\n");
  schedule();
  debug_error("PANIC - This line should be unreachable (%s:%d)\n", __FILE__, __LINE__);
  for(;;)asm("hlt");
}
