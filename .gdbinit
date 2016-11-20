file sysroot/boot/kernel
target remote localhost:1234
break int_handler_breakpoint

define q
monitor quit
end

define reg
monitor info registers
end

define mm
monitor info mem
end

define cpu
monitor info cpus
end

define reset
monitor system_reset
end

define restore_env
set $name = $arg0
python 
registers = ['rax', 'rbx', 'rcx', 'rdx', 'rsi',
  'rdi', 'rbp', 'rsp', 'r8', 'r9', 'r10', 'r11',
  'r12', 'r13', 'r14', 'r15', 'rip']
values = {}
for r in registers:
  values[r] = gdb.parse_and_eval('$name->{}'.format(r))

for r in registers:
  gdb.parse_and_eval('${}={}'.format(r, values[r]))
gdb.execute('frame 0')
end
