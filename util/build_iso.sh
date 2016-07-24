#!/usr/bin/env bash

set -e

. util/helpers.sh

function fail() {
  print_error "Something went wrong"
  die "Building boot iso failed"
}

SYSROOT=${SYSROOT-${BUILDROOT}/sysroot}

function collect() {
print_info "Collecting sysroot"

mkdir -p ${SYSROOT}

# Grub menu configuration
mkdir -p ${SYSROOT}/boot/grub
cp ${BUILDROOT}/util/grub.cfg ${SYSROOT}/boot/grub/grub.cfg
}

function mkimage() {
print_info "Making boot disk"
grub-mkrescue -o ${BUILDROOT}/mittos64.iso ${SYSROOT}
}

collect
mkimage
