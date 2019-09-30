#!/usr/bin/env bash

set -euo pipefail

self="${0}"
self_name="${self##*/}"

# Get Git root directory
git rev-parse --git-dir 1>/dev/null # Exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

run() {
    echo "[${self_name}] running: ${1}"
    "${1}"
}

run "${git_root_dir}/util/authors.sh"
# run "${git_root_dir}/util/autopep8-pycodestyle.sh"
run "${git_root_dir}/util/clang-format.sh"
run "${git_root_dir}/util/shellcheck.sh"
run "${git_root_dir}/util/todo.sh"
