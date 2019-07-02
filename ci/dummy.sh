#!/usr/bin/env bash

self="${0}"
self_name="${self##*/}"

datetime=$(date -u +"%Y-%m-%dT%H:%M:%SZ")

trace() { echo "[${self_name}] [${datetime}] [trace] ${*}"; }
debug() { echo "[${self_name}] [${datetime}] [debug] ${*}"; }
info() { echo "[${self_name}] [${datetime}] [info] ${*}"; }
warning() { echo "[${self_name}] [${datetime}] [warning] ${*}"; }
error() { echo "[${self_name}] [${datetime}] [error] ${*}" 1>&2; }
fatal() { echo "[${self_name}] [${datetime}] [fatal] ${*}" 1>&2; }

run() {
    local rc
    info "Running: ${*}"
    "${@}"
    rc="${?}"
    if [[ ${rc} != 0 ]]; then
        fatal "Command failed (RC=${rc}): ${*}"
        exit ${rc}
    fi
}

# Get the Git root directory
info "Verifying that we are inside of a Git repository"
run git rev-parse --git-dir 1>/dev/null
readonly git_root_dir="$(git rev-parse --show-toplevel)"

# Get the fdump executable
readonly fdump="${git_root_dir}/cmake-build/fdump"
if [[ ! -x "${fdump}" ]]; then
    fatal "fdump ('${fdump}') is not executable; aborting test"
    exit 1
fi

# Gather the test files
test_files=()
test_files+=("${git_root_dir}/resources/test_files/0bytes")
test_files+=("${git_root_dir}/resources/test_files/1byte")
test_files+=("${git_root_dir}/resources/test_files/6bytes")
test_files+=("${git_root_dir}/resources/test_files/25bytes")
test_files+=("${git_root_dir}/resources/test_files/500bytes")

# Run fdump on all test files
for i in "${!test_files[@]}"; do
    test_file=${test_files[$i]}
    run "${fdump}" "${test_file}"
done
