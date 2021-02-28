#!/bin/bash
# Do a round trip with the MPEG-G reference decoder
set -e

############ Check prerequisites ###############

if ! command -v git &> /dev/null
then
    echo "Git could not be found. Please install git (debian: sudo apt-get install git)."
    exit 1
fi

if [[ ! -x $MPEGG_REF_DECODER ]]
then
    echo "MPEG-G reference decoder could not be found. Please set environment variable MPEGG_REF_DECODER."
    exit 1
fi

if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    fileExt=".exe"
    timecmd=""
    timeMsg=""
elif [[ "$OSTYPE" == "darwin"* ]]; then
    fileExt=""
    timeMsg="Command ran for %Es, used %Mkb ram"
    timecmd='gtime -f'
else 
    fileExt=""
    timeMsg="Command ran for %Es, used %Mkb ram"
    timecmd='/usr/bin/time -f'
fi


git_root_dir="$(git rev-parse --show-toplevel)"


compress_roundtrip () {

if [[ "$2" == "" ]]; then
    second_input_file=""
else
    second_input_file="--input-suppl-file $2"
fi

echo "Genie compress"
$timecmd "$timeMsg" \
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
$timecmd "$timeMsg" \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -o /tmp/output_1.fastq \
    --output-suppl-file output_2.fastq \
    -i /tmp/output.mgb -f \
    || { echo "Genie decompress ($1; $2; $3) failed!" ; exit 1; }
rm /tmp/output_1.fastq
rm -f /tmp/output_2.fastq

echo "Refdecoder decompress"
$timecmd "$timeMsg" \
    $MPEGG_REF_DECODER \
    -i /tmp/output.mgb \
    -o /tmp/output.mgrec \
    || { echo "Reference decoder ($1; $2; $3) failed!" ; exit 1; }
rm /tmp/output.mgb

echo "Genie convert"
$timecmd "$timeMsg" \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -o /tmp/output_1.fastq \
    --output-suppl-file /tmp/output_2.fastq \
    -i /tmp/output.mgrec -f \
    || { echo "Genie convert ($1; $2; $3) failed!" ; exit 1; }
rm /tmp/output_1.fastq
rm -f /tmp/output_2.fastq
rm /tmp/output.mgrec
}

convert_roundtrip() {
if [[ "$2" == "" ]]; then
    second_input_file=""
else
    second_input_file="--input-suppl-file $2"
fi

echo "Genie convert"
$timecmd "$timeMsg" \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -o /tmp/output.mgrec \
    -i $1 -f \
    $second_input_file \
    $3 \
    || { echo "Genie convert ($1; $2; $3) failed!" ; exit 1; }

echo "Genie convert back"
$timecmd "$timeMsg" \
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
wget --no-check-certificate \
    https://cloud.fab-mue.de/s/J8bYea9BYfdNPBj/download \
    -O /tmp/ERR174310_short_1.fastq.gz \
    || { echo 'Could not download single end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_short_1.fastq.gz

compress_roundtrip "/tmp/ERR174310_short_1.fastq" "" "--low-latency --qv none --read-ids none"
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "" "--low-latency"
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "" "--qv none --read-ids none"
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "" ""
convert_roundtrip "/tmp/ERR174310_short_1.fastq" "" ""

echo "*** Paired-end fastq"
# Get fastq file no 2
wget --no-check-certificate \
    https://cloud.fab-mue.de/s/Hzx8b29cbAYBBto/download \
    -O /tmp/ERR174310_short_2.fastq.gz \
    || { echo 'Could not download paired end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_short_2.fastq.gz

compress_roundtrip "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq" "--low-latency --qv none --read-ids none"
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq" "--low-latency"
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq" "--qv none --read-ids none"
compress_roundtrip "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq" ""
convert_roundtrip "/tmp/ERR174310_short_1.fastq" "/tmp/ERR174310_short_2.fastq" ""
rm /tmp/ERR174310_short_2.fastq
rm /tmp/ERR174310_short_1.fastq
