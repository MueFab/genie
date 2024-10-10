#!/bin/bash
# Build genie in debug or release mode

############ Check prerequisites ###############

if [[ "$1" != "debug" && "$1" != "release" && "$1" != "doc" ]]; then
    echo "Usage: build.sh [debug|release|doc]"
    exit 1
fi

if ! command -v git &> /dev/null
then
    echo "Git could not be found. Please install git (debian: sudo apt-get install git)."
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

if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    architecture="-A x64"
    sam_support="-DGENIE_SAM_SUPPORT=OFF"
else
    architecture=""
    sam_support="-DGENIE_SAM_SUPPORT=ON"
fi

git_root_dir="$(git rev-parse --show-toplevel)"

############ Run ###############

cd $git_root_dir

if [[ "$1" == "debug" ]]; then
    mkdir -p cmake-build-debug
    cd cmake-build-debug
    cmake .. $architecture -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DGENIE_USE_OPENMP=ON -DBUILD_DOCUMENTATION=ON -DGENIE_WERROR=ON $sam_support || { echo 'Cmake failed!' ; exit 1; }

    if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
        echo "Skipping automatic build for MSVC"
        exit 0
    fi

    make -j || { echo 'Make failed!' ; exit 1; }

elif [[ "$1" == "release" ]]; then
    mkdir -p cmake-build-release
    cd cmake-build-release
    cmake .. $architecture -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DGENIE_USE_OPENMP=ON -DBUILD_DOCUMENTATION=OFF -DGENIE_WERROR=ON $sam_support || { echo 'Cmake failed!' ; exit 1; }

    if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
        echo "Skipping automatic build for MSVC"
        exit 0
    fi

    make -j || { echo 'Make failed!' ; exit 1; }
else
    if ! command -v doxygen &> /dev/null
    then
        echo "Doxygen could not be found. Please install cmake (debian: sudo apt-get install doxygen)."
         exit 1
    fi
    mkdir -p cmake-build-debug
    cd cmake-build-debug
    cmake .. $architecture -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DGENIE_USE_OPENMP=ON -DBUILD_DOCUMENTATION=ON -DGENIE_WERROR=ON $sam_support || { echo 'Cmake failed!' ; exit 1; }

    make doc || { echo 'Generating documentation failed!' ; exit 1; }
fi
