#include <debug.h>
#include <int.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <vga.h>
#include <serial.h>
#include <registers.h>
#include <cpu.h>

lock_t debug_lock;

void debug_init()
{
  spin_unlock(&debug_lock);
  vga_init();
  serial_init(PORT_COM1);
}

uint64_t num2str(uint64_t num, uint32_t base, char *buf)
{
  // Convert a number to string
  if(num == 0)
  {
    buf[0] = '0';
    buf[1] = '\0';
    return 0;
  }
  uint32_t i=0, j=0;
  char chars[] = "0123456789ABCDEF";
  // Build the string starting with the least significant digit
  while(num > 0)
  {
    buf[i++] = chars[num%base];
    num /= base;
  }

  // Invert the string to get digits in right order
  i--;
  j=0;
  while(j<i)
  {
    char t = buf[i];
    buf[i--] = buf[j];
    buf[j++] = t;
  }

  // Zero terminate
  buf[i+j+1] = '\0';
  return i+j+1;
}

void debug_putch(char c)
{
  vga_printch(c);
  serial_write(PORT_COM1, c);
}
void debug_putsn(char *s, size_t n)
{
  while(n--)
    debug_putch(*s++);
}
void debug_puts(char *s)
{
  debug_putsn(s, strlen(s));
}

void debug_vprintf(char *str, va_list args)
{
  size_t len = 0;
  if(!(*str))
    return;
  if(*str != '%')
  {
    while(str[len] && str[len] != '%')
      len++;
    debug_putsn(str, len);
    return debug_vprintf(&str[len], args);
  }
  str++;
  char *prefix;
  uint32_t base = 0;
#define DEBUG_VPRINTF_BASE_CHAR 255
#define DEBUG_VPRINTF_BASE_STR 1024
  switch(*str)
  {
    case 'b':
      prefix = "0b";
      base = 2;
      break;
    case 'o':
      prefix = "0";
      base = 8;
      break;
    case 'd':
      prefix = "";
      base = 10;
      break;
    case 'x':
      prefix = "0x";
      base = 16;
      break;
    case 'c':
      prefix = "";
      base = DEBUG_VPRINTF_BASE_CHAR;
      break;
    case 's':
      prefix = "";
      base = DEBUG_VPRINTF_BASE_STR;
      break;
    case '%':
    default:
      debug_putch('%');
      break;
  }
  if(base == DEBUG_VPRINTF_BASE_STR)
    debug_puts(va_arg(args, char *));
  else if(base == DEBUG_VPRINTF_BASE_CHAR)
    debug_putch((char)va_arg(args, uint64_t));
  else if(base)
  {
    uint64_t num = va_arg(args, uint64_t);
    char buf[128];
    num2str(num, base, buf);
    debug_puts(prefix);
    debug_puts(buf);
  }
  str++;
  return debug_vprintf(str, args);
}

void debug_printf(char *str, ...)
{
  va_list args;
  va_start(args, str);
  debug_vprintf(str, args);
  va_end(args);
}

void print_registers(registers_t *r)
{
  debug("Num:%x Error:%x\n", r->int_no, r->err_code);
  debug("RAX:%x RBX:%x RCX:%x RDX:%x\n", r->rax, r->rbx, r->rcx, r->rdx);
  debug("RSI:%x RDI:%x RBP:%x RSP:%x\n", r->rsi, r->rdi, r->rbp, r->rsp);
  debug("R8:%x R9:%x R10:%x R11:%x\n", r->r8, r->r9, r->r10, r->r11);
  debug("R12:%x R13:%x R14:%x R15:%x\n", r->r12, r->r13, r->r14, r->r15);
  debug("RIP:%x RFL:%x\n", r->rip, r->rflags);
  debug("CS:%x SS:%x\n", r->cs, r->ss);
  debug("CR0:%x CR2:%x CR3:%x CR4:%x\n", read_cr0(), read_cr2(), read_cr3(), read_cr4());
}
