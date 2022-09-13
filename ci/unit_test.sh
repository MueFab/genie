#!/bin/bash
# Run genie unit tests without external data
set -e

############ Check prerequisites ###############

if ! command -v git &> /dev/null
then
    echo "Git could not be found. Please install git (debian: sudo apt-get install git)."
    exit 1
fi

if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    fileExt=".exe"
else
    fileExt=""
fi

git_root_dir="$(git rev-parse --show-toplevel)"

if [[ ! -x $git_root_dir/cmake-build-debug/bin/util-tests$fileExt ]]
then
    echo "Utils test not found. Please build genie in debug mode first."
    exit 1
fi

if [[ ! -x $git_root_dir/cmake-build-debug/bin/coding-tests$fileExt ]]
then
    echo "Coding test not found. Please build genie in debug mode first."
    exit 1
fi

if [[ ! -x $git_root_dir/cmake-build-debug/bin/gabac-tests$fileExt ]]
then
    echo "Gabac test not found. Please build genie in debug mode first."
    exit 1
fi

if [[ ! -x $git_root_dir/cmake-build-debug/bin/read-tests$fileExt ]]
then
    echo "Read test not found. Please build genie in debug mode first."
    exit 1
fi

if [[ ! -x $git_root_dir/cmake-build-debug/bin/quality-tests$fileExt ]]
then
    echo "Quality test not found. Please build genie in debug mode first."
    exit 1
fi

############ Run ###############

echo "Running unit tests:"
echo "Utils..."
cd $git_root_dir/cmake-build-debug/bin
./util-tests$fileExt --gtest_output=xml:utils_test.xml --gtest_brief=1 || { echo 'Utils test failed!' ; exit 1; }
echo "Utils test passed!"
echo "Coding..."
cd $git_root_dir/cmake-build-debug/bin
./coding-tests$fileExt --gtest_output=xml:coding_test.xml --gtest_brief=1 || { echo 'Coding test failed!' ; exit 1; }
echo "Coding test passed!"
echo "Gabac..."
cd $git_root_dir/cmake-build-debug/bin
./gabac-tests$fileExt --gtest_output=xml:gabac_test.xml --gtest_brief=1 || { echo 'Gabac test failed!' ; exit 1; }
echo "Gabac test passed!"
echo "Read..."
cd $git_root_dir/cmake-build-debug/bin
./read-tests$fileExt --gtest_output=xml:read_test.xml --gtest_brief=1 || { echo 'Read test failed!' ; exit 1; }
echo "Read test passed!"
echo "Quality..."
cd $git_root_dir/cmake-build-debug/bin
./quality-tests$fileExt --gtest_output=xml:calq.xml --gtest_brief=1 || { echo 'Quality test failed!' ; exit 1; }
echo "Qquality test passed!"