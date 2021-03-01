#!/usr/bin/env bash

if [[ ! -z "${CI}" ]]; then set -x; fi
set -euo pipefail

self="${0}"
self_name="${self##*/}"

git rev-parse --git-dir 1>/dev/null # Exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly coverage_dir="${git_root_dir}/cmake-build-coverage/coverage"
readonly documentation_dir="${git_root_dir}/cmake-build-documentation/documentation"
readonly page_input_dir="${git_root_dir}/data/misc/page"
readonly page_output_dir="${git_root_dir}/page-output"

cp --recursive "${page_input_dir}" "${page_output_dir}"
cp --recursive "${coverage_dir}" "${page_output_dir}"
cp --recursive "${documentation_dir}" "${page_output_dir}"

echo "[${self_name}] page output: ${page_output_dir}/index.html";
