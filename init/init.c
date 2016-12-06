#include <stdio.h>
#include <unistd.h>
int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  asm("syscall" :: "a" (0x3FF), "D" ("Hello, world!\n"));
  for(;;);
  return 0;
}
