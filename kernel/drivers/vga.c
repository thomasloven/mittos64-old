#include <vga.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <mem.h>
#include <ports.h>


void *vidmem = P2V(0xb8000);
uint16_t buffer[VGA_SIZE];

struct {
  uint32_t x;
  uint32_t y;
  uint8_t style;
} cursor;

void sync()
{
  // Copy the buffer to video memory
  memcpy(vidmem, buffer, sizeof(buffer));
}

void setcursor()
{
  // Move hardware cursor to correspond to the software one
  uint16_t position = cursor.y*VGA_WIDTH + cursor.x;
  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(position & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((position >> 8) & 0xF));
}

void scroll()
{
  // Scroll all lines up and clear the bottom one
  while(cursor.y >= VGA_LINES)
  {
    memmove(buffer, &buffer[VGA_WIDTH], sizeof(buffer)-2*VGA_WIDTH);
    memset(&buffer[VGA_SIZE-VGA_WIDTH], 0, 2*VGA_WIDTH);
    cursor.y--;
  }
}

void vga_printch(char c)
{
  // Print a character to VGA memory
  if(c == '\n')
  {
    // Linebreak
    cursor.x = 0;
    cursor.y++;
  } else {
    buffer[cursor.y*VGA_WIDTH + cursor.x] = c | (cursor.style << 8);
    cursor.x++;
  }

  if(cursor.x >= VGA_WIDTH)
  {
  // Wrap line
    cursor.x = 0;
    cursor.y++;
  }
  scroll();
  sync();
  setcursor();
}

void vga_init()
{
  // Set cursor at top left
  cursor.x = cursor.y = 0;
  cursor.style = 0x07;
  // Clear video memory
  memset(buffer, 0, sizeof(buffer));
  sync();
}
