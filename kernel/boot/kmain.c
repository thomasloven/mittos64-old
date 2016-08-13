#include <debug.h>
#include <int.h>
#include <multiboot.h>
#include <mem.h>

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

  // We still need the GDT to be mapped in
  extern void *BootGDT;
  vmm_set_page(0, V2P(&BootGDT), V2P(&BootGDT), PAGE_PRESENT);

  void *a = kmalloc(0x400);
  void *b = kmalloc(0x200);
  void *c = kmalloc(0x100);
  kfree(b);
  void *d = kmalloc(0x100);

  (void)a;
  (void)b;
  (void)c;
  (void)d;

  heap_print();

  debug_info("BOOT COMPLETE\n");
  for(;;)asm("hlt");
}
