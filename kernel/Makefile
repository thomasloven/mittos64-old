#
# Makefile for mittos64 kernel
# (c) Thomas Lovén 2016-2017
# mittos64@thomasloven.com
#

ifeq ($(MITTOS64),)
  $(error Build environment is not activated. Please source activate)
endif


# Find all source files and corresponding object files
KERNEL_SRC := $(wildcard **/*.[cS])
KERNEL_OBJS := $(addprefix obj/,$(patsubst %,%.o,$(basename $(KERNEL_SRC))))

# Kernel object file
KERNEL := obj/boot/kernel


# Default compilation flags
CFLAGS ?= -Wall -Wextra
CFLAGS += -ffreestanding -mcmodel=large
CPPFLAGS += -Iinclude
# Optimize only if not debugging
ifdef NDEBUG
CFLAGS += -O2
CPPFLAGS += -DNDEBUG
else
CFLAGS += -ggdb -O0
ASFLAGS += -ggdb
endif

KERNEL_DEP := $(KERNEL_OBJS:.o=.d)
DEPFLAGS = -MT $@ -MMD -MP -MF obj/$*.d

all: $(KERNEL)

# Make sure the object directories exist
OBJ_DIRS := $(sort $(dir $(KERNEL_OBJS)))
$(KERNEL_OBJS): | $(OBJ_DIRS)
$(OBJ_DIRS):
	mkdir -p $@

# The kernel needs some special flags
$(KERNEL): LDFLAGS += -n -nostdlib -T Link.ld
$(KERNEL): LDLIBS := -lgcc
$(KERNEL): $(KERNEL_OBJS)
	$(LINK.c) $^ -o $@

# Use the default make compilation rules
obj/%.o: %.c obj/%.d
	$(COMPILE.c) $(DEPFLAGS) $< -o $@
obj/%.o: %.S obj/%.d
	$(COMPILE.S) $(DEPFLAGS) $< -o $@
obj/%.d: ;

# For installing stuff
${SYSROOT}/%: obj/%
	mkdir -p $(dir $@)
	cp $< $@

install-kernel: ${SYSROOT}/boot/kernel

install: install-kernel

clean:
	rm -rf obj/

-include $(KERNEL_DEP)

.PHONY: all clean install install-kernel
