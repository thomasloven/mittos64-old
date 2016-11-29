ifeq ($(MITTOS64),)
  $(error Build environment is not activated. Please source activate)
endif

LIBDIR := $(SYSROOT)/usr/lib

CRT := crt0
CRT_OBJ := $(addprefix obj/, $(patsubst %,%.o,$(CRT)))

ASFLAGS := -ggdb

all: $(CRT_OBJ)

OBJ_DIRS := $(sort $(dir $(CRT_OBJ)))
$(CRT_OBJ): | $(OBJ_DIRS)
$(OBJ_DIRS):
	mkdir -p $@

obj/%.o:%.S
	$(COMPILE.S) $^ -o $@

$(LIBDIR)/%: obj/%
	cp $< $@

install: $(patsubst %,$(LIBDIR)/%.o,$(CRT))

clean:
	rm -rf obj/

.PHONY: all clean install
