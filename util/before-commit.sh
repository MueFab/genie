#!/usr/bin/env bash

set -euo pipefail

self="${0}"
self_name="${self##*/}"

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

run() {
    echo "[${self_name}] running: ${1}"
    "${1}"
}

run "${git_root_dir}/util/authors.sh"
run "${git_root_dir}/util/clang-format.sh"
run "${git_root_dir}/util/shellcheck.sh"
