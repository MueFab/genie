#!/usr/bin/env bash

set -euxo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly genie="${git_root_dir}/cmake-build/bin/genie"
if [[ ! -x "${genie}" ]]; then exit 1; fi

"${genie}" \
    resources/test-files/fastq/fourteen-gattaca-records.fastq \
    --config-file-path resources/gabac-configs/

"${genie}" \
    resources/test-files/fastq/fourteen-gattaca-records.genie \
    --config-file-path resources/gabac-configs/

"${git_root_dir}/scripts/cmp-reordered-fastq.sh" \
    -1 resources/test-files/fastq/fourteen-gattaca-records.fastq \
    -2 resources/test-files/fastq/fourteen-gattaca-records.genie-decompressed.fastq

rm resources/test-files/fastq/fourteen-gattaca-records.genie
rm resources/test-files/fastq/fourteen-gattaca-records.genie-decompressed.fastq
