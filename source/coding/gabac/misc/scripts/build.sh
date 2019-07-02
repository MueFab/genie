#!/usr/bin/env bash

# Get the Git root directory
readonly git_root_dir="$(git rev-parse --show-toplevel)"

# Check if the 'build' directory exists - if not, create it
readonly build_dir="$git_root_dir/build"
if [ ! -d "$build_dir" ]; then
    printf "Creating build directory: %s\\n" "$build_dir"
    mkdir -p "$build_dir"
fi

# Build everything
cd "$build_dir" || exit 1
cmake .. \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DGABAC_BUILD_SHARED_LIB=ON || exit 1
make || exit 1
#make tests || exit -1
make gabacify || exit 1
