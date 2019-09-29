#!/usr/bin/env bash

set -euxo pipefail

self="${0}"
self_name="${self##*/}"

git rev-parse --git-dir 1>/dev/null # Exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly build_dir="${git_root_dir}/cmake-build-debug-all"
if [[ ! -d "${build_dir}" ]]; then
    echo "[${self_name}] error: build directory does not exist: ${build_dir}"
    exit 1
fi

# Check whether the documentation and the coverage report were generated
readonly doc_dir="${build_dir}/doc"
if [[ ! -d "${doc_dir}" ]]; then
    echo "[${self_name}] error: documentation directory does not exist: ${doc_dir}"
    exit 1
fi
readonly codecov_dir="${build_dir}/codecov"
if [[ ! -d "${codecov_dir}" ]]; then
    echo "[${self_name}] error: coverage report directory does not exist: ${codecov_dir}"
    exit 1
fi

# Compose the pages directory
readonly pages_dir="${git_root_dir}/pages"
if [[ -d "${pages_dir}" ]]; then
    echo "[${self_name}] error: pages directory exists already: ${pages_dir}"
    exit 1
fi
mkdir -p "${pages_dir}"
cp --recursive "${doc_dir}" "${pages_dir}"
cp --recursive "${codecov_dir}" "${pages_dir}"
cp "${git_root_dir}/data/misc/index.html" "${pages_dir}"
