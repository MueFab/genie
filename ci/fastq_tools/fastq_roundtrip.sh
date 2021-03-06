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
    timing_command=""
elif [[ "$OSTYPE" == "darwin"* ]]; then
    exe_file_extension=""
    timing_command='gtime -f "Command ran for %Es, used %Mkb ram"'
    if [[ ! -x $MPEGG_REF_DECODER ]]
    then
        echo "MPEG-G reference decoder could not be found. Please set environment variable MPEGG_REF_DECODER."
        exit 1
    fi
else 
    exe_file_extension=""
    timing_command='/usr/bin/time -f "Command ran for %Es, used %Mkb ram"'
    if [[ ! -x $MPEGG_REF_DECODER ]]
    then
        echo "MPEG-G reference decoder could not be found. Please set environment variable MPEGG_REF_DECODER."
        exit 1
    fi
fi


git_root_dir="$(git rev-parse --show-toplevel)"

compress_roundtrip () {
    genie_encoder_parameters="$1"
    fastq_cmp_error_parameters="$2"
    fastq_cmp_error_parameters_refdecoder="$fastq_cmp_error_parameters"
    genie_decoder_recombine=""
    if [[ "$paired_fastq_file" == "" ]]; then
        paired_fastq_parameter=""
        fastq_cmp_input_parameters="-i $primary_fastq_file -p /tmp/output_1.fastq"
    else
        paired_fastq_parameter="--input-suppl-file $paired_fastq_file"
        fastq_cmp_input_parameters="-i $primary_fastq_file -j $paired_fastq_file -p /tmp/output_1.fastq -q /tmp/output_2.fastq"
        if [[ "$genie_encoder_parameters" != *"--low-latency"* ]] && [[ "$genie_encoder_parameters" != *"--read-ids none"* ]]; then
            genie_decoder_recombine="--combine-pairs"
            fastq_cmp_error_parameters_refdecoder="$fastq_cmp_error_parameters_refdecoder --broken_pairing"
        fi
    fi

    echo "Genie compress"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -i $primary_fastq_file \
        $paired_fastq_parameter \
        -o /tmp/output.mgb -f \
        $genie_encoder_parameters \
        || { echo "Genie compress ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }

    echo "Compressed:"
    ls -l $primary_fastq_file
    if [[ "$paired_fastq_file" != "" ]]; then
        ls -l $paired_fastq_file
    fi
    ls -l /tmp/output.mgb
    echo "Genie decompress"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -o /tmp/output_1.fastq \
        --output-suppl-file /tmp/output_2.fastq \
        -i /tmp/output.mgb -f \
        $genie_decoder_recombine \
        || { echo "Genie decompress ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }

    $git_root_dir/ci/fastq_tools/fastq_cmp_complete.py $fastq_cmp_input_parameters $fastq_cmp_error_parameters || { echo "Invalid output!" ; exit 1; }

    rm /tmp/output_1.fastq
    rm -f /tmp/output_2.fastq

    if [[ "$OSTYPE" != "win32" && "$OSTYPE" != "cygwin" && "$OSTYPE" != "msys" ]]; then
        echo "Refdecoder decompress"
        eval $timing_command \
            $MPEGG_REF_DECODER \
            -i /tmp/output.mgb \
            -o /tmp/output.mgrec \
            || { echo "Reference decoder ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }
        rm /tmp/output.mgb

        echo "Genie convert"
        eval $timing_command \
            $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
            -o /tmp/output_1.fastq \
            --output-suppl-file /tmp/output_2.fastq \
            -i /tmp/output.mgrec -f \
            || { echo "Genie convert ($primary_fastq_file; $paired_fastq_file; $genie_encoder_parameters) failed!" ; exit 1; }

        rm /tmp/output.mgrec

        $git_root_dir/ci/fastq_tools/fastq_cmp_complete.py $fastq_cmp_input_parameters $fastq_cmp_error_parameters_refdecoder || { echo "Invalid output!" ; exit 1; }

        rm /tmp/output_1.fastq
        rm -f /tmp/output_2.fastq
    fi
}

convert_roundtrip() {
    fastq_cmp_error_parameters="$1"
    if [[ "$paired_fastq_file" == "" ]]; then
        paired_fastq_parameter=""
        fastq_cmp_input_parameters="-i $primary_fastq_file -p /tmp/output_1.fastq"
    else
        paired_fastq_parameter="--input-suppl-file $paired_fastq_file"
        fastq_cmp_input_parameters="-i $primary_fastq_file -j $paired_fastq_file -p /tmp/output_1.fastq -q /tmp/output_2.fastq"
    fi

    echo "Genie convert"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -o /tmp/output.mgrec \
        -i $primary_fastq_file -f \
        $paired_fastq_parameter \
        || { echo "Genie convert ($primary_fastq_file; $paired_fastq_file) failed!" ; exit 1; }

    echo "Genie convert back"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -i /tmp/output.mgrec \
        -o /tmp/output_1.fastq \
        --output-suppl-file /tmp/output_2.fastq \
        || { echo "Genie convert ($primary_fastq_file; $paired_fastq_file) failed!" ; exit 1; }

    $git_root_dir/ci/fastq_tools/fastq_cmp_complete.py $fastq_cmp_input_parameters $fastq_cmp_error_parameters || { echo "Invalid output!" ; exit 1; }

    rm /tmp/output_1.fastq
    rm /tmp/output_2.fastq
    rm /tmp/output.mgrec
}

if [[ "$paired_fastq_file" == "" ]]; then
    compress_roundtrip "--low-latency --qv none --read-ids none" "--broken_names --broken_qualities"
    compress_roundtrip "--low-latency" ""
    compress_roundtrip "--qv none --read-ids none" "--broken_names --broken_qualities --broken_order"
    compress_roundtrip "" "--broken_order"
    convert_roundtrip ""
else
    compress_roundtrip "--low-latency --qv none --read-ids none" "--broken_names --broken_qualities"
    compress_roundtrip "--low-latency" "--patched_names"
    compress_roundtrip "--qv none --read-ids none" "--broken_names --broken_qualities --broken_order --broken_pairing"
    compress_roundtrip "" "--broken_order --patched_names"
    convert_roundtrip "--patched_names"
fi
