#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  printf("Hello, world!\n");
  printf("Some numbers:%d\n", 12345);
  printf("And hex:%#x\n", 0xabcdef);
  printf("And a string:%s\n", "Hello!");

  printf("Arguments passed to init:\n");
  for(int i=0; i < argc; i++)
    printf("%d: %s\n", i, argv[i]);
  for(;;);
  return 0;
}
