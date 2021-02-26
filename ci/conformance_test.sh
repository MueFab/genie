#!/bin/bash
# Do a round trip with the MPEG-G reference decoder
set -e

############ Check prerequisites ###############

if ! command -v git &> /dev/null
then
    echo "Git could not be found. Please install git (debian: sudo apt-get install git)."
    exit 1
fi

if [[ ! -x $MPEGG_REF_DECODER ]]
then
    echo "MPEG-G reference decoder could not be found. Please set environment variable MPEGG_REF_DECODER."
    exit 1
fi

if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    fileExt=".exe"
else
    fileExt=""
fi


git_root_dir="$(git rev-parse --show-toplevel)"


############ Run ###############
