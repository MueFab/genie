#!/bin/bash 
set -e

if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters. Usage: $ ./benchmark.sh [path_to_raw_streams_directory]"
    exit 1
fi




git_root_dir="$(git rev-parse --show-toplevel)"
genie="$git_root_dir/cmake-build-release/bin/genie"

working_dir="${1}"

if [ ! -x $genie ]; then
    echo "Could not detect genie executable."
    exit 1
fi


files=(${working_dir}/rawstream_*)

if [ ! -f "${files[0]}" ]; then
    echo "No raw streams detected!"
    exit 1
fi

cd $working_dir

for f in rawstream*
do
    access_unit=`echo $f | awk -F'[_]' '{print $2}'`
    descriptor=`echo $f | awk -F'[_]' '{print $3}'`
    subdescriptor=`echo $f | awk -F'[_]' '{print $4}'`
    echo $access_unit $descriptor $subdescriptor
    $genie gabac -t benchmark -i $f -o "config_$f.json" -d $descriptor -s $subdescriptor
done

