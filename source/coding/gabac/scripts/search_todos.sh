#!/usr/bin/env bash

# Get the Git root directory
readonly git_root_dir="$(git rev-parse --show-toplevel)"

# Search all TODOs
grep -r -n --color 'TODO' "$git_root_dir/source" || exit 1
grep -r -n --color 'TODO' "$git_root_dir/tests" || exit 1
