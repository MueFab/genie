#!/usr/bin/env bash

# Extract a chromosome from a SAM or BAM file

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
# Required programs
# -----------------------------------------------------------------------------

readonly samtools="/path/to/samtools"
if [[ ! -x "${samtools}" ]]; then
    error "'${samtools}' is not executable"
    exit 1
fi

# -----------------------------------------------------------------------------
# Command line
# -----------------------------------------------------------------------------

if [[ "${#}" != 2 ]]; then
    info "Usage: ${self_name} <file.[sam|bam]> <chromosome>"
    exit 1
fi
readonly input_sambam="${1}"
readonly output_prefix=${1%.*} # strip .sam/.bam
readonly chromosome="${2}"
if [[ ! -f "${input_sambam}" ]]; then
    error "'${input_sambam}' is not a regular file"
    exit 1
fi

# -----------------------------------------------------------------------------
# Do it
# -----------------------------------------------------------------------------

# Convert input file from SAM to BAM, if necessary
if [[ "${1}" == *.sam ]]; then
    info "SAM-to-BAM conversion: ${input_sambam} -> ${output_prefix}.bam"
    if [[ -f "${output_prefix}.bam" ]]; then
        info "BAM file '${output_prefix}.bam' already exists (not reproducing it)"
    else
        run "${samtools}" view -bh "${1}" 1>"${output_prefix}.bam"
    fi
fi

# Construct BAM index file, if necessary
info "Constructing BAM index file: ${output_prefix}.bai"
if [[ -f "${output_prefix}.bai" ]]; then
    info "BAM index file '${output_prefix}.bai' already exists (not reproducing it)"
else
    run "${samtools}" index "${output_prefix}.bam" "${output_prefix}.bai"
fi

# Extract the chromosome
info "Extracting chromosome: ${chromosome}"
if [[ -f "${output_prefix}.${chromosome}.sam" ]]; then
    info "SAM file '${output_prefix}.${chromosome}.sam' already exists (not reproducing it)"
else
    run "${samtools}" view -h "${output_prefix}.bam" "${chromosome}" 1>"${output_prefix}.${chromosome}.sam"
fi
