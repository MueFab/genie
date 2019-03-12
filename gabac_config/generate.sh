#!/bin/bash
for filename in ./*.0; do
    echo "********s${filename}*********"
    /home/fabian/Documents/dev/gabac/build/bin/gabacify analyze -i ${filename} -c ${filename}.json
done
