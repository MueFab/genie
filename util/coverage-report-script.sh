#!/usr/bin/env bash

set -euo pipefail

git rev-parse --git-dir 1>/dev/null # exit if not inside Git repo

GIT_REPO_PATH=$(git rev-parse --show-toplevel)
cd $GIT_REPO_PATH

# Create lcov report
  # capture coverage info
lcov --directory . --capture --output-file coverage.info
  # filter out system and extra files.
  # To also not include test code in coverage add them with full path to the patterns: '*/tests/*'
lcov --remove coverage.info '*/cmake-build-debug/*' '/usr/*' --output-file coverage.info
  # output coverage data for debugging (optional)
lcov --list coverage.info
  #Generate HTML output:
genhtml coverage.info --output-directory out
 # Uploading to CodeCov with TOKEN
#export CODECOV_TOKEN="7d8ec23c-382b-4b43-a528-210eeb893776"
  # '-f' specifies file(s) to use and disables manual coverage gathering and file search which has already been done above
#bash <(curl -s https://codecov.io/bash) -f coverage.info || echo "Codecov did not collect coverage reports"