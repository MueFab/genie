#!/bin/bash
reference_sw="../../../mpegg-reference-sw/bin/decoder/mpegg-decoder-p2"
lae="../../cmake-build-debug/bin/local-assembly-encoder"

declare -a files=("../../data/mpeg-g/conformance/gen-011/gen-011" "../../data/mpeg-g/conformance/gen-012/gen-012")
declare -a au=("U" "P" "N" "M" "I")

for file in "${files[@]}"
do
    for unit in "${au[@]}"
    do
        $lae --input-file $file.sam -o $file.$unit.mpegg -t $unit
	$reference_sw -v debug -i $file.$unit.mpegg -o $file.$unit.mgr
    done
done

