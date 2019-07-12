#!/usr/bin/env bash

set -euxo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly tmp_dir="${git_root_dir}/tmp"
if [[ -d "${tmp_dir}" ]]; then exit 1; fi
mkdir -p "${tmp_dir}"

readonly gabac_app="${git_root_dir}/cmake-build/bin/gabac-app"
if [[ ! -x "${gabac_app}" ]]; then exit 1; fi

input_files=()
configuration_files=()
bytestream_files=()
uncompressed_files=()

input_files+=("${git_root_dir}/resources/gabac-test-files/one-million-zero-bytes")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/equality-coding.json")
bytestream_files+=("${git_root_dir}/tmp/one-million-zero-bytes.gabac-bytestream")
uncompressed_files+=("${git_root_dir}/tmp/one-million-zero-bytes.gabac-uncompressed")

input_files+=("${git_root_dir}/resources/gabac-test-files/one-million-zero-bytes")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/match-coding.json")
bytestream_files+=("${git_root_dir}/tmp/one-million-zero-bytes.gabac-bytestream")
uncompressed_files+=("${git_root_dir}/tmp/one-million-zero-bytes.gabac-uncompressed")

input_files+=("${git_root_dir}/resources/gabac-test-files/one-mebibyte-random")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/rle-coding.json")
bytestream_files+=("${git_root_dir}/tmp/one-mebibyte-random.gabac-bytestream")
uncompressed_files+=("${git_root_dir}/tmp/one-mebibyte-random.gabac-uncompressed")

for i in "${!input_files[@]}"; do
    input_file=${input_files[$i]}
    configuration_file=${configuration_files[$i]}
    bytestream_file=${bytestream_files[$i]}
    uncompressed_file=${uncompressed_files[$i]}

    "${gabac_app}" encode \
        -l error \
        -i "${input_file}" \
        -c "${configuration_file}" \
        -o "${bytestream_file}"

    "${gabac_app}" decode \
        -l error \
        -i "${bytestream_file}" \
        -c "${configuration_file}" \
        -o "${uncompressed_file}"

    diff "${input_file}" "${uncompressed_file}"

    rm "${bytestream_file}" "${uncompressed_file}"
done

rm -rf "${tmp_dir}"
