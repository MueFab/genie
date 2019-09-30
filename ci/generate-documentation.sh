#!/usr/bin/env bash

if [[ ! -z "${CI}" ]]; then set -x; fi
set -euo pipefail

self="${0}"
self_name="${self##*/}"

git rev-parse --git-dir 1>/dev/null # Exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly build_dir="${git_root_dir}/cmake-build-documentation"

mkdir --parents "${build_dir}"
cd "${build_dir}"
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_DOCUMENTATION=ON
make doc

echo "[${self_name}] documentation: ${build_dir}/documentation/html/index.html";
