#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  int num = 5;

  if(fork())
  {
    num = 10;
    printf("I am the parent! - %d\n", num);
  } else {
    num = 3;
    printf("I am the child! - %d\n", num);
  }

  for(;;);
  return 0;
}
