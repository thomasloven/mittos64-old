file sysroot/boot/kernel
target remote localhost:1234

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

