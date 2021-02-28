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

echo "Single-end fastq"
wget --no-check-certificate https://cloud.fab-mue.de/s/bC6ZQrCxMkW2P4y/download -O /tmp/ERR174310_tiny_1.fastq.gz || { echo 'Could not download single end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_tiny_1.fastq.gz
valgrind --suppressions=$git_root_dir/ci/omp.supp --gen-suppressions=all --leak-check=full --track-origins=yes --error-exitcode=1 $git_root_dir/cmake-build-release/bin/genie$fileExt run -i /tmp/ERR174310_tiny_1.fastq -o /tmp/single_fastq.mgb -f || { echo 'Memcheck for genie compression of single end fastq failed!' ; exit 1; }
rm /tmp/single_fastq.mgb
#rm /tmp/single_fastq.mgrec


echo "Paired-end fastq"
wget --no-check-certificate https://cloud.fab-mue.de/s/QMoZPiX47NwJqcP/download -O /tmp/ERR174310_tiny_2.fastq.gz || { echo 'Could not download paired end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_tiny_2.fastq.gz
valgrind --suppressions=$git_root_dir/ci/omp.supp --gen-suppressions=all --leak-check=full --track-origins=yes --error-exitcode=1 $git_root_dir/cmake-build-release/bin/genie$fileExt run -i /tmp/ERR174310_tiny_1.fastq --input-suppl-file /tmp/ERR174310_tiny_2.fastq -o /tmp/paired_fastq.mgb -f || { echo 'Memcheck for genie compression of paired end fastq failed!' ; exit 1; }
rm /tmp/paired_fastq.mgb
#rm /tmp/paired_fastq.mgrec
