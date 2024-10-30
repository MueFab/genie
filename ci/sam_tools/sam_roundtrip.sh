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
    genie_decoder_ref_parameter=""
    genie_transcoder_ref_parameter="--no_ref"
    if [[ "$genie_encoder_parameters" == *"-r "* ]]; then
         genie_decoder_ref_parameter="-r $fasta_file"
         genie_transcoder_ref_parameter="-r $fasta_file"
    fi
    echo "-----------------Genie compress input"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -i $sam_file \
        -o $working_dir/output.mgb -f \
        -w $working_dir \
        $genie_encoder_parameters \
        || { echo "Genie compress ($sam_file; $genie_encoder_parameters) failed!" ; exit 1; }

    echo "-----------------Compressed:"
    ls -l $sam_file
    ls -l $working_dir/output.mgb
    
    rm $working_dir/output.mgb.json
    #rm $working_dir/output.mgb.unsupported.mgrec

    echo "-----------------Genie decompress"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension run \
        -o $working_dir/output.sam \
        -w $working_dir \
        $genie_decoder_ref_parameter \
        -i $working_dir/output.mgb -f \
        || { echo "Genie decompress ($sam_file; $genie_decoder_ref_parameter) failed!" ; exit 1; }

    ls -l $working_dir/output.sam

    echo "-----------------Check output files:"
    $git_root_dir/ci/sam_tools/sam_cmp_complete.py -i $working_dir/output.sam -j $sam_file || { echo "Invalid output!" ; exit 1; }
    echo "-----------------Output files ok!"

  rm $working_dir/output.sam

    if [[ "$OSTYPE" != "win32" && "$OSTYPE" != "cygwin" && "$OSTYPE" != "msys" ]]; then
        echo "-----------------Refdecoder decompress"
        eval $timing_command \
            $MPEGG_REF_DECODER \
            -i $working_dir/output.mgb \
            -o $working_dir/output.mgrec \
            $genie_decoder_ref_parameter \
            || { echo "Reference decoder ($sam_file; $genie_encoder_parameters) failed!" ; exit 1; }
        rm $working_dir/output.mgb

        echo "-----------------Refdecoder decoded:"
        ls -l $working_dir/output.mgrec

        echo "-----------------Refdecoder output transcode"
    eval $timing_command \
        $git_root_dir/cmake-build-release/bin/genie$exe_file_extension transcode-sam \
        -i $working_dir/output.mgrec \
        -o $working_dir/output.sam \
        -f \
        $genie_transcoder_ref_parameter \
        || { echo "Genie transcode ($sam_file; $genie_encoder_parameters) failed!" ; exit 1; }

        rm $working_dir/output.mgrec

        echo "-----------------Refdecoder output transcoded:"
        ls -l $working_dir/output.sam

        echo "-----------------Check output files:"
        $git_root_dir/ci/sam_tools/sam_cmp_complete.py -i $working_dir/output.sam -j $sam_file || { echo "Invalid output!" ; exit 1; }
        echo "-----------------Output files ok!"

        rm $working_dir/output.sam
    fi
}

#compress_roundtrip "--qv none --read-ids none --low-latency"
compress_roundtrip ""
compress_roundtrip "-r $fasta_file"
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
