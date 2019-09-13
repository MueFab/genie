#!/usr/bin/env bash

set -euo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo

echo running apply-clang-format.sh...

GIT_REPO_PATH=$(git rev-parse --show-toplevel)
cd $GIT_REPO_PATH

echo checking ./source...
find ./source -iname *.h -o -iname *.c -o -iname *.cc | xargs clang-format -style=file -i -fallback-style=none

echo checking ./tests...
find ./tests -iname *.h -o -iname *.c -o -iname *.cc | xargs clang-format -style=file -i -fallback-style=none

echo Done!