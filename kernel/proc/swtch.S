.intel_syntax noprefix

.global swtch

swtch:
# void swtch(uint64_t *old, uint64_t *new)
# Switches stacks preserving callee preserved registers according to System V ABI
  push rbp
  mov rbp, rsp
  push r15
  push r14
  push r13
  push r12
  push rbx
  push rbp
  cmp rdi, 0x0
  jz .switch_in
  mov [rdi], rsp
.switch_in:
  mov rsp, [rsi]
  pop rbp
  pop rbx
  pop r12
  pop r13
  pop r14
  pop r15
  leaveq
  ret
