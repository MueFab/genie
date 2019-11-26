#!/usr/bin/env bash

set -euo pipefail

self="${0}"
self_name="${self##*/}"

# Check whether all required commands are available
cmds=()
cmds+=("git")
cmds+=("rm")
#
for i in "${!cmds[@]}"; do
    cmd=${cmds[${i}]}
    if not command -v "${cmd}" &>/dev/null; then
        echo "[${self_name}] error: command does not exist: ${cmd}"
        exit 1
    fi
done

# Get Git root directory
git rev-parse --git-dir 1>/dev/null # Exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

# Get the Genie encoder
readonly build_dir="${git_root_dir}/cmake-build-release"
readonly spring_encoder="${build_dir}/bin/genie"
if [[ ! -x "${spring_encoder}" ]]; then
    echo "[${self_name}] error: spring_encoder application does not exist: ${spring_encoder}"
    exit 1
fi

readonly yaml2fastqscript="${git_root_dir}/src/libs/spring/yaml2fastq.py"
if [[ ! -f "${yaml2fastqscript}" ]]; then
    echo "[${self_name}] error: yaml2fastqscript application does not exist: ${yaml2fastqscript}"
    exit 1
fi

# Get the MPEG-G reference software decoder
# Adjust this path to point to your mpegg-decoder-p2 executable!
#        vvvvvvvvvvvvvvvv
readonly mpegg_bin_path="../mpegg-reference-sw/build-debug/bin"

readonly mpegg_decoder_p2="${mpegg_bin_path}/decoder/mpegg-decoder-p2"
if [[ ! -x "${mpegg_decoder_p2}" ]]; then
    echo "[${self_name}] error: mpegg-decoder-p2 application does not exist: ${mpegg_decoder_p2}"
    exit 1
fi

readonly mpegg_record_to_yaml="${mpegg_bin_path}/tools/mpegg-record-to-yaml"
if [[ ! -x "${mpegg_record_to_yaml}" ]]; then
    echo "[${self_name}] error: mpegg-record-to-yaml application does not exist: ${mpegg_record_to_yaml}"
    exit 1
fi

echo "[${self_name}] testing single end global assembly"

# Test data (single end)
readonly fastq_file="${git_root_dir}/data/fastq/fourteen-records.fastq"
readonly bitstream_file="${fastq_file}.bitstream"
readonly decoded_file="${bitstream_file}.decoded"
readonly decoded_file_yaml="${bitstream_file}.decoded.yaml"
readonly decoded_file_fastq="${bitstream_file}.decoded.fastq"
readonly expected_file_fastq="${bitstream_file}.new.fastq"

echo "[${self_name}] running spring-encoder"

"${spring_encoder}" \
    "${fastq_file}" \
    --output-file-path "${bitstream_file}"

echo "[${self_name}] running mpegg-decoder-p2"

"${mpegg_decoder_p2}" \
    --verbose debug \
    --bitstream "${bitstream_file}" \
    --output "${decoded_file}"

echo "[${self_name}] running mpegg-record-to-yaml"

"${mpegg_record_to_yaml}" \
    -i "${decoded_file}" \
    -o "${decoded_file_yaml}"

echo "[${self_name}] running yaml2fastq"
python3 \
    "${yaml2fastqscript}" \
    "${decoded_file_yaml}" \
    "${decoded_file_fastq}"

echo "[${self_name}] comparing decompressed file with expected result."
cmp "${decoded_file_fastq}" "${expected_file_fastq}"

echo "[${self_name}] success"
echo "[${self_name}] used files:"
echo "[${self_name}]     fastq file:            ${fastq_file}"
echo "[${self_name}]     bitstream file:        ${bitstream_file}"
echo "[${self_name}]     decoded file:          ${decoded_file}"
echo "[${self_name}]     decoded file (yaml):   ${decoded_file_yaml}"
echo "[${self_name}]     decoded file (fastq):  ${decoded_file_fastq}"

echo ""
echo "[${self_name}] testing paired end global assembly"

# Test data (paired end)
readonly fastq_file_paired="${git_root_dir}/data/fastq/fourteen-records-pair.fastq"
readonly bitstream_file_paired="${fastq_file}.paired.bitstream"
readonly decoded_file_paired="${bitstream_file_paired}.decoded"
readonly decoded_file_paired_yaml="${bitstream_file_paired}.decoded.yaml"
readonly decoded_file_fastq_1="${bitstream_file_paired}.decoded_1.fastq"
readonly decoded_file_fastq_2="${bitstream_file_paired}.decoded_2.fastq"
readonly expected_file_fastq_1="${bitstream_file_paired}.new_1.fastq"
readonly expected_file_fastq_2="${bitstream_file_paired}.new_2.fastq"

echo "[${self_name}] running spring-encoder"

"${spring_encoder}" \
    "${fastq_file}" \
    "${fastq_file_paired}" \
    --output-file-path "${bitstream_file_paired}"

echo "[${self_name}] running mpegg-decoder-p2"

"${mpegg_decoder_p2}" \
    --verbose debug \
    --bitstream "${bitstream_file_paired}" \
    --output "${decoded_file_paired}"

echo "[${self_name}] running mpegg-record-to-yaml"

"${mpegg_record_to_yaml}" \
    -i "${decoded_file_paired}" \
    -o "${decoded_file_paired_yaml}"

echo "[${self_name}] running yaml2fastq"
python3 \
    "${yaml2fastqscript}" \
    "${decoded_file_paired_yaml}" \
    "${decoded_file_fastq_1}" \
    "${decoded_file_fastq_2}"

echo "[${self_name}] comparing decompressed file with expected result."
cmp "${decoded_file_fastq_1}" "${expected_file_fastq_1}"
cmp "${decoded_file_fastq_2}" "${expected_file_fastq_2}"

echo "[${self_name}] success"
echo "[${self_name}] used files:"
echo "[${self_name}]     paired fastq file:     ${fastq_file_paired}"
echo "[${self_name}]     bitstream file:        ${bitstream_file_paired}"
echo "[${self_name}]     decoded file:          ${decoded_file_paired}"
echo "[${self_name}]     decoded file (yaml):   ${decoded_file_paired_yaml}"
echo "[${self_name}]     decoded file (fastq 1):${decoded_file_fastq_1}"
echo "[${self_name}]     decoded file (fastq 2):${decoded_file_fastq_2}"
