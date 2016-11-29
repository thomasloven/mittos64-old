ifeq ($(MITTOS64),)
  $(error Build environment is not activated. Please source activate)
endif


installdir := $(SYSROOT)/boot


CFLAGS ?= -Wall -Wextra -ggdb -O0


all: init

$(installdir)/init: init
	mkdir -p $(dir $@)
	cp $< $@

install: $(installdir)/init

clean:
	rm -rf init

.PHONY: all clean install
