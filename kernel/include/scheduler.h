#pragma once
#include <thread.h>

void schedule();
void scheduler_insert(thread_t *th);
void scheduler_init();
