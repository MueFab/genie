#!/usr/bin/env bash

set -euo pipefail

self="${0}"
self_name="${self##*/}"

# Check whether all required commands are available
cmds=()
cmds+=("autopep8")
cmds+=("find")
cmds+=("git")
cmds+=("pycodestyle")
cmds+=("read")
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

# Get all relevant files
dirs=()
dirs+=("${git_root_dir}/ci")
dirs+=("${git_root_dir}/src")
dirs+=("${git_root_dir}/test")
dirs+=("${git_root_dir}/util")
#
extensions=()
extensions+=("*.py")
#
files=()
#
for i in "${!dirs[@]}"; do
    dir=${dirs[${i}]}
    for j in "${!extensions[@]}"; do
        extension=${extensions[${j}]}
        while IFS='' read -r line; do
            files+=("${line}");
        done < <(find "${dir}" -name "${extension}")
    done
done

# Apply autopep8 and pycodestyle
for i in "${!files[@]}"; do
    file=${files[${i}]}
    echo "[${self_name}] running autopep8 on: ${file}"
    # autopep8 --in-place --max-line-length=120 "${file}"
    echo "[${self_name}] running pycodestyle on: ${file}"
    # pycodestyle --max-line-length=120 "${file}"
done
