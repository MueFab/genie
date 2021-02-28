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
else
    fileExt=""
fi


git_root_dir="$(git rev-parse --show-toplevel)"


############ Run ###############

echo "*** Single-end fastq"
# Get fastq file no 1
wget --no-check-certificate \
    https://cloud.fab-mue.de/s/J8bYea9BYfdNPBj/download \
    -O /tmp/ERR174310_short_1.fastq.gz \
    || { echo 'Could not download single end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_short_1.fastq.gz

# Check conversion of single end fastq to mgrec -> testing mgrec exporter
echo "** fastq -> (genie) -> mgrec"
/usr/bin/time -f "Command ran for %Es, used %Mkb ram" \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -o /tmp/single_fastq.mgrec \
    -i /tmp/ERR174310_short_1.fastq -f \
    || { echo 'Genie failed to convert fastq to mgrec!' ; exit 1; }
rm /tmp/single_fastq.mgrec

# Compress to mgb -> Testing single-end fastq importer, qv-writeout, name-tokenizer, spring encoder, mgb-exporter
echo "** fastq -> (genie) -> mgb"
/usr/bin/time -f "Command ran for %Es, used %Mkb ram" \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -i /tmp/ERR174310_short_1.fastq \
    -o /tmp/single_fastq.mgb -f \
    || { echo 'Genie failed to compress single end fastq!' ; exit 1; }

# Decompress to fastq again -> Testing single-end mgb-importer, qv-decoder, name-decoder, spring decoder, fastq exporter
echo "** mgb -> (genie) -> fastq"
/usr/bin/time -f "Command ran for %Es, used %Mkb ram" \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -o /tmp/single_fastq.fastq \
    -i /tmp/single_fastq.mgb -f \
    || { echo 'Genie failed to decompress single end fastq!' ; exit 1; }
rm /tmp/single_fastq.fastq

# Decompress using reference decoder, conformance check for single end fastq mgb
echo "** mgb -> (ref_decoder) -> mgrec"
/usr/bin/time -f "Command ran for %Es, used %Mkb ram" \
    $MPEGG_REF_DECODER \
    -i /tmp/single_fastq.mgb \
    -o /tmp/single_fastq.mgrec \
    || { echo 'MPEGG reference decoder failed to decompress single end fastq!' ; exit 1; }
rm /tmp/single_fastq.mgb

# Check conversion of single end mgrec to fastq -> testing mgrec importer
echo "** mgrec -> (genie) -> fastq"
/usr/bin/time -f "Command ran for %Es, used %Mkb ram" \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -i /tmp/single_fastq.mgrec \
    -o /tmp/single_fastq.fastq -f \
    || { echo 'Genie failed to convert mgrec to fastq!' ; exit 1; }
rm /tmp/single_fastq.mgrec
rm /tmp/single_fastq.fastq




echo "*** Paired-end fastq"
# Get fastq file no 2
wget --no-check-certificate \
    https://cloud.fab-mue.de/s/Hzx8b29cbAYBBto/download \
    -O /tmp/ERR174310_short_2.fastq.gz \
    || { echo 'Could not download paired end fastq!' ; exit 1; }
gzip -df /tmp/ERR174310_short_2.fastq.gz

# Check conversion of paired end fastq to mgrec -> testing mgrec exporter
echo "** fastq -> (genie) -> mgrec"
/usr/bin/time -f "Command ran for %Es, used %Mkb ram" \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -o /tmp/paired_fastq.mgrec \
    -i /tmp/ERR174310_short_1.fastq \
    --input-suppl-file /tmp/ERR174310_short_2.fastq -f \
    || { echo 'Genie failed to convert fastq to mgrec!' ; exit 1; }
rm /tmp/paired_fastq.mgrec

# Compress to mgb -> Testing paired-end fastq importer, qv-writeout, name-tokenizer, spring encoder, mgb-exporter
echo "** fastq -> (genie) -> mgb"
/usr/bin/time -f "Command ran for %Es, used %Mkb ram" \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -i /tmp/ERR174310_short_1.fastq \
    --input-suppl-file /tmp/ERR174310_short_2.fastq \
    -o /tmp/paired_fastq.mgb -f \
    || { echo 'Genie failed to compress paired end fastq!' ; exit 1; }
rm /tmp/ERR174310_short_1.fastq
rm /tmp/ERR174310_short_2.fastq

# Decompress to fastq again -> Testing paired-end mgb-importer, qv-decoder, name-decoder, spring decoder, fastq exporter
echo "** mgb -> (genie) -> fastq"
/usr/bin/time -f "Command ran for %Es, used %Mkb ram" \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -o /tmp/paired_fastq_1.fastq \
    --output-suppl-file /tmp/paired_fastq_2.fastq \
    -i /tmp/paired_fastq.mgb -f \
    || { echo 'Genie failed to decompress paired end fastq!' ; exit 1; }
rm /tmp/paired_fastq_1.fastq
rm /tmp/paired_fastq_2.fastq

# Decompress using reference decoder, conformance check for paired end fastq mgb
echo "** mgb -> (ref_decoder) -> mgrec"
/usr/bin/time -f "Command ran for %Es, used %Mkb ram" \
    $MPEGG_REF_DECODER \
    -i /tmp/paired_fastq.mgb \
    -o /tmp/paired_fastq.mgrec \
    || { echo 'MPEGG reference decoder failed to decompress paired end fastq!' ; exit 1; }
rm /tmp/paired_fastq.mgb

# Check conversion of paired end mgrec to fastq -> testing mgrec importer
echo "** mgrec -> (genie) -> fastq"
/usr/bin/time -f "Command ran for %Es, used %Mkb ram" \
    $git_root_dir/cmake-build-release/bin/genie$fileExt run \
    -i /tmp/paired_fastq.mgrec \
    -o /tmp/paired_fastq_1.fastq \
    --output-suppl-file /tmp/paired_fastq_2.fastq -f \
    || { echo 'Genie failed to convert mgrec to fastq!' ; exit 1; }
rm /tmp/paired_fastq.mgrec
rm /tmp/paired_fastq_1.fastq
rm /tmp/paired_fastq_2.fastq
