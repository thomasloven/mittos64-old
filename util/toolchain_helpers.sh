#!/usr/bin/env bash

. util/helpers.sh

# check_toolchain

readonly STASH=${TOOLCHAIN}/stash
readonly PREFIX=${TOOLCHAIN}

function fail() {
  print_error "Something went wrong"
  print_info "You may be able to find some information about what in ${STASH}/error-PACKAGE"
  die "Building toolchain failed"
}

function download() {
  # download url
  # Downloads the file at url unless cached
  # Return true if a file was downloaded
  local url=$1

  pushd "${STASH}" >/dev/null
    if [[ ${url} == git* ]]; then
      local filename=$(basename "${url}" .git)
      if [[ ! -d ${filename} ]]; then
        echo "Cloning" "${filename}"
        /usr/bin/env git clone --depth 1 "${url}" \
          >/dev/null 2>&1 || fail
        print_ok "Clone complete"
        return 0
      else
        print_ok "Using cached ${filename}"
        return 1
      fi
    else
      local filename=$(basename "${url}")
      if [[ ! -f ${filename} ]]; then
        echo "Downloading" "${filename}"
        /usr/bin/env curl -# -O "${url}" || fail
        /usr/bin/env tar -xf "${filename}" || fail
        print_ok "Download complete"
        return 0
      else
        print_ok "Using cached" "${filename}"
        return 1
      fi
    fi
  popd >/dev/null
}

function check() {
  # check package
  # Check if a package is installed
  # Returns true if installed
  local package=$1
  local file=${package}_checkfile
  if [[ -f ${!file} ]]; then
    return 0
  fi
  return 1
}

function config_make_install() {
  local package=$1
  local filename=${package}_filename
  local config=${package}_config
  local make=${package}_make
  local install=${package}_install

  mkdir -p "${STASH}/build-${package}"
  pushd "${STASH}/build-${package}" >/dev/null || fail
    rm -rf ./*

    echo "Configuring"
    ../${!filename}/configure \
      --prefix=${PREFIX} \
      ${!config-} \
      >/dev/null 2>>"${STASH}/error-${package}.log" || fail

    echo "Building"
    make --jobs=100 ${!make-all} \
      >/dev/null 2>>"${STASH}/error-${package}.log" || fail

    echo "Installing"
    make ${!install-install} \
      >/dev/null 2>>"${STASH}/error-${package}.log" || fail
  popd >/dev/null
}

function build_package() {
  local package=$1
  local filename=${package}_filename
  local url=${package}_url
  local patch=${package}_patchcmd

  if check ${package}; then
    print_ok "Package ${package} already built"
    return 0
  fi

  print_info "Building ${package}"

  echo "Downloading"
  if download ${!url}; then
    pushd "${STASH}/${!filename}" >/dev/null || fail
    echo "Applying patches / preparing to build"
      eval ${!patch}
    popd >/dev/null
  fi

  echo "Building and installing"
  if type -t "${package}_install" >/dev/null; then
    eval "${package}_install"
  else
    config_make_install ${package}
  fi

  print_ok "Build complete"

}
