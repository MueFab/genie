#!/usr/bin/env bash

set -euo pipefail

self="${0}"
self_name="${self##*/}"

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

cmds=()
cmds+=("rm")

for i in "${!cmds[@]}"; do
    cmd=${cmds[${i}]}
    if not command -v "${cmd}" &>/dev/null; then
        echo "[${self_name}] error: command does not exist: ${cmd}"
        exit 1
    fi
done

readonly build_dir="${git_root_dir}/cmake-build-release"
readonly ureads_encoder="${build_dir}/bin/ureads-encoder"
if [[ ! -x "${ureads_encoder}" ]]; then
    echo "[${self_name}] error: ureads-encoder application does not exist: ${ureads_encoder}"
    exit 1
fi

# Adjust this path to point to your mpegg-decoder-p2 executable!
#        vvvvvvvvvvvvvvvv
readonly mpegg_decoder_p2="/path/to/mpegg-reference-sw/build-debug/bin/decoder/mpegg-decoder-p2"
if [[ ! -x "${mpegg_decoder_p2}" ]]; then
    echo "[${self_name}] error: mpegg-decoder-p2 application does not exist: ${mpegg_decoder_p2}"
    exit 1
fi

readonly fastq_file="${git_root_dir}/data/fastq/simplest.fastq"
readonly bitstream_file="${fastq_file}.bitstream"
readonly decoded_file="${bitstream_file}.decoded"

"${ureads_encoder}" \
    --input-file "${fastq_file}" \
    --output-file "${bitstream_file}"

"${mpegg_decoder_p2}" \
    --verbose debug \
    --bitstream "${bitstream_file}" \
    --output "${decoded_file}"

rm "${bitstream_file}"
rm "${decoded_file}"
