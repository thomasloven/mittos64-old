#include <mem.h>

#define VGA_MEMORY 0xb8000
#define VGA_ROWS 24
#define VGA_COLS 80

void clear_screen()
{
  // Clear the video memory
  unsigned char *m = (void *)P2V(VGA_MEMORY);
  for(int i = 0; i < VGA_ROWS*VGA_COLS*2; i++)
  {
    *m++ = 0;
  }
}

void prints(const char *s, unsigned int row, unsigned int col)
{
  // Very simple pure ascii string printing
  unsigned char *m = (void *)P2V(VGA_MEMORY);
  m += 2*VGA_COLS*row+2*col;
  while(*s)
  {
    *m++ = *s++;
    *m++ = 0x7;
  }
}

int kmain(void)
{
  clear_screen();
  prints("Hello, world!", 0, 0);
  for(;;)asm("hlt");
}
