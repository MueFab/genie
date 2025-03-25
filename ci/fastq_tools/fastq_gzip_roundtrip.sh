#!/bin/bash
# Do a round trip with the MPEG-G reference decoder
set -e

primary_fastq_file="$1"
paired_fastq_file="$2"
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

    echo "-----------------Zip Input files"
    primary_gz_file="$primary_fastq_file.gz"
    eval gzip -c "$primary_fastq_file" > $primary_gz_file || { echo "Zipping $primary_fastq_file failed!"; exit 1; }


    if [[ "$paired_fastq_file" == "" ]]; then
        paired_fastq_parameter=""
        fastq_cmp_input_parameters="-i $primary_fastq_file -p $working_dir/output_1.fastq"
    else
        paired_gz_file="$paired_fastq_file.gz"
        eval gzip -c "$paired_fastq_file" > "$paired_gz_file" || { echo "Zipping $paired_fastq_file failed!"; exit 1; }

    echo "-----------------Zipping Complete"

        paired_fastq_parameter="--input-suppl-file $paired_gz_file"
        fastq_cmp_input_parameters="-i $primary_fastq_file -j $paired_fastq_file -p $working_dir/output_1.fastq -q $working_dir/output_2.fastq"
        if [[ "$genie_encoder_parameters" != *"--low-latency"* ]] && [[ "$genie_encoder_parameters" != *"--read-ids none"* ]]; then
            genie_decoder_recombine="--combine-pairs"
            fastq_cmp_error_parameters_refdecoder="$fastq_cmp_error_parameters_refdecoder --broken_pairing"
        fi
    fi


    echo "-----------------Genie compress"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -i $primary_gz_file \
        $paired_fastq_parameter \
        -o $working_dir/output.mgb -f \
        -w $working_dir \
        $genie_encoder_parameters \
        --entropy gabac \
        || { echo "Genie compress ($primary_gz_file; $paired_gz_file; $genie_encoder_parameters) failed!" ; exit 1; }

    echo "-----------------Compressed:"
    ls -l $primary_gz_file
    if [[ "$paired_gz_file" != "" ]]; then
        ls -l $paired_gz_file
    fi
    ls -l $working_dir/output.mgb

    rm $working_dir/output.mgb.json

    echo "-----------------Genie decompress"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -o $working_dir/output_1.fastq.gz \
        --output-suppl-file $working_dir/output_2.fastq.gz -f \
        -w $working_dir \
        -i $working_dir/output.mgb -f \
        $genie_decoder_recombine \
        || { echo "Genie decompress ($primary_gz_file; $paired_gz_file; $genie_encoder_parameters) failed!" ; exit 1; }

    echo "-----------------Unzip output"

    eval gzip -d $working_dir/output_1.fastq.gz
    eval gzip -d $working_dir/output_2.fastq.gz

    ls -l $working_dir/output_1.fastq
    ls -l $working_dir/output_2.fastq

    echo "-----------------Check output files:"
    $git_root_dir/ci/fastq_tools/fastq_cmp_complete.py $fastq_cmp_input_parameters $fastq_cmp_error_parameters || { echo "Invalid output!" ; exit 1; }
    echo "-----------------Output files ok!"

    rm $working_dir/output_1.fastq
    rm -f $working_dir/output_2.fastq

    if [[ "$OSTYPE" != "win32" && "$OSTYPE" != "cygwin" && "$OSTYPE" != "msys" ]]; then
        echo "-----------------Refdecoder decompress"
        eval $timing_command \
            $MPEGG_REF_DECODER \
            -i $working_dir/output.mgb \
            -o $working_dir/output.mgrec \
            || { echo "Reference decoder ($primary_gz_file; $paired_gz_file; $genie_encoder_parameters) failed!" ; exit 1; }
        rm $working_dir/output.mgb

        echo "-----------------Refdecoder decoded:"
        ls -l $working_dir/output.mgrec

        echo "-----------------Refdecoder output transcode"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension transcode-fastq \
        -i $working_dir/output.mgrec \
        -o $working_dir/output_1.fastq \
        --output-suppl-file $working_dir/output_2.fastq -f \
        || { echo "Genie transcode ($primary_gz_file; $paired_gz_file; $genie_encoder_parameters) failed!" ; exit 1; }

        rm $working_dir/output.mgrec

        echo "-----------------Refdecoder output transcoded:"
        ls -l $working_dir/output_1.fastq
        ls -l $working_dir/output_2.fastq

        echo "-----------------Check output files:"
        $git_root_dir/ci/fastq_tools/fastq_cmp_complete.py $fastq_cmp_input_parameters $fastq_cmp_error_parameters_refdecoder || { echo "Invalid output!" ; exit 1; }
        echo "-----------------Output files ok!"

        rm $working_dir/output_1.fastq
        rm -f $working_dir/output_2.fastq
    fi
}

if [[ "$paired_fastq_file" == "" ]]; then
#    compress_roundtrip "--low-latency --qv none --read-ids none" "--broken_names --broken_qualities"
    compress_roundtrip "--low-latency" ""
#    compress_roundtrip "--qv none --read-ids none" "--broken_names --broken_qualities --broken_order"
#    compress_roundtrip "" "--broken_order"
else
#    compress_roundtrip "--low-latency --qv none --read-ids none" "--broken_names --broken_qualities"
    compress_roundtrip "--low-latency" "--patched_names"
#    compress_roundtrip "--qv none --read-ids none" "--broken_names --broken_qualities --broken_order --broken_pairing"
#    compress_roundtrip "" "--broken_order --patched_names"
fi
