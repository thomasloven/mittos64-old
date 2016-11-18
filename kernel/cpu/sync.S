.intel_syntax noprefix
.global spin_lock
spin_lock:
  mov rdx, rdi
  mov rax, 0x0
  mov rcx, 0x1
.retry:
  xor rax, rax
  lock cmpxchg [rdx], cl
  jnz .retry
  ret

.global spin_unlock
spin_unlock:
  movq [rdi], 0x0
  ret
