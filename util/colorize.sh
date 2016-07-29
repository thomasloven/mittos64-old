#!/usr/bin/env bash

function ce()
{
  echo -e "${@}"
}

while read line; do
  echo "$line" | sed \
    -e "s/^\[\(INFO\)\]/\[$(ce \\033[36m)\1$(ce \\033[0m)\]/" \
    -e "s/^\[\(OK\)\]/\[$(ce \\033[32m)\1$(ce \\033[0m)\]/" \
    -e "s/^\[\(WARNING\)\]/\[$(ce \\033[33m)\1$(ce \\033[0m)\]/" \
    -e "s/^\[\(ERROR\)\]/\[$(ce \\033[31m)\1$(ce \\033[0m)\]/"
done
