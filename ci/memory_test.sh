#!/bin/bash
# Do roundtrips with very small files (performance reasons) while checking for memory issues
set -e

############ Check prerequisites ###############

if ! command -v git &> /dev/null
then
    echo "Git could not be found. Please install git (debian: sudo apt-get install git)."
    exit 1
fi

if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    fileExt=".exe"
else
    fileExt=""
fi

if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "Skipping memcheck for non-linux OS"
    return 0
fi


git_root_dir="$(git rev-parse --show-toplevel)"


############ Run ###############

echo "*** Single-end fastq"
# Get fastq file no 1
curl -L \
    https://seafile.cloud.uni-hannover.de/f/436246ec6e0245728fb1/?dl=1  \
    --output /tmp/ERR174310_tiny_1.fastq.gz \
    || { echo 'Could not download single end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_tiny_1.fastq.gz

$git_root_dir/ci/fastq_tools/fastq_memcheck.sh "/tmp/ERR174310_tiny_1.fastq" ""

echo "*** Paired-end fastq"
# Get fastq file no 2
curl -L \
    https://seafile.cloud.uni-hannover.de/f/5cdec5d05dca497092c4/?dl=1 \
    --output /tmp/ERR174310_tiny_2.fastq.gz \
    || { echo 'Could not download paired end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_tiny_2.fastq.gz

$git_root_dir/ci/fastq_tools/fastq_memcheck.sh "/tmp/ERR174310_tiny_2.fastq" ""

rm /tmp/ERR174310_tiny_1.fastq
rm /tmp/ERR174310_tiny_2.fastq
