#!/usr/bin/env bash

reference_sw="../../mpegg-reference-sw/bin/decoder/mpegg-decoder-p2"
lae="../cmake-build-debug/bin/local-assembly-encoder"

declare -a files=("../data/mpeg-g/conformance/gen-011/gen-011" "../data/mpeg-g/conformance/gen-012/gen-012")
declare -a au=("U" "P" "N" "M" "I")

printf "\n\n*** Testing: local assembly ***\n"

for file in "${files[@]}"
do
    for unit in "${au[@]}"
    do
        #$lae --input-file $file.sam -o $file.$unit.mbg -t $unit
        #$reference_sw -v debug -i $file.$unit.mbg -o $file.$unit.mgrecs
        ./single_conformance_test.sh lae $file.sam $file.$unit.mgrecs "-t $unit"
        if [ $? -ne 0 ]; then
            exit -1
        fi
    done
done

printf "\n\n*** Success: local assembly ***\n"
