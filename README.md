MITTOS64
========

By Thomas Lovén - 2016

For development, start by sourcing `activate.sh`

    source ./activate.sh

Running `make` will automatically download and compile a cross compiler
toolchain and the grub bootloader.

It will also download and build objconv, which is required for building grub
under OSX. This will probably fail if you're not on a mac.

Building the cross compiler requires a c compiler as well as gmp, mpfr and mpc.
The c compiler installed by xcode command line tools will do. It's installed
automatically if you install homebrew. The rest of the packages can be
installed with homebrew. You'll also want homebrews ctags.

    homebrew install gmp mpfr libmpc ctags
