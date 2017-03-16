#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;


  FILE *fp = fopen("/dev/sda", "r+");
  char *buffer = malloc(512);

  fread(buffer, 512, 1, fp);
  printf("Read: %hhx %hhx %hhx %hhx\n", buffer[0], buffer[1], buffer[510], buffer[511]);

  buffer[511] ^= 0x1;
  fseek(fp, 0, SEEK_SET);
  fwrite(buffer, 512, 1, fp);
  fclose(fp);

  fp = fopen("/dev/sda", "r");
  fread(buffer, 512, 1, fp);
  printf("Read: %hhx %hhx %hhx %hhx\n", buffer[0], buffer[1], buffer[510], buffer[511]);
  fclose(fp);


  for(;;);
  return 0;
}
