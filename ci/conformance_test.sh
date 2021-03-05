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


############ Run ###############

echo "*** Single-end fastq"

# Get fastq file no 1
curl \
    http://www.tnt.uni-hannover.de/~voges/data/genie/wgs/h-sapiens/ERP001775/ERR174310_short_1.fastq.gz \
    --output /tmp/ERR174310_short_1.fastq.gz \
    || { echo 'Could not download single end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_short_1.fastq.gz

$git_root_dir/ci/fastq_tools/fastq_roundtrip.sh "/tmp/ERR174310_short_1.fastq" ""

echo "*** Paired-end fastq"
# Get fastq file no 2
curl \
    http://www.tnt.uni-hannover.de/~voges/data/genie/wgs/h-sapiens/ERP001775/ERR174310_short_2.fastq.gz \
    --output /tmp/ERR174310_short_2.fastq.gz \
    || { echo 'Could not download paired end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_short_2.fastq.gz

$git_root_dir/ci/fastq_tools/fastq_roundtrip.sh "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq"

rm /tmp/ERR174310_short_2.fastq
rm /tmp/ERR174310_short_1.fastq
