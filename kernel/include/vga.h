#pragma once

#define VGA_WIDTH 80
#define VGA_LINES 25
#define VGA_SIZE (VGA_WIDTH*VGA_LINES)

void vga_init();
void vga_printch(char c);
