#!/usr/bin/env bash

declare -a files=("../data/mpeg-g/conformance/gen-011/gen-011" "../data/mpeg-g/conformance/gen-012/gen-012")
declare -a au=("U" "P" "N" "M" "I")

printf "\n\n*** Testing: local assembly ***\n"

for file in "${files[@]}"
do
    for unit in "${au[@]}"
    do
        ./single_conformance_test.sh lae $file.sam $file.$unit.mgrecs "-t $unit"
        if [ $? -ne 0 ]; then
            exit -1
        fi
    done
done

printf "\n\n*** Success: local assembly ***\n"
