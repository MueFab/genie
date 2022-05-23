#!/bin/bash 

git_root_dir="$(git rev-parse --show-toplevel)"
genie="$git_root_dir/cmake-build-release/bin/genie"
samtools="samtools"
#$genie transcode-sam -i "$1" -o "$1.mgrec" -r "$2" -c -f
#$genie transcode-sam -i "$1.mgrec" -o "$1.tmp.sam" -f -r "$2"

#samtools view -H "$1" > "$1.clean.sam"
#cat "$1.tmp.sam" >> "$1.clean.sam"

#rm "$1.tmp.sam"

$genie transcode-sam -i "$1" -o "-.mgrec" -r "$2" -c -f | $genie transcode-sam -i "-.mgrec" -o "-.sam" -f -r "$2" | $samtools sort  -@ 6 -O BAM
