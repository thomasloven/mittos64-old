#pragma once
#include <stdint.h>

#define TICK_FREQ 1000

void pit_init();
uint64_t get_ticks();
