#!/usr/bin/env bash

# Search for matching files in two directories

self="${0}"
self_name="${self##*/}"

datetime=$(date -u +"%Y-%m-%dT%H:%M:%SZ")

trace() { echo "${self_name}: trace: ${*}"; }
debug() { echo "${self_name}@${datetime}: debug: ${*}"; }
info() { echo "${*}"; }
warning() { echo "${self_name}: warning: ${*}"; }
error() { echo "${self_name}: error: ${*}" 1>&2; }
fatal() { echo "${self_name}: fatal: ${*}" 1>&2; }

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

if [[ ${#} != 2 ]]; then
    info "Usage: ${self_name} DIR1 DIR2"
    info "Search for matching files in two directories"
    exit 1
fi

readonly dir1="${1}"
readonly dir2="${2}"

readonly files_in_dir1=$(find "${dir1}" -maxdepth 1 -type f)
readonly files_in_dir2=$(find "${dir2}" -maxdepth 1 -type f)

readonly num_files_in_dir1=$(find "${dir1}" -maxdepth 1 -type f | wc -l)
readonly num_files_in_dir2=$(find "${dir2}" -maxdepth 1 -type f | wc -l)

matching_file_pairs=()
unmatched_files=()
num_processed_files=0
num_supernumerary_matches=0

info "Comparing ${num_files_in_dir1} file(s) from '${dir1}' to ${num_files_in_dir2} file(s) from '${dir2}'"

for f1 in ${files_in_dir1}; do
    found_matching_file_pair=false
    for f2 in ${files_in_dir2}; do
        diff "${f1}" "${f2}" &>/dev/null
        rc="${?}"
        if [[ ${rc} == 0 ]]; then
            if [[ ${found_matching_file_pair} == true ]]; then
                num_supernumerary_matches=$((num_supernumerary_matches + 1))
                warning "Found a supernumerary match for: ${f1}"
                warning "Previous match: ${matching_file_pairs[-1]#*:}"
                warning "Current match: ${f2}"
            fi
            found_matching_file_pair=true
            matching_file_pairs+=("${f1}:${f2}")
        fi
    done
    if [[ ${found_matching_file_pair} == false ]]; then
        unmatched_files+=("${f1}")
        warning "Did not find a matching file in '${dir2}' for '${f1}'"
    fi
    num_processed_files=$((num_processed_files + 1))
    info "Processed ${num_processed_files}/${num_files_in_dir1} file(s)"
done

info "Found ${#matching_file_pairs[@]} matching file pair(s)"
info "Of which ${num_supernumerary_matches} are supernumerary"
for kv in "${matching_file_pairs[@]}"; do
    key=${kv%%:*}
    value=${kv#*:}
    info "  ${key}  <->  ${value}"
done

info "Did not find matches for ${#unmatched_files[@]} file(s)"
for f in "${unmatched_files[@]}"; do
    info "  ${f}"
done
