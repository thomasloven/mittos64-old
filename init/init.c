#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  char *a = malloc(0x100);
  printf("Pointer: %p\n", a);
  char *b = malloc(0x100);
  printf("Pointer: %p\n", b);
  char *c = malloc(0x10);
  printf("Pointer: %p\n", c);
  c[0] = 5;
  free(a);
  free(b);
  free(c);

  for(;;);
  return 0;
}
