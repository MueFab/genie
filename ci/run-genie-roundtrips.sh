#!/usr/bin/env bash

set -euxo pipefail

self="${0}"
self_name="${self##*/}"

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly build_dir="${git_root_dir}/cmake-build-release"
readonly genie="${build_dir}/bin/genie"
if [[ ! -x "${genie}" ]]; then
    echo "[${self_name}] error: genie application does not exist: ${genie}"
    exit 1
fi

readonly tmp_dir="${git_root_dir}/tmp"
if [[ -d "${tmp_dir}" ]]; then
    echo "[${self_name}] error: tmp directory exists already: ${tmp_dir}"
    exit 1
fi
mkdir -p "${tmp_dir}"

input_files=()
config_file_paths=()
compressed_files=()
decompressed_files=()

input_files+=("${git_root_dir}/resources/test-files/fastq/fourteen-gattaca-records.fastq")
config_file_paths+=("${git_root_dir}/resources/gabac-configs/")
compressed_files+=("${tmp_dir}/fourteen-gattaca-records.genie")
decompressed_files+=("${tmp_dir}/fourteen-gattaca-records.genie-decompressed.fastq")

input_files+=("${git_root_dir}/resources/test-files/fastq/simplest.fastq")
config_file_paths+=("${git_root_dir}/resources/gabac-configs/")
compressed_files+=("${tmp_dir}/simplest.genie")
decompressed_files+=("${tmp_dir}/simplest.genie-decompressed.fastq")

for i in "${!input_files[@]}"; do
    input_file=${input_files[${i}]}
    config_file_path=${config_file_paths[${i}]}
    compressed_file=${compressed_files[${i}]}
    decompressed_file=${decompressed_files[${i}]}

    "${genie}" "${input_file}" \
        --config-file-path "${config_file_path}" \
        --output-file-path "${compressed_file}"

    "${genie}" "${compressed_file}" \
        --config-file-path "${config_file_path}" \
        --output-file-path "${decompressed_file}"

    "${git_root_dir}/util/cmp-reordered-fastq.sh" \
        -1 "${input_file}" \
        -2 "${decompressed_file}"

    rm "${compressed_file}"
    rm "${decompressed_file}"
    rm "${git_root_dir}/genie.log"
done

rm -rf "${tmp_dir}"
