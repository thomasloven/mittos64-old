# Check that the build environment is setup
# Idea taken from https://github.com/klange/toaruos
ifeq ($(MITTOS64),)
  $(error Build environment is not activated. Please source activate)
endif

.PHONY: all clean kernel libc init
SHELL := bash

CC=$(TARGET)-gcc
AR=$(TARGET)-ar
LD=$(TARGET)-ld
FLAGS_TO_PASS:= \
	CC=$(CC) \
	AR=$(AR) \
	LD=$(LD)

all: kernel libc init

# A trick to only build phony target if necessary
kernel:
ifeq ($(shell make -sqC kernel || echo 1), 1)
	@(. util/helpers.sh; print_info "Building kernel")
	$(MAKE) -C kernel install $(FLAGS_TO_PASS)
endif

libc:
ifeq ($(shell make -sqC libc || echo 1), 1)
	@(. util/helpers.sh; print_info "Building c library")
	$(MAKE) -C libc install $(FLAGS_TO_PASS)
	$(MAKE) $(MAKECMDGOALS)
endif

init:
ifeq ($(shell make -sqC init || echo 1), 1)
	@(. util/helpers.sh; print_info "Building init")
	$(MAKE) -C init install $(FLAGS_TO_PASS)
endif

tags:
	ctags -R kernel

clean:
	@(. util/helpers.sh; print_info "Cleaning up")
	$(MAKE) -C kernel clean
	rm -f mittos64.iso
	$(MAKE) -C libc clean
	$(MAKE) -C init clean
	rm -f qemu-error.log
	rm -f serial.log
