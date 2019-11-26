#!/usr/bin/env bash

declare -a files=("../data/fastq/fourteen-records")
declare -a pairs=("../data/fastq/fourteen-records-pair")

printf "\n\n*** Testing: local assembly ***\n"

for ((i=0;i<${#files[@]};++i)); do
    printf "%s is in %s\n" "${files[i]}" "${pairs[i]}"
    ./single_conformance_test.sh genie "${pairs[i]}.fastq" "${files[i]}.mgrecs" "${files[i]}.fastq"
    if [ $? -ne 0 ]; then
    	exit -1
    fi
done

printf "\n\n*** Success: local spring ***\n"
