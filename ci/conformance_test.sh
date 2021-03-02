#!/bin/bash
# Do a round trip with the MPEG-G reference decoder
set -e

############ Check prerequisites ###############

if ! command -v git &> /dev/null
then
    echo "Git could not be found. Please install git (debian: sudo apt-get install git)."
    exit 1
fi

if [[ "$OSTYPE" != "win32" && "$OSTYPE" != "cygwin" && "$OSTYPE" != "msys" ]]; then
    if [[ ! -x $MPEGG_REF_DECODER ]]
    then
        echo "MPEG-G reference decoder could not be found. Please set environment variable MPEGG_REF_DECODER."
        exit 1
    fi
fi

if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    fileExt=".exe"
    timecmd=""
elif [[ "$OSTYPE" == "darwin"* ]]; then
    fileExt=""
    timecmd='gtime -f "Command ran for %Es, used %Mkb ram"'
else 
    fileExt=""
    timecmd='/usr/bin/time -f "Command ran for %Es, used %Mkb ram"'
fi


git_root_dir="$(git rev-parse --show-toplevel)"


compress_roundtrip () {
    if [[ "$2" == "" ]]; then
        second_input_file=""
        cmp_files="-i $1 -p /tmp/output_1.fastq"
    else
        second_input_file="--input-suppl-file $2"
        cmp_files="-i $1 -j $2 -p /tmp/output_1.fastq -q /tmp/output_2.fastq"
    fi

    echo "Genie compress"
    eval $timecmd \
        $git_root_dir/cmake-build-release/bin/genie$fileExt run \
        -i $1 \
        $second_input_file \
        -o /tmp/output.mgb -f \
        $3 \
        || { echo "Genie compress ($1; $2; $3) failed!" ; exit 1; }

    echo "Compressed:"
    ls -l $1
    if [[ "$2" != "" ]]; then
        ls -l $2
    fi
    ls -l /tmp/output.mgb
    echo "Genie decompress"
    eval $timecmd \
        $git_root_dir/cmake-build-release/bin/genie$fileExt run \
        -o /tmp/output_1.fastq \
        --output-suppl-file /tmp/output_2.fastq \
        -i /tmp/output.mgb -f \
        || { echo "Genie decompress ($1; $2; $3) failed!" ; exit 1; }

    $git_root_dir/ci/fastq_cmp_complete.py $cmp_files $4 || { echo "Invalid output!" ; exit 1; }

    rm /tmp/output_1.fastq
    rm -f /tmp/output_2.fastq

    if [[ "$OSTYPE" != "win32" && "$OSTYPE" != "cygwin" && "$OSTYPE" != "msys" ]]; then
        echo "Refdecoder decompress"
        eval $timecmd \
            $MPEGG_REF_DECODER \
            -i /tmp/output.mgb \
            -o /tmp/output.mgrec \
            || { echo "Reference decoder ($1; $2; $3) failed!" ; exit 1; }
        rm /tmp/output.mgb

        echo "Genie convert"
        eval $timecmd \
            $git_root_dir/cmake-build-release/bin/genie$fileExt run \
            -o /tmp/output_1.fastq \
            --output-suppl-file /tmp/output_2.fastq \
            -i /tmp/output.mgrec -f \
            || { echo "Genie convert ($1; $2; $3) failed!" ; exit 1; }

        rm /tmp/output.mgrec

        $git_root_dir/ci/fastq_cmp_complete.py $cmp_files $4 || { echo "Invalid output!" ; exit 1; }

        rm /tmp/output_1.fastq
        rm -f /tmp/output_2.fastq
    fi
}

convert_roundtrip() {
    if [[ "$2" == "" ]]; then
        second_input_file=""
    else
        second_input_file="--input-suppl-file $2"
    fi

    echo "Genie convert"
    eval $timecmd \
        $git_root_dir/cmake-build-release/bin/genie$fileExt run \
        -o /tmp/output.mgrec \
        -i $1 -f \
        $second_input_file \
        $3 \
        || { echo "Genie convert ($1; $2; $3) failed!" ; exit 1; }

    echo "Genie convert back"
    eval $timecmd \
        $git_root_dir/cmake-build-release/bin/genie$fileExt run \
        -i /tmp/output.mgrec \
        -o /tmp/output_1.fastq \
        --output-suppl-file /tmp/output_2.fastq \
        || { echo "Genie convert ($1; $2; $3) failed!" ; exit 1; }

    rm /tmp/output_1.fastq
    rm /tmp/output_2.fastq
    rm /tmp/output.mgrec
}



############ Run ###############

echo "*** Single-end fastq"

# Get fastq file no 1
curl \
    https://cloud.fab-mue.de/s/J8bYea9BYfdNPBj/download \
    --output /tmp/ERR174310_short_1.fastq.gz \
    || { echo 'Could not download single end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_short_1.fastq.gz

compress_roundtrip "/tmp/ERR174310_short_1.fastq" "" "--low-latency --qv none --read-ids none" "-n -s"
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "" "--low-latency" ""
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "" "--qv none --read-ids none" "-n -s -u"
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "" "" "-u"
convert_roundtrip "/tmp/ERR174310_short_1.fastq" "" "" ""

echo "*** Paired-end fastq"
# Get fastq file no 2
curl \
    https://cloud.fab-mue.de/s/Hzx8b29cbAYBBto/download \
    --output /tmp/ERR174310_short_2.fastq.gz \
    || { echo 'Could not download paired end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_short_2.fastq.gz

compress_roundtrip "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq" "--low-latency --qv none --read-ids none" "-n -s -u"
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq" "--low-latency" "-c"
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq" "--qv none --read-ids none" "-n -s -u"
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq" "" "-u -c"
convert_roundtrip "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq" ""
rm /tmp/ERR174310_short_2.fastq
rm /tmp/ERR174310_short_1.fastq
