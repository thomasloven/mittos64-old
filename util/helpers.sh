#!/usr/bin/env bash


function print_info() {
  echo -e "[\\033[36mINFO\\033[0m] $@"
}
function print_ok() {
  echo -e "[\\033[32mOK\\033[0m] $@"
}
function print_warning() {
  echo -e "[\\033[33mWARNING\\033[0m] $@"
}
function print_error() {
  echo -e "[\\033[31mERROR\\033[0m] $@"
}
function die() {
  print_error $@
  exit 1
}

function check_toolchain() {
  if [[ -z ${MITTOS64+x} ]]; then
    die "TOOLCHAIN is not set. Please source activate"
    exit 1
  fi
}

check_toolchain

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
  # Test output functions
  print_info "This is an information message"
  print_ok "This is OK"
  print_warning "Be careful"
  print_error "Something bad happened"
fi
