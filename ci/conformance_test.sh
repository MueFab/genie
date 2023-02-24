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
curl -L \
    https://seafile.cloud.uni-hannover.de/f/e32a272813834b6e888a/?dl=1 \
    --output /tmp/ERR174310_short_1.fastq.gz \
    || { echo 'Could not download single end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_short_1.fastq.gz

$git_root_dir/ci/fastq_tools/fastq_roundtrip.sh "/tmp/ERR174310_short_1.fastq" ""

echo "*** Paired-end fastq"
# Get fastq file no 2
curl -L \
    https://seafile.cloud.uni-hannover.de/f/7b5741e91b604d1bbd6c/?dl=1 \
    --output /tmp/ERR174310_short_2.fastq.gz \
    || { echo 'Could not download paired end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_short_2.fastq.gz

$git_root_dir/ci/fastq_tools/fastq_roundtrip.sh "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq"
rm /tmp/ERR174310_short_2.fastq
rm /tmp/ERR174310_short_1.fastq

if [[ "$OSTYPE" != "win32" && "$OSTYPE" != "cygwin" && "$OSTYPE" != "msys" ]]; then

    echo "*** SAM"
    # Get sam file
    curl -L \
        https://seafile.cloud.uni-hannover.de/f/ed4f5f9570d14c9b9d24/?dl=1  \
        --output /tmp/NA12878_S1_chr22_trunc.sam.gz \
        || { echo 'Could not download sam file!' ; exit 1; }
    gzip -df /tmp/NA12878_S1_chr22_trunc.sam.gz

    # Get sam file
    curl -L \
        https://seafile.cloud.uni-hannover.de/f/99a9c09bd5664cb8a785/?dl=1  \
        --output /tmp/hg19_custom.fa.gz \
        || { echo 'Could not download sam file!' ; exit 1; }
    gzip -df /tmp/hg19_custom.fa.gz

    $git_root_dir/ci/sam_tools/sam_transcoder_test.sh "/tmp/NA12878_S1_chr22_trunc.sam"
    $git_root_dir/ci/sam_tools/sam_roundtrip.sh "/tmp/NA12878_S1_chr22_trunc.sam" "/tmp/hg19_custom.fa"

    rm /tmp/NA12878_S1_chr22_trunc.sam

fi
