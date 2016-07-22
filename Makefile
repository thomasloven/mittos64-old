# Check that the build environment is setup
# Idea taken from https://github.com/klange/toaruos
ifeq ($(MITTOS64),)
  $(error Build environment is not activated. Please source activate)
endif

.PHONY: all

all:

