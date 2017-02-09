#include <stdio.h>
#include <unistd.h>
int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  fork();
  for(;;);
  return 0;
}
