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

if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "Skipping memcheck for non-linux OS"
    return 0
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

    echo "Genie compress"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -i $primary_fastq_file \
        $genie_paired_input_parameter \
        -o /tmp/output.mgb -f \
        $genie_encoder_parameters \
        || { echo "Genie compress ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }

    echo "Genie decompress"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -o /tmp/output_1.fastq \
        --output-suppl-file /tmp/output_2.fastq \
        -i /tmp/output.mgb -f \
        || { echo "Genie decompress ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }
    rm /tmp/output_1.fastq
    rm -f /tmp/output_2.fastq
    rm /tmp/output.mgb
}

convert_roundtrip() {
    if [[ "$paired_fastq_file" == "" ]]; then
        genie_paired_input_parameter=""
    else
        genie_paired_input_parameter="--input-suppl-file $paired_fastq_file"
    fi

    echo "Genie convert"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -o /tmp/output.mgrec \
        -i $primary_fastq_file -f \
        $genie_paired_input_parameter \
        $genie_encoder_parameters \
        || { echo "Genie convert ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }

    echo "Genie convert back"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -i /tmp/output.mgrec \
        -o /tmp/output_1.fastq \
        --output-suppl-file /tmp/output_2.fastq \
        || { echo "Genie convert ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }

    rm /tmp/output_1.fastq
    rm /tmp/output_2.fastq
    rm /tmp/output.mgrec
}

compress_roundtrip "--low-latency --qv none --read-ids none"
compress_roundtrip "--low-latency" 
compress_roundtrip "--qv none --read-ids none"
compress_roundtrip ""
convert_roundtrip
