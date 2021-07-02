#!/bin/bash
# Do a round trip with the MPEG-G reference decoder
set -e

sam_file="$1"
fasta_file="$2"
working_dir="${3:-"./"}"

############ Check prerequisites ###############

if ! command -v git &> /dev/null
then
    echo "Git could not be found. Please install git (debian: sudo apt-get install git)."
    exit 1
fi


if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    exe_file_extension=".exe"
    timing_command=""
elif [[ "$OSTYPE" == "darwin"* ]]; then
    exe_file_extension=""
    timing_command='gtime -f "Command ran for %Es, used %Mkb ram"'
else 
    exe_file_extension=""
    timing_command='/usr/bin/time -f "Command ran for %Es, used %Mkb ram"'
fi


git_root_dir="$(git rev-parse --show-toplevel)"

transcoder_roundtrip () {
    echo "-----------------Genie transcode input"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension transcode-sam \
        -i $sam_file \
        -w $working_dir \
        -o $working_dir/transcoded.mgrec -f \
        || { echo "Genie transcode ($sam_file; $genie_encoder_parameters) failed!" ; exit 1; }

    echo "-----------------input transcoded:"
    ls -l $working_dir/transcoded.mgrec


    echo "-----------------Genie output transcode"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension transcode-sam \
        -i $working_dir/transcoded.mgrec \
        -w $working_dir \
        -o $working_dir/output.sam \
        -f \
        || { echo "Genie transcode ($sam_file; $genie_encoder_parameters) failed!" ; exit 1; }

    rm $working_dir/transcoded.mgrec
    echo "-----------------Output transcoded:"
    ls -l $working_dir/output.sam

    echo "-----------------Check output files:"
    $git_root_dir/ci/sam_tools/sam_cmp_complete.py -i $working_dir/output.sam -j $sam_file || { echo "Invalid output!" ; exit 1; }
    echo "-----------------Output files ok!"

    rm $working_dir/output.sam
}

transcoder_roundtrip ""
