#!/usr/bin/env bash

set -euxo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly build_dir="${git_root_dir}/cmake-build-debug-all"
if [[ ! -d "${build_dir}" ]]; then
    mkdir -p "${build_dir}"
fi

cd "${build_dir}"
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_COVERAGE=OFF \
    -DBUILD_DOCUMENTATION=OFF \
    -DBUILD_TESTS=ON
make
make doc
