#!/usr/bin/env bash

# Get the Git root directory
readonly git_root_dir="$(git rev-parse --show-toplevel)"

echo "TODOs in the source/ tree:"
grep -r -n --color 'TODO' "$git_root_dir/source"

echo "TODOs in the tests/ tree:"
grep -r -n --color 'TODO' "$git_root_dir/tests"
