#pragma once

#define   GDT_WRITE      (1<<41)
#define   GDT_EXECUTE    (1<<43)
#define   GDT_CODEDATA   (1<<44)
#define   GDT_PRESENT    (1<<47)
#define   GDT_64BIT      (1<<53)

#ifndef __ASSEMBLER__
#include <stdint.h>
extern uint64_t BootGDT;
#endif
