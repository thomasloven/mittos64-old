#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;


  int fd[2];
  pipe(fd);

  pid_t childpid = fork();

  if(childpid)
  {
    printf("I am parent!\n");
    close(fd[1]);
    write(fd[0], "Hi, pipe!\n", 10);
    close(fd[0]);
  }
  else
  {
    printf("I am child!\n");
    close(fd[0]);
    char buffer[255];
    read(fd[1], buffer, 255);
    printf("Received string: %s\n", buffer);
    close(fd[1]);
  }


  for(;;);
  return 0;
}
