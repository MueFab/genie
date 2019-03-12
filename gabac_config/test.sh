#!/bin/bash
for filename in ./*.0; do
    /home/fabian/Documents/dev/gabac/build/bin/gabacify encode -i ${filename} -c ${filename}.json -o ${filename}.out
    /home/fabian/Documents/dev/gabac/build/bin/gabacify decode -i ${filename}.out -c ${filename}.json -o ${filename}.out.decoded
    diff ${filename} ${filename}.out.decoded >> log.txt
done
