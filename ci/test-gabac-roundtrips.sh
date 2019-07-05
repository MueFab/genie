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

# We plan to use the 'tmp' directory for our tests - check if it already
# exists; otherwise create it and proceed
readonly tmp_dir="${git_root_dir}/tmp"
if [[ -d "${tmp_dir}" ]]; then
    fatal "Directory '${tmp_dir}' already exists"
    exit 1
fi
info "Creating directory for GABAC roundtrips: ${tmp_dir}"
run mkdir -p "${tmp_dir}"

readonly gabac_app="${git_root_dir}/cmake-build/bin/gabac-app"
if [[ ! -x "${gabac_app}" ]]; then
    fatal "GABAC application '${gabac_app}' is not executable"
    exit 1
fi

# Gather the test files
input_files=()
configuration_files=()
bytestream_files=()
uncompressed_files=()
#
input_files+=("${git_root_dir}/resources/gabac-test-files/one-million-zero-bytes")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/equality-coding.json")
bytestream_files+=("${git_root_dir}/tmp/one-million-zero-bytes.gabac-bytestream")
uncompressed_files+=("${git_root_dir}/tmp/one-million-zero-bytes.gabac-uncompressed")
#
input_files+=("${git_root_dir}/resources/gabac-test-files/one-million-zero-bytes")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/match-coding.json")
bytestream_files+=("${git_root_dir}/tmp/one-million-zero-bytes.gabac-bytestream")
uncompressed_files+=("${git_root_dir}/tmp/one-million-zero-bytes.gabac-uncompressed")
#
input_files+=("${git_root_dir}/resources/gabac-test-files/one-mebibyte-random")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/rle-coding.json")
bytestream_files+=("${git_root_dir}/tmp/one-mebibyte-random.gabac-bytestream")
uncompressed_files+=("${git_root_dir}/tmp/one-mebibyte-random.gabac-uncompressed")

# Do the test roundtrips
for i in "${!input_files[@]}"; do
    info "Running roundtrip ${i}"

    # Gather the i-th files
    input_file=${input_files[$i]}
    configuration_file=${configuration_files[$i]}
    bytestream_file=${bytestream_files[$i]}
    uncompressed_file=${uncompressed_files[$i]}

    # Encode
    run "${gabac_app}" encode \
        -l error \
        -i "${input_file}" \
        -c "${configuration_file}" \
        -o "${bytestream_file}"

    # Decode
    run "${gabac_app}" decode \
        -l error \
        -i "${bytestream_file}" \
        -c "${configuration_file}" \
        -o "${uncompressed_file}"

    # Check
    run diff "${input_file}" "${uncompressed_file}"

    info "Success"

    rm "${bytestream_file}" "${uncompressed_file}"
done

# Delete the 'tmp' directory
rm -rf "${tmp_dir}"
