#!/usr/bin/env bash

set -euxo pipefail

git rev-parse --git-dir 1>/dev/null # Exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly build_dir="${git_root_dir}/cmake-build-release"
if [[ ! -d "${build_dir}" ]]; then
    mkdir -p "${build_dir}"
fi

cd "${build_dir}"
cmake .. -DCMAKE_BUILD_TYPE=Release
make --jobs
