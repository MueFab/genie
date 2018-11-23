#!/usr/bin/env bash

# Get the Git root directory
readonly git_root_dir="$(git rev-parse --show-toplevel)"

# Check if the 'external_tools' directory exists - if not, create it
readonly external_tools_dir="$git_root_dir/external_tools"
if [ ! -d "$external_tools_dir" ]; then
    printf "Creating external_tools directory: %s\\n" "$external_tools_dir"
    mkdir -p "$external_tools_dir"
fi

# External tools URLs
readonly cppcheck_url="https://github.com/danmar/cppcheck/archive/1.85.zip"
readonly cppcheck_zip="$external_tools_dir/1.85.zip"
readonly cppcheck_dir="$external_tools_dir/cppcheck-1.85"
#
readonly cpplint_url="https://github.com/cpplint/cpplint/archive/1.3.0.zip"
readonly cpplint_zip="$external_tools_dir/1.3.0.zip"
readonly cpplint_dir="$external_tools_dir/cpplint-1.3.0"
#
readonly pycodestyle_url="https://github.com/PyCQA/pycodestyle/archive/2.4.0.zip"
readonly pycodestyle_zip="$external_tools_dir/2.4.0.zip"
readonly pycodestyle_dir="$external_tools_dir/pycodestyle-2.4.0"

# Download and build cppcheck if it does not exist already
if [ ! -d "$cppcheck_dir" ]; then
    printf "Downloading cppcheck\\n"
    cd "$external_tools_dir" || exit -1
    wget "$cppcheck_url"
    unzip "$cppcheck_zip"
    rm "$cppcheck_zip"
    printf "Building cppcheck\\n"
    cd "$cppcheck_dir" || exit -1
    mkdir -p build
    cd build || exit -1
    cmake -DCMAKE_INSTALL_PREFIX=. ..
    make install
else
    printf "cppcheck already downloaded and built\\n"
fi

# Download cpplint if it does not exist already
if [ ! -d "$cpplint_dir" ]; then
    printf "Downloading cpplint\\n"
    cd "$external_tools_dir" || exit -1
    wget "$cpplint_url"
    unzip "$cpplint_zip"
    rm "$cpplint_zip"
else
    printf "cpplint already downloaded\\n"
fi

# Download pycodestyle if it does not exist already
if [ ! -d "$pycodestyle_dir" ]; then
    printf "Downloading pycodestyle\\n"
    cd "$external_tools_dir" || exit -1
    wget "$pycodestyle_url"
    unzip "$pycodestyle_zip"
    rm "$pycodestyle_zip"
else
    printf "pycodestyle already downloaded\\n"
fi
