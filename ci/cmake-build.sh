#!/usr/bin/env bash

set -euxo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly cmake_build_dir="${git_root_dir}/cmake-build"
if [[ ! -d "${cmake_build_dir}" ]]; then
    mkdir -p "${cmake_build_dir}"
fi

cd "${cmake_build_dir}"
cmake ..
# cmake .. -DBUILD_DOCUMENTATION=ON
make
# make doc
