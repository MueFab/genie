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
    run /bin/rm -fr "$self_rel_dir/bin/$build"
    run /bin/rm -fR "$self_rel_dir/build-$build"
done

#run /bin/rm -fr "$self_rel_dir/Testing"
