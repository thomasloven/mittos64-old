#!/usr/bin/env bash

. util/toolchain_helpers.sh

bindir=${TOOLCHAIN}/bin

packages=(automake autoconf musl_headers binutils gcc musl gdb)
if [[ `uname` == 'Darwin' ]]; then
  packages=("${packages[@]}" objconv)
fi
packages=("${packages[@]}" grub)

musl_checkfile=${SYSROOT}/usr/lib/libc.a
musl_filename=musl
musl_url=git://git.musl-libc.org/musl
musl_config="--target=${TARGET} \
  --prefix=${SYSROOT}/usr \
  --disable-shared \
  --enable-debug \
  CFLAGS=-O0"

musl_headers_checkfile=${SYSROOT}/usr/include/complex.h
musl_headers_filename=${musl_filename}
musl_headers_url=${musl_url}
function musl_headers_install() {
  local package=musl

  pushd ${STASH}/${package} >/dev/null

    echo "Configuring"
    echo "ARCH=x86_64">config.mak
    echo "prefix=${SYSROOT}/usr">>config.mak
    echo "includedir=\$(prefix)/include">> config.mak
    echo "Compiling"
    make install-headers \
      >/dev/null 2>>"${STASH}/error-${package}.log" || fail
  popd >/dev/null
}

# Binutils 2.26
binutils_checkfile=${bindir}/${TARGET}-ld
binutils_filename=binutils-2.26
binutils_url=ftp://ftp.gnu.org/gnu/binutils/${binutils_filename}.tar.gz
binutils_config="--target=${TARGET} \
  --with-sysroot=${SYSROOT} \
  --disable-nls \
  --disable-werror"
binutils_patchcmd="patch -p0 -N < ${BUILDROOT}/util/binutils.patch; cd ld; automake"

# GCC 6.1.0
gcc_checkfile=${bindir}/${TARGET}-gcc
gcc_filename=gcc-6.1.0
gcc_url=ftp://ftp.gnu.org/gnu/gcc/${gcc_filename}/${gcc_filename}.tar.gz
gcc_config="--target=${TARGET} \
      --disable-nls \
      --enable-languages=c,c++ \
      --with-sysroot=${SYSROOT}"
gcc_make="all-gcc all-target-libgcc"
gcc_install="install-gcc install-target-libgcc"
gcc_patchcmd="patch -p0 -N < ${BUILDROOT}/util/gcc.patch; cd libstdc++-v3; autoconf"

# GDB 7.12
gdb_checkfile=${bindir}/x86_64-elf-linux-gdb
gdb_filename=gdb-7.12
gdb_url=ftp://ftp.gnu.org/gnu/gdb/${gdb_filename}.tar.gz
gdb_config="--target=x86_64-elf-linux \
      --disable-debug \
      --disable-dependency-tracking \
      --with-python=/usr"

# objconv
# This is required for building grub on OSX
objconv_checkfile=${bindir}/objconv
objconv_filename=objconv
objconv_url=git://github.com/vertis/objconv.git
function objconv_install() {
  # Download and build objconv
  local package=objconv

  pushd ${STASH}/${package} >/dev/null

    echo "Compiling"
    g++ -o ${TOOLCHAIN}/bin/objconv -O2 src/*.cpp \
      >/dev/null 2>>"${STASH}/error-${package}.log" || fail
  popd >/dev/null
}

# GRUB
# The target x86_64-elf is close enough to what we want
# Make sure to use the right target compiler chains
grub_checkfile=${bindir}/grub-mkimage
grub_filename=grub
grub_url=git://git.savannah.gnu.org/grub.git
grub_config="--target=x86_64-elf \
      --disable-werror \
      TARGET_CC=${TARGET}-gcc \
      TARGET_OBJCOPY=${TARGET}-objcopy \
      TARGET_STRIP=${TARGET}-strip \
      TARGET_NM=${TARGET}-nm \
      TARGET_RANLIB=${TARGET}-ranlib"
grub_patchcmd="autogen.sh"

# automake 1.11.1
# GCC requires exactly this version
automake_checkfile=${TOOLCHAIN}/bin/automake
automake_filename=automake-1.11.1
automake_url=ftp://ftp.gnu.org/gnu/automake/${automake_filename}.tar.gz

# autoconf 2.64
# GCC requires exactly this version
autoconf_checkfile=${TOOLCHAIN}/bin/autoconf
autoconf_filename=autoconf-2.64
autoconf_url=ftp://ftp.gnu.org/gnu/autoconf/${autoconf_filename}.tar.gz

function checkall() {
  for package in "${packages[@]}"
  do
    check ${package} || return 1
  done
  return 0
}

function main() {
  # If called with -c the script will check if a rebuild is necessary
  while getopts "c" OPTION
  do
    case ${OPTION} in
      c)
        checkall
        exit $?
        ;;
    esac
  done

  print_info "Preparing toolchain"

  mkdir -p "${STASH}"
  mkdir -p "${SYSROOT}/usr/include"

  PATH=${TOOLCHAIN}/bin:${PATH}
  for package in "${packages[@]}"
  do
    build_package ${package} || return 1
  done
}

main "${@}"
