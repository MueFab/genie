#!/bin/bash
# Do a round trip with the MPEG-G reference decoder
set -e

primary_fastq_file="$1"
paired_fastq_file="$2"

############ Check prerequisites ###############

if ! command -v git &> /dev/null
then
    echo "Git could not be found. Please install git (debian: sudo apt-get install git)."
    exit 1
fi

if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    exe_file_extension=".exe"
else
    exe_file_extension=""
fi

if [[ "$OSTYPE" != "linux"* ]]; then
    echo "Skipping memcheck for non-linux OS"
    exit 0
fi


git_root_dir="$(git rev-parse --show-toplevel)"


############ Run ###############

compress_roundtrip () {
    genie_encoder_parameters="$1"
    if [[ "$paired_fastq_file" == "" ]]; then
        genie_paired_input_parameter=""
        genie_paired_output_parameter=""
    else
        genie_paired_input_parameter="--input-suppl-file $paired_fastq_file"
        genie_paired_output_parameter="--output-suppl-file $paired_fastq_file"
    fi

    echo "Genie transcode fastq to mgrec"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension transcode-fastq \
        -i $primary_fastq_file \
        $genie_paired_input_parameter \
        -o /tmp/transcoded.mgrec -f \
        || { echo "Genie transcode ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }

    echo "Genie compress"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -i /tmp/transcoded.mgrec \
        -o /tmp/output.mgb -f \
        $genie_encoder_parameters \
        || { echo "Genie compress ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }

    rm /tmp/transcoded.mgrec
    rm /tmp/output.mgb.json
    rm /tmp/output.mgb.unsupported.mgrec

    echo "Genie decompress"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -o /tmp/output.mgrec \
        -i /tmp/output.mgb -f \
        || { echo "Genie decompress ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }


    echo "Genie transcode fastq to mgrec"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension transcode-fastq \
        -i /tmp/output.mgrec \
        -o /tmp/output_1.fastq \
        --output-suppl-file /tmp/output_2.fastq -f \
        || { echo "Genie transcode ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }

    
    rm /tmp/output_1.fastq
    rm -f /tmp/output_2.fastq
    rm /tmp/output.mgb
}

compress_roundtrip "--low-latency --qv none --read-ids none"
compress_roundtrip "--low-latency" 
compress_roundtrip "--qv none --read-ids none"
compress_roundtrip ""
