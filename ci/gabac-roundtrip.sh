#!/bin/bash
git_root_dir="$(git rev-parse --show-toplevel)"
$git_root_dir/cmake-build-release/gabac-app -i $git_root_dir/data/gabac/test-files/genNeg.pos -o genNeg.out -t encode -d 0 -s 1
$git_root_dir/cmake-build-release/gabac-app -i genNeg.out -o genNeg.decoded -t decode -d 0 -s 1
diff --report-identical-files $git_root_dir/data/gabac/test-files/genNeg.pos genNeg.decoded
