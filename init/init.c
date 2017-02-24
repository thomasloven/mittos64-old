#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  if(!fork())
  {
    if(!fork())
      return 200;
    return 100;
  }

  for(int j=0; j <= 10; j++)
  {
    if(!fork())
      return j;
  }
  while(1)
  {
    int retval;
    int pid = wait(&retval);
    printf("Pid: %d exited with %d\n", pid, retval);
  }


  for(;;);
  return 0;
}
