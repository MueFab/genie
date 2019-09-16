#!/usr/bin/env bash

set -euo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

dirs=()
dirs+=("${git_root_dir}/source")
dirs+=("${git_root_dir}/tests")

patterns=()
patterns+=("FIXME")
patterns+=("TODO")

for i in "${!dirs[@]}"; do
    dir=${dirs[${i}]}
    for j in "${!patterns[@]}"; do
        pattern=${patterns[${j}]}
        echo "${pattern}s in tree: ${dir}"

        grep --color --line-number --recursive "${pattern}" "${dir}"
    done
done
