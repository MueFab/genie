#!/usr/bin/env bash

if [[ ! -z "${CI}" ]]; then set -x; fi
set -euo pipefail

self="${0}"
self_name="${self##*/}"

git rev-parse --git-dir 1>/dev/null # Exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly build_dir="${git_root_dir}/cmake-build-coverage"

# Build
mkdir -p "${build_dir}"
cd "${build_dir}"
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_COVERAGE=ON \
    -DBUILD_TESTS=ON
make --jobs
cd "${git_root_dir}"

# Run tests
"${build_dir}/bin/gabac-tests"
# "${build_dir}/bin/util-tests"

# Capture coverage info
readonly coverage_info="coverage.info"
lcov --directory . --capture --output-file "${coverage_info}"

# Filter out Linux system files
lcov \
    --remove "${coverage_info}" \
    --output-file "${coverage_info}" \
    '/usr/include/*' \
    '/usr/lib/*'

# Filter out macOS system files
lcov \
    --remove "${coverage_info}" \
    --output-file "${coverage_info}" \
    '/Applications/*'

# Filter out own build and unit test files
lcov \
    --remove "${coverage_info}" \
    --output-file "${coverage_info}" \
    '*/genie/cmake-build-*/*' \
    '*/genie/test/*'

# Output coverage data on the console (optional)
lcov --list "${coverage_info}"

# Generate HTML report
readonly coverage_dir="${build_dir}/coverage/html"
genhtml "${coverage_info}" --output-directory "${coverage_dir}"
echo "[${self_name}] coverage report: ${coverage_dir}/index.html";

# Upload coverage report to codevio.io (if run on Travis)
set +u # In the following lines do *not* treat unset variables as an error
# shellcheck disable=SC2236
if [[ ! -z "${CI}" ]]; then
    bash <(curl -s https://codecov.io/bash) -f "${coverage_info}" -t "${CODECOV_TOKEN}"
fi
set -u # Treat unset variables as error again
