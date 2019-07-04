#!/usr/bin/env bash

# Generate an AUTHORS file based on the output of git shortlog. Uses ABC
# order, strips out leading spaces and numbers, then filters out specific
# authors.

git rev-parse --git-dir 1>/dev/null || exit 1 # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

git shortlog --summary --email \
  | perl -spe 's/^\s+\d+\s+//' \
  | sed -e '/^CommitSyncScript.*$/d' \
  > AUTHORS
