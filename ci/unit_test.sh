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


run_the_test()
{
    cd $git_root_dir/cmake-build-debug/bin/Debug
    TestSetName=$1
    if [[ ! -x $TestSetName$fileExt ]]
    then
        echo $TestSetName" not found. Please build genie in debug mode first."
        exit 1
    fi

    echo $TestSetName
    ./$TestSetName$fileExt --gtest_output=xml:$TestSetName.xml --gtest_brief=1 || { echo $TestSetName ' failed!';  }
    echo $TestSetName" passed!"
}

############ Run ###############

echo "Running unit tests:"


run_the_test "annotationParameterSet-tests"
run_the_test "entropy-tests"
run_the_test "genie-test-annotation"
run_the_test "genie-test-conformance"
run_the_test "genie-test-contact"
run_the_test "genie-test-genotype"
run_the_test "genie-test-likelihood"
run_the_test "genie-test-records"
run_the_test "mpeggBSC-tests"
run_the_test "VariantSiteRecord-tests"


