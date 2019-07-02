#!/usr/bin/env bash

# Get the Git root directory
readonly git_root_dir="$(git rev-parse --show-toplevel)"

# Get the correct shared library extension, depending whether we are on
# Linux (*.so) or macOS (*.dylib)
shared_lib_extension=""
if [ "${TRAVIS}" = "true" ]; then
    if [ "${TRAVIS_OS_NAME}" = "linux" ]; then
        shared_lib_extension="so"
    elif [ "${TRAVIS_OS_NAME}" = "osx" ]; then
        shared_lib_extension="dylib"
    else
        echo "Not a Linux or macOS build; skipping Python unit tests"
        exit 0
    fi
else
    if [[ $(uname -s) == Linux* ]]; then
        shared_lib_extension="so"
    elif [[ $(uname -s) == Darwin* ]]; then
        shared_lib_extension="dylib"
    else
        echo "Not a Linux or macOS build; skipping Python unit tests"
        exit 0
    fi
fi

# Go to the folder of gabac.py and fire up the unit tests
cd "$git_root_dir/source/python_api/" || exit -1
LIBGABAC_PATH="$git_root_dir/build/libgabac.$shared_lib_extension" python -m unittest discover -v "$git_root_dir/tests/python_api/"
