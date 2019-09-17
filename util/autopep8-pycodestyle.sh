#!/usr/bin/env bash

set -euo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

if not command -v autopep8 &>/dev/null; then
    echo "error: command does not exist: autopep8"
    exit 1
fi

if not command -v pycodestyle &>/dev/null; then
    echo "error: command does not exist: pycodestyle"
    exit 1
fi

dirs=()
dirs+=("${git_root_dir}/source")
dirs+=("${git_root_dir}/tests")

extensions=()
extensions+=("*.py")

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
        echo "running autopep8 on: ${file}"
        autopep8 --in-place "${file}"
        echo "running pycodestyle on: ${file}"
        pycodestyle "${file}"
    else
        echo "skipping: ${file}"
    fi
done
