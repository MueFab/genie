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

# Check wether any .gcda files are present
num_gcda_files=$(find "${build_dir}" -type f -name '*.gcda' | wc -l)
num_gcda_files="${num_gcda_files//[[:space:]]/}"
if [[ "${num_gcda_files}" == 0 ]]; then
    echo "[${self_name}] error: no .gcda files found in build directory: ${build_dir}"
    exit 1
fi

cd "${git_root_dir}"

# Capture coverage info
lcov --directory . --capture --output-file coverage.info

# Filter out Linux system files
lcov \
    --remove coverage.info \
    --output-file coverage.info \
    '/usr/include/*' \
    '/usr/lib/*'

# Filter out macOS system files
lcov \
    --remove coverage.info \
    --output-file coverage.info \
    '/Applications/*'

# Filter out own build and unit test files
lcov \
    --remove coverage.info \
    --output-file coverage.info \
    '*/genie/cmake-build-*/*' \
    '*/genie/test/*'

# Output coverage data on the console (optional)
lcov --list coverage.info

# Upload report to codevio.io (if run on Travis) or generate local HTML report
set +u # in the following lines do *not* treat unset variables as an error
# shellcheck disable=SC2236
if [[ ! -z "${CI}" ]]; then
    bash <(curl -s https://codecov.io/bash) -f coverage.info -t "${CODECOV_TOKEN}"
else
    readonly local_codecov_dir="${build_dir}/codecov/html"
    genhtml coverage.info --output-directory "${local_codecov_dir}"
    echo "[${self_name}] coverage report generated locally at: ${local_codecov_dir}";

fi
set -u # treat unset variables as error again
