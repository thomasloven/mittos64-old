#pragma once

#ifndef __ASSEMBLER__
void load_idt(void *);
uint64_t read_cr0();
uint64_t read_cr2();
uint64_t read_cr3();
void write_cr3(uint64_t);
uint64_t read_cr4();
#endif

#define CR0_MP (1<<1)
#define CR0_EM (1<<2)

#define CR4_OSFXSR (1<<9)
#define CR4_OSXMMEXCPT (1<<10)
