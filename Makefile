# Check that the build environment is setup
# Idea taken from https://github.com/klange/toaruos
ifeq ($(MITTOS64),)
  $(error Build environment is not activated. Please source activate)
endif

.PHONY: all clean
SHELL := bash

all:

clean:
	@(. util/helpers.sh; print_info "Cleaning up")
	rm -f mittos64.iso
	rm -f qemu-error.log
