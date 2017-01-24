#pragma once

#define CPUID_FUNCTION_VENDOR   0x00000000
#define CPUID_FUNCTION_FEATURES 0x00000001
#define CPUID_FUNCTION_TLB      0x00000003
#define CPUID_FUNCTION_SERIAL   0x00000004

#define CPUID_FUNCTIONX_VENDOR    0x80000000
#define CPUID_FUNCTIONX_FEATURES  0x80000001
#define CPUID_FUNCTIONX_BRAND0    0x80000002
#define CPUID_FUNCTIONX_BRAND1    0x80000003
#define CPUID_FUNCTIONX_BRAND2    0x80000004

#define CPUID_FEATURE_MSR (cpuid_features_d & 1<<5)
#define CPUID_FEATURE_APIC (cpuid_features_d & 1<<9)
#define CPUID_FEATURE_SYSCALL (cpuid_featuresx_d & 1<<11)

#ifndef __ASSEMBLER__

#include <stdint.h>

void cpuid(uint32_t code, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d);

extern uint32_t cpuid_features_b, cpuid_features_c, cpuid_features_d;
extern uint32_t cpuid_featuresx_b, cpuid_featuresx_c, cpuid_featuresx_d;
extern uint32_t cpuid_signature;
extern uint32_t cpuid_max, cpuid_maxx;

#endif
