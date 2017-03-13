#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;


  FILE *fp = fopen("/dev/debug", "w");
  fprintf(fp, "Hello, filesystem!\n");


  for(;;);
  return 0;
}
