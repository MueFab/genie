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

if [[ ! -x $git_root_dir/cmake-build-debug/bin/Debug/annotationParameterSet-tests$fileExt ]]
then
    echo "annotationParameterSet test not found. Please build genie in debug mode first."
    exit 1
fi

if [[ ! -x $git_root_dir/cmake-build-debug/bin/Debug/VariantGenotypeRecord-tests$fileExt ]]
then
    echo "VariantGenotypeRecord test not found. Please build genie in debug mode first."
    exit 1
fi

if [[ ! -x $git_root_dir/cmake-build-debug/bin/Debug/VariantSiteRecord-tests$fileExt ]]
then
    echo "VariantSiteRecord test not found. Please build genie in debug mode first."
    exit 1
fi

############ Run ###############

echo "Running unit tests:"
echo "annotationParameterSet..."
cd $git_root_dir/cmake-build-debug/bin/Debug
./annotationParameterSet-tests$fileExt --gtest_output=xml:annotationParameterSet-tests.xml --gtest_brief=1 || { echo 'annotationParameterSet-tests failed!' ; exit 1; }
echo "annotationParameterSet-tests passed!"
echo "VariantGenotypeRecord..."
cd $git_root_dir/cmake-build-debug/bin/Debug
./VariantGenotypeRecord-tests$fileExt --gtest_output=xml:VariantGenotypeRecord-tests.xml --gtest_brief=1 || { echo 'VariantGenotypeRecord-tests failed!' ; exit 1; }
echo "VariantGenotypeRecord-tests passed!"
echo "VariantSiteRecord..."
cd $git_root_dir/cmake-build-debug/bin/Debug
./VariantSiteRecord-tests$fileExt --gtest_output=xml:VariantSiteRecord-tests.xml --gtest_brief=1 || { echo 'VariantSiteRecord-tests failed!' ; exit 1; }
echo "VariantSiteRecord-tests passed!"
