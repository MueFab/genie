#!/usr/bin/env bash

set -euo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

dirs=()
dirs+=("${git_root_dir}/ci")
dirs+=("${git_root_dir}/source")
dirs+=("${git_root_dir}/tests")
dirs+=("${git_root_dir}/util")

sh_files=()
for i in "${!dirs[@]}"; do
    dir=${dirs[${i}]}
    while IFS='' read -r line; do
        sh_files+=("${line}");
    done < <(find "${dir}" -name '*.sh')
done

for i in "${!sh_files[@]}"; do
    sh_file=${sh_files[${i}]}
    echo "processing: ${sh_file}"
    shellcheck "${sh_file}"
done
