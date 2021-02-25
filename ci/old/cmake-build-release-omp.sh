#!/usr/bin/env bash

if [[ ! -z "${CI}" ]]; then set -x; fi
set -euo pipefail

git rev-parse --git-dir 1>/dev/null # Exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly build_dir="${git_root_dir}/cmake-build-release-omp"

mkdir -p "${build_dir}"
cd "${build_dir}"
cmake .. -DCMAKE_BUILD_TYPE=Release -DGENIE_USE_OPENMP=ON
make --jobs
