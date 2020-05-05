#!/usr/bin/env bash

self="$0"
self_name="${self##*/}"
self_rel_dir="${self%/*}"
self_abs_dir="$(pwd)/$self_rel_dir"
BUILDS="${BUILDS:-Debug Release}"

run() {
    local rc
    echo "[${self_name}] $*"
    "$@"
    rc="$?"
    if [ "$rc" != 0 ]; then
        echo "Command failed (RC=$rc): $*" 1>&2
        exit $rc
    fi
}

for build in $BUILDS; do
    run cd "$self_abs_dir"
    build_dir="${self_abs_dir}/build-${build}"
    run /bin/mkdir -p "$build_dir"
    run cd "$build_dir"
    run cmake "$@" -D CMAKE_BUILD_TYPE="${build}" -D GENIE_USE_OPENMP=ON ..
    run make -j8
done
