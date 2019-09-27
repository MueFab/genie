#!/usr/bin/env bash

set -euxo pipefail

self="${0}"
self_name="${self##*/}"

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly build_dir="${git_root_dir}/cmake-build-debug-all"
readonly gabac_tests_app="${build_dir}/bin/gabac-tests"
if [[ ! -x "${gabac_tests_app}" ]]; then
    echo "[${self_name}] error: gabac-tests application does not exist: ${gabac_tests_app}"
    exit 1
fi

"${gabac_tests_app}"
