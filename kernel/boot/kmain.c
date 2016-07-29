#include <debug.h>

int kmain(void)
{
  debug_init();
  debug_ok("MITTOS64 kernel booted\n");
  debug_build_time();
  debug_git_info();

  // Test the printf functions
  debug("binary:%b octal:%o dec:%d\n", 0xAA55, 0123, 456);
  debug("hex:%x string:%s char:%c\n", 0xabcd, "Hello", 'T');
  debug("pointer:%x\n", kmain);

  debug_info("An information string\n");
  debug_ok("%s prints ok\n", "This string");
  debug_warning("A warning message\n");
  debug_error("%d is less than %x\n", 12, 17);

  debug_info("BOOT COMPLETE\n");
  for(;;)asm("hlt");
}
