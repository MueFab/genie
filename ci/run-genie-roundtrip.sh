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

run git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly genie="${git_root_dir}/cmake-build/source/apps/genie/genie"
if [[ ! -x "${genie}" ]]; then
    fatal "Genie application '${genie}' is not executable"
    exit 1
fi

run "${genie}" \
    resources/test-files/fastq/fourteen-gattaca-records.fastq \
    --config-file-path resources/gabac-configs/

run "${genie}" \
    resources/test-files/fastq/fourteen-gattaca-records.genie \
    --config-file-path resources/gabac-configs/

"${git_root_dir}/scripts/cmp-reordered-fastq.sh" \
    -1 resources/test-files/fastq/fourteen-gattaca-records.fastq \
    -2 resources/test-files/fastq/fourteen-gattaca-records.genie-decompressed.fastq

info "Cleaning up"
run rm resources/test-files/fastq/fourteen-gattaca-records.genie
run rm resources/test-files/fastq/fourteen-gattaca-records.genie-decompressed.fastq
