#!/usr/bin/env bash

if [[ ! -z "${CI}" ]]; then set -x; fi
set -euo pipefail

self="${0}"
self_name="${self##*/}"

git rev-parse --git-dir 1>/dev/null # Exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly coverage_dir="${git_root_dir}/cmake-build-coverage/coverage"
readonly documentation_dir="${git_root_dir}/cmake-build-documentation/documentation"
readonly page_dir="${git_root_dir}/page"

mkdir --parents "${page_dir}"
cp --recursive "${coverage_dir}" "${page_dir}"
cp --recursive "${documentation_dir}" "${page_dir}"
cp "${git_root_dir}/data/misc/index.html" "${page_dir}"

echo "[${self_name}] page: ${page_dir}/index.html";
