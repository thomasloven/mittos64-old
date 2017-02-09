#!/usr/bin/env bash

set -e

function main()
{
  readonly musl_dir=$1

  syscall_arch=${musl_dir}/arch/x86_64/syscall_arch.h

  syscall_in=${musl_dir}/arch/x86_64/bits/syscall.h.in

  syscall_dir=${musl_dir}/src/internal/x86_64/

  rm ${musl_dir}/src/thread/x86_64/__set_thread_area.s
  rm ${syscall_dir}/syscall.s

  cat >> ${syscall_arch} << EOF
#define SYSCALL_NO_INLINE
EOF

  cat > ${syscall_dir}/syscall_1.c << EOF
#include <stdarg.h>

#pragma weak __syscall_common
long __syscall_common(long num, ...)
{
  for(;;);
}
long __syscall_default(long num, ...)
{ for(;;); }

typedef long (*syscall_fn)(long, ...);

EOF

  sed -n -E 's/.*__NR_([^[:space:]]+).*/long __attribute__\(\(weak\)\) __syscall_\1\(long, ...\);/p' < ${syscall_in} >> ${syscall_dir}/syscall_1.c

  cat >> ${syscall_dir}/syscall_1.c << EOF

syscall_fn __syscall_fns[] = {
EOF

  sed -n -E 's/.*__NR_([^[:space:]]+)[[:space:]]+([0-9]+).*/\[\2\] = __syscall_\1,/p' < ${syscall_in} >> ${syscall_dir}/syscall_1.c

  cat >> ${syscall_dir}/syscall_1.c << EOF

};

EOF
cat >${syscall_dir}/syscall_2.c << EOF
#include <stdarg.h>
typedef long (*syscall_fn)(long, ...);
extern long __syscall_common(long, ...);
extern syscall_fn __syscall_fns[];

long __syscall(long num, ...)
{
  va_list args;
  va_start(args, num);
  long a1 = va_arg(args, long);
  long a2 = va_arg(args, long);
  long a3 = va_arg(args, long);
  long a4 = va_arg(args, long);
  long a5 = va_arg(args, long);
  long a6 = va_arg(args, long);
  va_end(args);

  if(__syscall_fns[num])
    return __syscall_fns[num](num, a1, a2, a3, a4, a5, a6);
  else
    return __syscall_common(num, a1, a2, a3, a4, a5, a6);
}
EOF

cat >${musl_dir}/arch/x86_64/pthread_arch.h <<EOF
struct pthread *__pthread_self();
#define TP_ADJ(p) (p)
#define MC_PC gregs[REG_RIP]
EOF

}

main $@
