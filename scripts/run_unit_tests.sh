#!/usr/bin/env bash

# Get the Git root directory
readonly git_root_dir="$(git rev-parse --show-toplevel)"

# Run C/C++ unit tests
"$git_root_dir/build/tests"
