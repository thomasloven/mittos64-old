#pragma once

void syscall_init();

typedef long (*syscall_handler_t)(long num, long, long, long, long, long, long);

#define SYSCALL_DECL(name) \
  long syscall_##name(long, long, long, long, long, long, long)

#define SYSCALL_DEF(name) \
  long syscall_##name(long num, long _a1, long _a2, long _a3, long _a4, long _a5, long _a6)

#define _SYSCALL_INIT6(t1,n1,t2,n2,t3,n3,t4,n4,t5,n5,t6,n6) \
  (void)num; \
  t1 n1 = (t1)_a1; \
  (void)n1; \
  t2 n2 = (t2)_a2; \
  (void)n2; \
  t3 n3 = (t3)_a3; \
  (void)n3; \
  t4 n4 = (t4)_a4; \
  (void)n4; \
  t5 n5 = (t5)_a5; \
  (void)n5; \
  t6 n6 = (t6)_a6; \
  (void)n6;
#define _SYSCALL_INIT5(t1,n1,t2,n2,t3,n3,t4,n4,t5,n5) _SYSCALL_INIT6(t1,n1,t2,n2,t3,n3,t4,n4,t5,n5,long,__a6)
#define _SYSCALL_INIT4(t1,n1,t2,n2,t3,n3,t4,n4) _SYSCALL_INIT6(t1,n1,t2,n2,t3,n3,t4,n4,long,__a5,long,__a6)
#define _SYSCALL_INIT3(t1,n1,t2,n2,t3,n3) _SYSCALL_INIT6(t1,n1,t2,n2,t3,n3,long,__a4,long,__a5,long,__a6)
#define _SYSCALL_INIT2(t1,n1,t2,n2) _SYSCALL_INIT6(t1,n1,t2,n2,long,__a3,long,__a4,long,__a5,long,__a6)
#define _SYSCALL_INIT1(t1,n1) _SYSCALL_INIT6(t1,n1,long,__a2,long,__a3,long,__a4,long,__a5,long,__a6)
#define _SYSCALL_INIT0() _SYSCALL_INIT6(long,__a1,long,__a2,long,__a3,long,__a4,long,__a5,long,__a6)

#define __SYSCALL_NARGS(a0,b0,a1,b1,a2,b2,a3,b3,a4,b4,a5,b5,a6,n,...) n
#define _SYSCALL_NARGS(...) __SYSCALL_NARGS(__VA_ARGS__,6,6,5,5,4,4,3,3,2,2,1,1,0,0)
#define __SYSCALL_CONCAT(a,b) a##b
#define _SYSCALL_CONCAT(a,b) __SYSCALL_CONCAT(a,b)
#define _SYSCALL_INIT(a,...) _SYSCALL_CONCAT(a, _SYSCALL_NARGS(__VA_ARGS__))(__VA_ARGS__)
#define SYSCALL_INIT(...) _SYSCALL_INIT(_SYSCALL_INIT,__VA_ARGS__)

#define SYSCALL_REGISTER(name, num) syscall_handlers[num] = syscall_##name

SYSCALL_DECL(open);
SYSCALL_DECL(close);
SYSCALL_DECL(read);
SYSCALL_DECL(write);
SYSCALL_DECL(isatty);
SYSCALL_DECL(seek);
SYSCALL_DECL(pipe);

SYSCALL_DECL(brk);

SYSCALL_DECL(fork);
SYSCALL_DECL(exit);
SYSCALL_DECL(wait);
