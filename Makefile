# Check that the build environment is setup
# Idea taken from https://github.com/klange/toaruos
ifeq ($(MITTOS64),)
  $(error Build environment is not activated. Please source activate)
endif

.PHONY: all clean kernel
SHELL := bash

CC=$(TARGET)-gcc
FLAGS_TO_PASS:= \
	CC=$(CC)

all: kernel

# A trick to only build phony target if necessary
kernel:
ifeq ($(shell make -sqC kernel || echo 1), 1)
	@(. util/helpers.sh; print_info "Building kernel")
	$(MAKE) -C kernel install $(FLAGS_TO_PASS)
endif

clean:
	@(. util/helpers.sh; print_info "Cleaning up")
	$(MAKE) -C kernel clean
	rm -f mittos64.iso
	rm -f qemu-error.log
