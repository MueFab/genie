#!/usr/bin/env bash

set -euxo pipefail

self="${0}"
self_name="${self##*/}"

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly build_dir="${git_root_dir}/cmake-build-debug-all"
if [[ ! -d "${build_dir}" ]]; then
    echo "[${self_name}] error: build directory does not exist: ${build_dir}"
    exit 1
fi

apps=()
apps+=("${build_dir}/bin/gabac-tests")
# apps+=("${build_dir}/bin/util-tests")

for i in "${!apps[@]}"; do
    app=${apps[${i}]}
    if [[ ! -x "${app}" ]]; then
        echo "[${self_name}] error: application does not exist: ${app}"
        exit 1
    fi
    "${app}"
done
