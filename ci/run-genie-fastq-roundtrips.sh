#!/usr/bin/env bash

set -e

if [[ ! -z "${CI}" ]]; then set -x; fi
set -euo pipefail

self="${0}"
self_name="${self##*/}"

git rev-parse --git-dir 1>/dev/null # Exit if not inside Git repo
readonly git_root_dir="$(git rev-parse --show-toplevel)"

readonly genie="${git_root_dir}/build/bin/genie"
if [[ ! -x "${genie}" ]]; then
    echo "[${self_name}] error: genie application does not exist: ${genie}"
    exit 1
fi

tempdir=$(mktemp -d)

URL_ROOT="ftp://ftp.sra.ebi.ac.uk/vol1/fastq/SRR554/SRR554369/"
FILE_1="SRR554369_1.fastq"
FILE_2="SRR554369_2.fastq"

# first download files
curl $URL_ROOT/$FILE_1.gz -o $tempdir/$FILE_1.gz
curl $URL_ROOT/$FILE_2.gz -o $tempdir/$FILE_2.gz

# unzip
gunzip $tempdir/$FILE_1.gz $tempdir/$FILE_2.gz

# variables
COMPRESSED_FILE=$tempdir/compressed.mgb
DECOMPRESSED_FILE=$tempdir/decompressed.fastq
DECOMPRESSED_PAIR_FILE=$tempdir/decompressed_pair.fastq

# first test single end compression for both files

for INFILE in $tempdir/$FILE_1 $tempdir/$FILE_2; do
    ${genie} run -i $INFILE -o $COMPRESSED_FILE
    ${genie} run -i $COMPRESSED_FILE -o $DECOMPRESSED_FILE
    ${git_root_dir}/util/cmp-reordered-fastq.sh \
        -1 $INFILE \
        -2 $DECOMPRESSED_FILE
    rm $COMPRESSED_FILE $DECOMPRESSED_FILE
done

# now we test paired end compression
# for that, first change the read ids in file 2 to match those in file 1
# since MPEG-G currently allows only one read id per pair
paste -d \\n $tempdir/$FILE_1 $tempdir/$FILE_2 | awk 'NR % 8 == 1 || NR % 8 == 4 ||  NR % 8 == 6 || NR % 8 == 0' > $tempdir/$FILE_2.tmp
mv $tempdir/$FILE_2.tmp $tempdir/$FILE_2

# compress
${genie} run -i $tempdir/$FILE_1 --input-suppl-file $tempdir/$FILE_2 -o $COMPRESSED_FILE

# decompress without combine pairs
${genie} run -i $COMPRESSED_FILE -o $DECOMPRESSED_FILE --output-suppl-file $DECOMPRESSED_PAIR_FILE
${git_root_dir}/util/cmp-reordered-fastq.sh \
    -1 $tempdir/$FILE_1 \
    -2 $DECOMPRESSED_FILE
${git_root_dir}/util/cmp-reordered-fastq.sh \
    -1 $tempdir/$FILE_2 \
    -2 $DECOMPRESSED_PAIR_FILE
rm $DECOMPRESSED_FILE $DECOMPRESSED_PAIR_FILE


# decompress with combine pairs
${genie} run -i $COMPRESSED_FILE -o $DECOMPRESSED_FILE --output-suppl-file $DECOMPRESSED_PAIR_FILE --combine-pairs
${git_root_dir}/util/cmp-reordered-fastq.sh \
    -1 $tempdir/$FILE_1 \
    -2 $DECOMPRESSED_FILE
${git_root_dir}/util/cmp-reordered-fastq.sh \
    -1 $tempdir/$FILE_2 \
    -2 $DECOMPRESSED_PAIR_FILE
# additionally check that the pairs are matched by looking at read names
cmp <(sed -n '1~4p' $DECOMPRESSED_FILE) <(sed -n '1~4p' $DECOMPRESSED_PAIR_FILE)

rm $COMPRESSED_FILE $DECOMPRESSED_FILE $DECOMPRESSED_PAIR_FILE

rm -rf "${tempdir}"

echo "[${self_name}] success"
