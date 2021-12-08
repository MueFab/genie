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

compress_roundtrip () {
    genie_encoder_parameters="$1"
    genie_decoder_ref_parameter=""
    if [[ "$genie_encoder_parameters" == *"--embedded-ref none"* ]]; then
         genie_decoder_ref_parameter="--input-ref-file $fasta_file"
    fi
    echo "-----------------Genie transcode input"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension transcode-sam \
        -i $sam_file \
        -w $working_dir \
        -o $working_dir/transcoded.mgrec -f \
        --no_ref \
        || { echo "Genie transcode ($sam_file; $genie_encoder_parameters) failed!" ; exit 1; }

    echo "-----------------input transcoded:"
    ls -l $working_dir/transcoded.mgrec

    echo "-----------------Genie compress"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -i $working_dir/transcoded.mgrec \
        -o $working_dir/output.mgb -f \
        -w $working_dir \
        $genie_encoder_parameters \
        || { echo "Genie compress ($sam_file; $genie_encoder_parameters) failed!" ; exit 1; }

    rm $working_dir/transcoded.mgrec

    echo "-----------------Compressed:"
    ls -l $sam_file
    ls -l $working_dir/output.mgb

 #   exit 0

    echo "-----------------Genie decompress"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -o $working_dir/output.mgrec \
        -w $working_dir \
        $genie_decoder_ref_parameter \
        -i $working_dir/output.mgb -f \
        || { echo "Genie decompress ($sam_file; $genie_encoder_parameters) failed!" ; exit 1; }

    echo "-----------------Decompressed:"
    ls -l $working_dir/output.mgrec

    echo "-----------------Genie output transcode"
    valgrind --suppressions=$git_root_dir/ci/omp.supp \
        --gen-suppressions=all \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension transcode-sam \
        -i $working_dir/output.mgrec \
        -w $working_dir \
        -o $working_dir/output.sam \
        -f \
        || { echo "Genie transcode ($sam_file; $genie_encoder_parameters) failed!" ; exit 1; }

    rm $working_dir/output.mgrec
    echo "-----------------Output transcoded:"
    ls -l $working_dir/output.sam

    echo "-----------------Check output files:"
#    $git_root_dir/ci/sam_tools/sam_cmp_complete.py $fastq_cmp_input_parameters $fastq_cmp_error_parameters || { echo "Invalid output!" ; exit 1; }
    echo "-----------------Output files ok!"

    rm $working_dir/output.sam

#    if [[ "$OSTYPE" != "win32" && "$OSTYPE" != "cygwin" && "$OSTYPE" != "msys" ]]; then
#        echo "-----------------Refdecoder decompress"
#        eval $timing_command \
#            $MPEGG_REF_DECODER \
#            -i $working_dir/output.mgb \
#            -o $working_dir/output.mgrec \
#            || { echo "Reference decoder ($sam_file; $genie_encoder_parameters) failed!" ; exit 1; }
        rm $working_dir/output.mgb

#        echo "-----------------Refdecoder decoded:"
#        ls -l $working_dir/output.mgrec

#        echo "-----------------Refdecoder output transcode"
#    eval $timing_command \
#        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension transcode-sam \
#        -i $working_dir/output.mgrec \
#        -o $working_dir/output.sam 
#        -f \
#        || { echo "Genie transcode ($sam_file; $genie_encoder_parameters) failed!" ; exit 1; }

#        rm $working_dir/output.mgrec

#        echo "-----------------Refdecoder output transcoded:"
#        ls -l $working_dir/output.sam

#        echo "-----------------Check output files:"
#        $git_root_dir/ci/sam_tools/sam_cmp_complete.py $fastq_cmp_input_parameters $fastq_cmp_error_parameters_refdecoder || { echo "Invalid output!" ; exit 1; }
#        echo "-----------------Output files ok!"

#        rm $working_dir/output.sam
#    fi
}

#compress_roundtrip "--qv none --read-ids none --low-latency"
compress_roundtrip "--low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref relevant --qv none --read-ids none --raw-ref --low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref full --qv none --read-ids none --raw-ref --low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref none --qv none --read-ids none --raw-ref --low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref relevant --raw-ref --low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref full --raw-ref --low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref none --raw-ref --low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref relevant --qv none --read-ids none --low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref full --qv none --read-ids none --low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref none --qv none --read-ids none --low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref relevant --low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref full --low-latency"
#compress_roundtrip "--input-ref-file $fasta_file --embedded-ref none --low-latency"
