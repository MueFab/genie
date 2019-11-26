#!/usr/bin/env bash

reference_sw="../../mpegg-reference-sw/bin/decoder/mpegg-decoder-p2"
lae="../cmake-build-debug/bin/local-assembly-encoder"
genie="../cmake-build-debug/bin/genie"

tool=$1
in=$2
expected_out=$3
args=$4

printf "\n***** Testing: $in *****\n"

if [ "$#" -lt 3 ]; then
    echo "Illegal number of parameters"
    echo "Usage: single_conformance_test.sh [tool(lae, ure, genie)], [input_file], [expected_output_file] \"[optional, additional arguments]\""
    exit -1
fi

function mytest {
    "$@"
    local status=$?
    if [ $status -ne 0 ]; then
        printf "\nerror with $1\n" >&2
	exit $status
    fi
    return $status
}

if [ "$tool" = "lae" ]; then
    mytest $lae $args -i $in -o $in.mbg
elif [ "$tool" = "genie" ]; then
    mytest $genie $args $in -o $in.mbg
    rm genie.log
elif [ "$tool" = "ure" ]; then
    mytest $ure -i $in -o $in.mbg
else
    echo "Unknown tool $tool" >&2
    exit -1
fi
mytest $reference_sw -i $in.mbg -o $in.mbg.mgrecs
mytest diff $in.mbg.mgrecs $expected_out
mytest rm $in.mbg
mytest rm $in.mbg.mgrecs

printf "***** Success: $in *****\n"
