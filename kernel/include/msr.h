#pragma once

#define MSR_APIC_BASE     0x0000001B
#define MSR_REG_EFER      0xc0000080
#define MSR_REG_STAR      0xc0000081
#define MSR_REG_LSTAR     0xc0000082
#define MSR_REG_FMASK     0xc0000084
#define MSR_REG_USER_GS   0xc0000101
#define MSR_REG_KERNEL_GS 0xc0000102

#ifndef __ASSEMBLER__

#include <stdint.h>
uint64_t msr_read(uint32_t msr);
void msr_write(uint32_t msr, uint64_t value);

#endif
