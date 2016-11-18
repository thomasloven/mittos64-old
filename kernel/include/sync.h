#pragma once

typedef uint64_t lock_t;

void spin_lock(lock_t *lock);
void spin_unlock(lock_t *lock);
