#!/usr/bin/env bash

git rev-parse --git-dir 1>/dev/null || exit 1 # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

echo "TODOs in the source/ tree:"
grep --color --line-number --recursive 'TODO' "${git_root_dir}/source"

echo "TODOs in the tests/ tree:"
grep --color --line-number --recursive 'TODO' "${git_root_dir}/tests"

echo "FIXMEs in the source/ tree:"
grep --color --line-number --recursive 'FIXME' "${git_root_dir}/source"

echo "FIXMEs in the tests/ tree:"
grep --color --line-number --recursive 'FIXME' "${git_root_dir}/tests"
