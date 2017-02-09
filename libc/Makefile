ifeq ($(MITTOS64),)
  $(error Build environment is not activated. Please source activate)
endif

LIBDIR := $(SYSROOT)/usr/lib
LIBC := $(LIBDIR)/libc.a

CRT := crt0
CRT_OBJ := $(addprefix obj/, $(patsubst %,%.o,$(CRT)))
SRC := $(filter-out $(CRT).%, $(wildcard *.[cS]))
OBJ := $(addprefix obj/, $(patsubst %,%.o, $(basename $(SRC))))

libfile := mittos64.o
LIB_OBJ := $(LIBC)($(libfile))

CFLAGS := -Wall -Wextra -ggdb -O0
ASFLAGS := -ggdb

all: $(CRT_OBJ) $(OBJ)

OBJ_DIRS := $(sort $(dir $(CRT_OBJ) $(OBJ)))
$(CRT_OBJ): | $(OBJ_DIRS)
$(OBJ_DIRS):
	mkdir -p $@

obj/%.o:%.c
	$(COMPILE.c) $^ -o $@
obj/%.o:%.S
	$(COMPILE.S) $^ -o $@
obj/$(libfile):$(OBJ)
	$(LD) -r $^ -o $@

$(LIBC)(%):obj/%
	$(AR) -d $@ $<
	$(AR) -rs $@ $<

$(LIBDIR)/%: obj/%
	cp $< $@

install: $(patsubst %,$(LIBDIR)/%.o,$(CRT)) $(LIB_OBJ)

clean:
	rm -rf obj/

.PHONY: all clean install
