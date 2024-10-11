#!/bin/bash
# Check if the source code is aligned to good practices and coding guides
set -e

############ Check prerequisites ###############

if ! command -v git &> /dev/null
then
    echo "Git could not be found. Please install git (debian: sudo apt-get install git)."
    exit 1
fi

if ! command -v cpplint &> /dev/null
then
    echo "Cpplint could not be found. Please install cpplint (debian: sudo pip3 install cpplint)."
    exit 1
fi

if ! command -v cppcheck &> /dev/null
then
    echo "Cppcheck could not be found. Please install cppcheck (debian: sudo apt-get install cppcheck)."
#    exit 1
fi

############ Run ###############

git_root_dir="$(git rev-parse --show-toplevel)"
echo "Checking source code:"
echo "Cpplint..."
cpplint --recursive --quiet --linelength=120 --filter=-whitespace/indent_namespace,-runtime/references $git_root_dir/src || { echo 'Cpplint failed!' ; exit 1; }
echo "Cppcheck..."
#cppcheck -q --error-exitcode=1 -DGENIE_USE_OPENMP $git_root_dir/src || { echo 'Cppcheck failed!' ; exit 1; }
echo "Source code is clean!"
