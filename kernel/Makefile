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
# Special file for keeping git information up to date
VERSION_OBJ := obj/boot/version.o


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

# Git status flags for the version file
GITHASH := $(shell git log -1 --pretty="tformat:%h")
GITDATE := $(shell git log -1 --pretty="tformat:%cd")
GITDIRTY := $(shell git status -s >/dev/null 2>/dev/null && echo 1 || echo 0)
GITMESSAGE := $(shell git log -1 --pretty="tformat:%s")
GITBRANCH := $(shell git log -1 --pretty="tformat:%d")
GITFLAGS := -DGITHASH='"$(GITHASH)"' \
	-DGITDATE='"$(GITDATE)"' \
	-DGITDIRTY='$(GITDIRTY)' \
	-DGITMESSAGE='"$(GITMESSAGE)"' \
	-DGITBRANCH='"$(GITBRANCH)"'
$(VERSION_OBJ): CFLAGS += $(GITFLAGS)

# The kernel needs some special flags
$(KERNEL): LDFLAGS += -n -nostdlib -T Link.ld
$(KERNEL): LDLIBS := -lgcc
$(KERNEL): $(KERNEL_OBJS)
	rm -rf $(VERSION_OBJ)
	$(MAKE) $(VERSION_OBJ)
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
