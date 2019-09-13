#!/usr/bin/env bash

set -euxo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly genie="${git_root_dir}/cmake-build-release/bin/genie"
if [[ ! -x "${genie}" ]]; then exit 1; fi

"${genie}" \
    "${git_root_dir}/resources/test-files/fastq/fourteen-gattaca-records.fastq" \
    --config-file-path "${git_root_dir}/resources/gabac-configs/" \
    --output-file-path "${git_root_dir}/resources/test-files/fastq/fourteen-gattaca-records.genie"

"${genie}" \
    "${git_root_dir}/resources/test-files/fastq/fourteen-gattaca-records.genie" \
    --config-file-path "${git_root_dir}/resources/gabac-configs/" \
    --output-file-path "${git_root_dir}/resources/test-files/fastq/fourteen-gattaca-records.genie-decompressed.fastq"

"${git_root_dir}/scripts/cmp-reordered-fastq.sh" \
    -1 "${git_root_dir}/resources/test-files/fastq/fourteen-gattaca-records.fastq" \
    -2 "${git_root_dir}/resources/test-files/fastq/fourteen-gattaca-records.genie-decompressed.fastq"

rm "${git_root_dir}/resources/test-files/fastq/fourteen-gattaca-records.genie"
rm "${git_root_dir}/resources/test-files/fastq/fourteen-gattaca-records.genie-decompressed.fastq"
rm "${git_root_dir}/genie.log"
