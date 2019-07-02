#!/usr/bin/env bash

# Get the Git root directory
readonly git_root_dir="$(git rev-parse --show-toplevel)"

echo "TODOs in the source/ tree:"
grep --color --line-number --recursive 'TODO' "${git_root_dir}/source"

echo "TODOs in the tests/ tree:"
grep --color --line-number --recursive 'TODO' "${git_root_dir}/tests"
