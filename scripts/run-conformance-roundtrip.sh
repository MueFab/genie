#!/usr/bin/env bash

set -euo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly ureads_encoder="${git_root_dir}/cmake-build/bin/ureads-encoder"
readonly mpegg_decoder_p2="/home/voges/repos/mpegg-reference-sw/build-debug/bin/decoder/mpegg-decoder-p2"

readonly fastq="${git_root_dir}/resources/test-files/fastq/simplest.fastq"
readonly bitstream="${fastq}.bitstream"
readonly decoded="${bitstream}.decoded"

"${ureads_encoder}" -i "${fastq}" -o "${bitstream}"
"${mpegg_decoder_p2}" -i "${bitstream}" -o "${decoded}"

rm "${bitstream}"
rm "${decoded}"
