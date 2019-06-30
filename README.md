MITTOS64
========

# ALSO SEE [mittos64](https://github.com/thomasloven/mittos64) FOR A NEWER VERSION OF THIS

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

In order to build the boot disk and run the emulator you'll need xorriso and
qemu - also installable by homebrew.

    homebrew install xorriso qemu


A note on qemu and compatibility
--------------------------------
I have had great trouble with qemu, gdb and the x86_64 structure in cobination.
For now, everything seem to work in the setup I'm using if the installed
version of qemu is commit: a470b33259bf82ef2336bfcd5d07640562d3f63b built with
--enable-curses.
You may need to remove the check for wide character support in the qemu
./configure file if building under OSX with homebrew (e.g. by deleting lines:
2953, 2954, 2956, 2957, 2958)

This is tested for GDB 7.12.

Among the problems that may arise if wrong versions are used are:

- g-packet something something too large in gdb
- gdb thinking sizeof(void *) == 4 even in 64 bit mode, meaning no variables can be viewed, because their addresses are truncated to 32 bits
- qemu crashing when a keyboard irq is received
