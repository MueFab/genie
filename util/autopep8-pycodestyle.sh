#!/usr/bin/env bash

set -euo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

cmds=()
cmds+=("autopep8")
cmds+=("pycodestyle")

for i in "${!cmds[@]}"; do
    cmd=${cmds[${i}]}
    if not command -v "${cmd}" &>/dev/null; then
        error "command does not exist: ${cmd}"
        exit 1
    fi
done

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
