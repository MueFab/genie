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

compress_roundtrip () {

if [[ "$2" == "" ]]; then
    second_input_file=""
    second_output_file=""
else
    second_input_file="--input-suppl-file $2"
    second_output_file="--output-suppl-file $2"
fi

echo "Genie compress"
valgrind --suppressions=$git_root_dir/ci/omp.supp \
    --gen-suppressions=all \
    --leak-check=full \
    --track-origins=yes \
    --error-exitcode=1 \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -i $1 \
    $second_input_file \
    -o /tmp/output.mgb -f \
    $3 \
    || { echo "Genie compress ($1; $2; $3) failed!" ; exit 1; }

echo "Genie decompress"
valgrind --suppressions=$git_root_dir/ci/omp.supp \
    --gen-suppressions=all \
    --leak-check=full \
    --track-origins=yes \
    --error-exitcode=1 \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -o /tmp/output_1.fastq \
    --output-suppl-file output_2.fastq \
    -i /tmp/output.mgb -f \
    || { echo "Genie decompress ($1; $2; $3) failed!" ; exit 1; }
rm /tmp/output_1.fastq
rm -f /tmp/output_2.fastq
rm /tmp/output.mgb
}

convert_roundtrip() {
if [[ "$2" == "" ]]; then
    second_input_file=""
else
    second_input_file="--input-suppl-file $2"
fi

echo "Genie convert"
valgrind --suppressions=$git_root_dir/ci/omp.supp \
    --gen-suppressions=all \
    --leak-check=full \
    --track-origins=yes \
    --error-exitcode=1 \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -o /tmp/output.mgrec \
    -i $1 -f \
    $second_input_file \
    $3 \
    || { echo "Genie convert ($1; $2; $3) failed!" ; exit 1; }

echo "Genie convert back"
valgrind --suppressions=$git_root_dir/ci/omp.supp \
    --gen-suppressions=all \
    --leak-check=full \
    --track-origins=yes \
    --error-exitcode=1 \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -i /tmp/output.mgrec \
    -o /tmp/output_1.fastq \
    --output-suppl-file /tmp/output_2.fastq \
    || { echo "Genie convert ($1; $2; $3) failed!" ; exit 1; }

rm /tmp/output_1.fastq
rm /tmp/output_2.fastq
rm /tmp/output.mgrec
}

echo "*** Single-end fastq"
# Get fastq file no 1
wget --no-check-certificate \
    https://cloud.fab-mue.de/s/bC6ZQrCxMkW2P4y/download  \
    -O /tmp/ERR174310_tiny_1.fastq.gz \
    || { echo 'Could not download single end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_tiny_1.fastq.gz

compress_roundtrip "/tmp/ERR174310_tiny_1.fastq" "" "--low-latency --qv none --read-ids none"
compress_roundtrip "/tmp/ERR174310_tiny_1.fastq" "" "--low-latency"
compress_roundtrip "/tmp/ERR174310_tiny_1.fastq" "" "--qv none --read-ids none"
compress_roundtrip "/tmp/ERR174310_tiny_1.fastq" "" ""
convert_roundtrip "/tmp/ERR174310_tiny_1.fastq" "" ""

echo "*** Paired-end fastq"
# Get fastq file no 2
wget --no-check-certificate \
    https://cloud.fab-mue.de/s/QMoZPiX47NwJqcP/download \
    -O /tmp/ERR174310_tiny_2.fastq.gz \
    || { echo 'Could not download paired end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_tiny_2.fastq.gz

compress_roundtrip "/tmp/ERR174310_tiny_1.fastq" "/tmp/ERR174310_tiny_2.fastq" "--low-latency --qv none --read-ids none"
compress_roundtrip "/tmp/ERR174310_tiny_1.fastq" "/tmp/ERR174310_tiny_2.fastq" "--low-latency"
compress_roundtrip "/tmp/ERR174310_tiny_1.fastq" "/tmp/ERR174310_tiny_2.fastq" "--qv none --read-ids none"
compress_roundtrip "/tmp/ERR174310_tiny_1.fastq" "/tmp/ERR174310_tiny_2.fastq" ""
convert_roundtrip "/tmp/ERR174310_tiny_1.fastq" "/tmp/ERR174310_tiny_2.fastq" ""

rm /tmp/ERR174310_tiny_1.fastq
rm /tmp/ERR174310_tiny_2.fastq
