#include <stdint.h>

uint64_t msr_read(uint32_t msr)
{
  uint64_t lo, hi;
  asm volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
  return ((hi&0xFFFFFFFF) << 32) | (lo&0xFFFFFFFF);
}

void msr_write(uint32_t msr, uint64_t value)
{
  asm volatile("wrmsr" : : "a" (value&0xFFFFFFFF), "d"(value>>32), "c"(msr));
}
