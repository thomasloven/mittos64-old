#pragma once
#include <stdint.h>
#include <process.h>
#include <thread.h>

void *load_elf(process_t *p, void *data);
thread_t *exec_elf(process_t *p, void *image);
