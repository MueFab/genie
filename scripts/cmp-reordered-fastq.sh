#!/usr/bin/env bash

# Compare two FASTQ files which are assumed to contain the same set of reads,
# but in different orders

# -----------------------------------------------------------------------------
# Bash script setup
# -----------------------------------------------------------------------------

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

# -----------------------------------------------------------------------------
# Command line
# -----------------------------------------------------------------------------

print_usage () {
    info "Usage: ${self_name} [options]"
    info ""
    info "Options:"
    info "  -h,--help              print this help"
    info "  -1,--one FASTQ_FILE_1  1st FASTQ file"
    info "  -2,--two FASTQ_FILE_2  2nd FASTQ file"
}

fastq_file_1=""
fastq_file_2=""

while [[ "${#}" -gt 0 ]]; do
    case ${1} in
        -h|--help) print_usage; exit 1;;
        -1|--one) fastq_file_1="${2}"; shift;;
        -2|--two) fastq_file_2="${2}"; shift;;
        *) error "Unknown parameter passed: ${1}"; print_usage; exit 1;;
    esac
    shift
done

# -----------------------------------------------------------------------------
# Do it
# -----------------------------------------------------------------------------

info "Sorting 1st FASTQ file '${fastq_file_1}'"
if [[ ! -f "${fastq_file_1}" ]]; then error "'${fastq_file_1}' is not a regular file"; exit 1; fi
run paste -d" " - - - - < "${fastq_file_1}" > "${fastq_file_1}.one_record_per_line"
run sort < "${fastq_file_1}.one_record_per_line" > "${fastq_file_1}.sorted"

info "Sorting 2nd FASTQ file '${fastq_file_2}'"
if [[ ! -f "${fastq_file_2}" ]]; then error "'${fastq_file_2}' is not a regular file"; exit 1; fi
run paste -d" " - - - - < "${fastq_file_2}" > "${fastq_file_2}.one_record_per_line"
run sort < "${fastq_file_2}.one_record_per_line" > "${fastq_file_2}.sorted"

info "Comparing FASTQ files '${fastq_file_1}.sorted' and '${fastq_file_2}.sorted'"
run cmp "${fastq_file_1}.sorted" "${fastq_file_2}.sorted"

info "FASTQ files '${fastq_file_1}.sorted' and '${fastq_file_2}.sorted' contain the same set of reads"

# -----------------------------------------------------------------------------
# Clean up
# -----------------------------------------------------------------------------

info "Cleaning up"
run rm "${fastq_file_1}.one_record_per_line"
run rm "${fastq_file_1}.sorted"
run rm "${fastq_file_2}.one_record_per_line"
run rm "${fastq_file_2}.sorted"
