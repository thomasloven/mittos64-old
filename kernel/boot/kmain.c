#include <debug.h>
#include <multiboot.h>
#include <mem.h>

int kmain(uint64_t multiboot_magic, void *multiboot_data)
{
  debug_init();
  debug_ok("MITTOS64 kernel booted\n");
  debug_build_time();
  debug_git_info();
  multiboot_init(multiboot_magic, P2V(multiboot_data));

  debug_info("BOOT COMPLETE\n");
  for(;;)asm("hlt");
}
