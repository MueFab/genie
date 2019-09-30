#!/usr/bin/env bash

set -euo pipefail

self="${0}"
self_name="${self##*/}"

# Check whether all required commands are available
cmds=()
cmds+=("cd")
cmds+=("git")
cmds+=("rm")
#
for i in "${!cmds[@]}"; do
    cmd=${cmds[${i}]}
    if not command -v "${cmd}" &>/dev/null; then
        echo "[${self_name}] error: command does not exist: ${cmd}"
        exit 1
    fi
done

# Get Git root directory
git rev-parse --git-dir 1>/dev/null # Exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

# Remove all build directories
cd "${git_root_dir}"
rm --recursive cmake-build-* 2>/dev/null || echo "[${self_name}] nothing to clean"
