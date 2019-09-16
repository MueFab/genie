#!/usr/bin/env bash

set -euxo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly build_dir="${git_root_dir}/cmake-build-release"
readonly gabac_app="${build_dir}/bin/gabac-app"
if [[ ! -x "${gabac_app}" ]]; then
    echo "error: gabac-app application does not exist: ${gabac_app}"
    exit 1
fi

readonly tmp_dir="${git_root_dir}/tmp"
if [[ -d "${tmp_dir}" ]]; then
    echo "error: tmp directory exists already: ${tmp_dir}"
    exit 1
fi
mkdir -p "${tmp_dir}"

input_files=()
configuration_files=()
bytestream_files=()
decompressed_files=()

input_files+=("${git_root_dir}/resources/gabac-test-files/0x041f5aac")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/equality-coding.json")
bytestream_files+=("${tmp_dir}/0x041f5aac.gabac-bytestream")
decompressed_files+=("${tmp_dir}/0x041f5aac.gabac-decompressed")

input_files+=("${git_root_dir}/resources/gabac-test-files/all-values-bytes")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/equality-coding.json")
bytestream_files+=("${tmp_dir}/all-values-bytes.gabac-bytestream")
decompressed_files+=("${tmp_dir}/all-values-bytes.gabac-decompressed")

input_files+=("${git_root_dir}/resources/gabac-test-files/one-mebibyte-random")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/rle-coding.json")
bytestream_files+=("${tmp_dir}/one-mebibyte-random.gabac-bytestream")
decompressed_files+=("${tmp_dir}/one-mebibyte-random.gabac-decompressed")

input_files+=("${git_root_dir}/resources/gabac-test-files/one-million-zero-bytes")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/equality-coding.json")
bytestream_files+=("${tmp_dir}/one-million-zero-bytes.gabac-bytestream")
decompressed_files+=("${tmp_dir}/one-million-zero-bytes.gabac-decompressed")

input_files+=("${git_root_dir}/resources/gabac-test-files/one-million-zero-bytes")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/match-coding.json")
bytestream_files+=("${tmp_dir}/one-million-zero-bytes.gabac-bytestream")
decompressed_files+=("${tmp_dir}/one-million-zero-bytes.gabac-decompressed")

input_files+=("${git_root_dir}/resources/gabac-test-files/void")
configuration_files+=("${git_root_dir}/resources/gabac-example-configs/match-coding.json")
bytestream_files+=("${tmp_dir}/void.gabac-bytestream")
decompressed_files+=("${tmp_dir}/void.gabac-decompressed")

for i in "${!input_files[@]}"; do
    input_file=${input_files[${i}]}
    configuration_file=${configuration_files[${i}]}
    bytestream_file=${bytestream_files[${i}]}
    decompressed_file=${decompressed_files[${i}]}

    "${gabac_app}" encode \
        --log_level error \
        --input_file "${input_file}" \
        --configuration_file "${configuration_file}" \
        --output_file "${bytestream_file}"

    "${gabac_app}" decode \
        --log_level error \
        --input_file "${bytestream_file}" \
        --configuration_file "${configuration_file}" \
        --output_file "${decompressed_file}"

    diff "${input_file}" "${decompressed_file}"

    rm "${bytestream_file}"
    rm "${decompressed_file}"
done

rm -rf "${tmp_dir}"
