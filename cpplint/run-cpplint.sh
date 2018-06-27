#!/bin/bash

if [ "$#" -ne 0 ]; then
    printf "Usage: $0 (without any arguments)\n";
    exit -1;
fi

source_folder="../source"
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

files=()
files+=("$source_folder/dsg/dsg.cc")
files+=("$source_folder/dsg/generation.h")
files+=("$source_folder/dsg/generation.cc")
files+=("$source_folder/dsg/ProgramOptions.h")
files+=("$source_folder/dsg/ProgramOptions.cc")
files+=("$source_folder/dsg/common/constants.h")
files+=("$source_folder/dsg/common/exceptions.h")
files+=("$source_folder/dsg/common/exceptions.cc")
files+=("$source_folder/dsg/common/operating-system.h")
files+=("$source_folder/dsg/common/return-codes.h")
files+=("$source_folder/dsg/common/utilities.h")
files+=("$source_folder/dsg/common/utilities.cc")
files+=("$source_folder/dsg/input/FileReader.h")
files+=("$source_folder/dsg/input/FileReader.cc")
files+=("$source_folder/dsg/input/fasta/FastaFileReader.h")
files+=("$source_folder/dsg/input/fasta/FastaFileReader.cc")
files+=("$source_folder/dsg/input/fasta/FastaRecord.h")
files+=("$source_folder/dsg/input/fasta/FastaRecord.cc")
files+=("$source_folder/dsg/input/fastq/FastqFileReader.h")
files+=("$source_folder/dsg/input/fastq/FastqFileReader.cc")
files+=("$source_folder/dsg/input/fastq/FastqRecord.h")
files+=("$source_folder/dsg/input/fastq/FastqRecord.cc")
files+=("$source_folder/dsg/input/sam/SamFileReader.h")
files+=("$source_folder/dsg/input/sam/SamFileReader.cc")
files+=("$source_folder/dsg/input/sam/SamRecord.h")
files+=("$source_folder/dsg/input/sam/SamRecord.cc")

for file in "${files[@]}"; do
    printf "Running cpplint on: $file\n";

    $python $cpplint_py $file;

    printf "\n";
done

