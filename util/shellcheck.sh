#!/usr/bin/env bash

set -euo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

dirs=()
dirs+=("${git_root_dir}/ci")
dirs+=("${git_root_dir}/source")
dirs+=("${git_root_dir}/tests")
dirs+=("${git_root_dir}/util")

files=()
for i in "${!dirs[@]}"; do
    dir=${dirs[${i}]}
    while IFS='' read -r line; do
        files+=("${line}");
    done < <(find "${dir}" -name '*.sh')
done

for i in "${!files[@]}"; do
    file=${files[${i}]}
    echo "processing: ${file}"
    shellcheck "${file}"
done
