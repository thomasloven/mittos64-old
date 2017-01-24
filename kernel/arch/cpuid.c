#include <cpuid.h>
#include <stdint.h>
#include <debug.h>

uint32_t cpuid_features_b, cpuid_features_c, cpuid_features_d;
uint32_t cpuid_featuresx_b, cpuid_featuresx_c, cpuid_featuresx_d;
uint32_t cpuid_signature;
uint32_t cpuid_max=0x0, cpuid_maxx=0x80000000;


void cpuid(uint32_t code, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
  uint32_t scrap;
  if(!a) a = &scrap;
  if(!b) b = &scrap;
  if(!c) c = &scrap;
  if(!d) d = &scrap;

  if((code < CPUID_FUNCTIONX_VENDOR && code > cpuid_max) || code > cpuid_maxx)
  {
    *a = *b = *c = *d = -1;
    return;
  }

  asm volatile("cpuid" : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d) : "a"(code));
}

