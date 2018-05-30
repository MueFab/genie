#!/bin/bash

if [ "$#" -ne 0 ]; then
    printf "Usage: $0 (without any arguments)\n";
    exit -1;
fi

source_folder="../source/"
if [ ! -d $source_folder ]; then
    printf "Error: File $source_folder is not a directory.\n";
    exit -1;
fi

cpplint_cfg="../source/CPPLINT.cfg"
if [ ! -f $cpplint_cfg ]; then
    printf "Error: File $cpplint_cfg is not a regular file.\n";
    exit -1;
fi

python="/usr/bin/python"
if [ ! -x $python ]; then
    printf "Error: Binary file $python is not executable.\n";
    exit -1;
fi

cpplint_py="cpplint.py"
if [ ! -f $cpplint_py ]; then
    printf "Error: Python script $cpplint_py is not a regular file.\n";
    exit -1;
fi

find="/usr/bin/find"
if [ ! -x $find ]; then
    printf "Error: Binary file $find is not executable.\n";
    exit -1;
fi

find_options=" -type f "
find_options+=" -name *.c "
find_options+=" -or -name *.cc "
find_options+=" -or -name *.cpp "
find_options+=" -or -name *.h "
find_options+=" -or -name *.hpp "

source_files=`$find $source_folder $find_options`

for source_file in $source_files; do
    printf "\n";
    printf "Running cpplint on: $source_file\n";
#     printf "\n";

    $python $cpplint_py $source_file;
done
