#!/usr/bin/env bash

set -euo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

dirs=()
dirs+=("${git_root_dir}/source")
dirs+=("${git_root_dir}/tests")

extensions=()
extensions+=("*.h")
extensions+=("*.hpp")
extensions+=("*.c")
extensions+=("*.cc")
extensions+=("*.cpp")

files=()

for i in "${!dirs[@]}"; do
    dir=${dirs[${i}]}
    for j in "${!extensions[@]}"; do
        extension=${extensions[${j}]}
        while IFS='' read -r line; do
            files+=("${line}");
        done < <(find "${dir}" -name "${extension}")
    done
done

for i in "${!files[@]}"; do
    file=${files[${i}]}
    if [[ ! ${file} =~ "third-party" ]]; then
        echo "processing: ${file}"
        clang-format -i "${file}"
    else
        echo "skipping: ${file}"
    fi
done
