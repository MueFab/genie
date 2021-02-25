#!/bin/bash
# Do a full test of genie
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
    exit 1
fi

if ! command -v cmake &> /dev/null
then
    echo "Cmake could not be found. Please install cmake (debian: sudo apt-get install cmake)."
    exit 1
fi

if ! command -v make &> /dev/null
then
    echo "Make could not be found. Please install make."
    exit 1
fi

############ Run ###############

git_root_dir="$(git rev-parse --show-toplevel)"

cd $git_root_dir/ci
./src_test.sh || { echo 'Src test failed!' ; exit 1; }
./build.sh debug || { echo 'Build debug failed!' ; exit 1; }
./unit_test.sh || { echo 'Unit tests failed!' ; exit 1; }
./memory_test.sh || { echo 'Memory tests failed!' ; exit 1; }
./roundtrip_test.sh || { echo 'Roundtrip tests failed!' ; exit 1; }
./conformance_test.sh || { echo 'Conformance tests failed!' ; exit 1; }
echo "Genie works as expected!"
