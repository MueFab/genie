#!/usr/bin/env bash

self="${0}"
self_name="${self##*/}"

datetime=$(date -u +"%Y-%m-%dT%H:%M:%SZ")

trace() { echo "[${self_name}] [${datetime}] [trace] ${*}"; }
debug() { echo "[${self_name}] [${datetime}] [debug] ${*}"; }
info() { echo "[${self_name}] [${datetime}] [info] ${*}"; }
warning() { echo "[${self_name}] [${datetime}] [warning] ${*}"; }
error() { echo "[${self_name}] [${datetime}] [error] ${*}" 1>&2; }
fatal() { echo "[${self_name}] [${datetime}] [fatal] ${*}" 1>&2; }

run() {
    local rc
    info "Running: ${*}"
    "${@}"
    rc="${?}"
    if [[ ${rc} != 0 ]]; then
        fatal "Command failed (RC=${rc}): ${*}"
        exit ${rc}
    fi
}

run git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

# Get the correct shared library extension, depending whether we are on
# Linux (*.so) or macOS (*.dylib)
shared_lib_extension=""
if [[ "${TRAVIS}" = "true" ]]; then
    if [[ "${TRAVIS_OS_NAME}" = "linux" ]]; then
        shared_lib_extension="so"
    elif [[ "${TRAVIS_OS_NAME}" = "osx" ]]; then
        shared_lib_extension="dylib"
    else
        warning "Not a Linux or macOS build; skipping Python unit tests"
        exit 0
    fi
else
    if [[ $(uname -s) == Linux* ]]; then
        shared_lib_extension="so"
    elif [[ $(uname -s) == Darwin* ]]; then
        shared_lib_extension="dylib"
    else
        warning "Not a Linux or macOS build; skipping Python unit tests"
        exit 0
    fi
fi

# Go to the folder of gabac.py and fire up the unit tests
run cd "${git_root_dir}/source/libs/gabac/python-api/"
LIBGABAC_PATH="${git_root_dir}/cmake-build/source/libs/gabac/gabac/libgabac.${shared_lib_extension}" python -m unittest discover -v "${git_root_dir}/tests/libs/gabac/python-api/"
