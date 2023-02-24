#!/bin/bash 

genie="/home/muenteferi/nobackup/genie/cmake-build-release/bin/genie"
samtools="/home/muenteferi/nobackup/samtools-build/bin/samtools"

cat "$1" | \
$samtools sort -n -@ 6 -O SAM | \
$genie transcode-sam -i "-.sam" -o "-.mgrec" -r "$2" -c -f -w ./tmp -t 6 | \
$genie transcode-sam -i "-.mgrec" -o "-.sam" -f -r "$2" -w ./tmp2 | \
$samtools sort  -@ 6 -O BAM
