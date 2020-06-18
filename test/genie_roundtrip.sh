#!/bin/bash
# Executes the roundtrip genie encoder -> reference decoder -> genie converter
# No validation of the output file yet, only crashes and error codes are tested for now

INPUT_FILE_FIRST=$1
INPUT_FILE_SECOND=$2
TIME_PATH="/bin/time"


TIME_CMD="${TIME_PATH} -f \"time result\ncmd:%C\nreal %es\nuser %Us \nsys  %Ss \nmemory:%MKB \ncpu %P\""

if [[ $# -eq 0 ]]
then
    echo "Usage: GENIE_PATH=[Path to genie executable] genie_roundtrip.sh [input file] [optional supplementary / paired file]"
    echo "Supported file types are fastq and sam."
    exit -1
fi

if ! [[ -x ${GENIE_PATH} ]]
then
    echo "GENIE_PATH environment variable not set or invalid"
    exit -1
else
    COMMAND_ENCODE="${TIME_CMD} ${GENIE_PATH} run"
    COMMAND_DECODE="${TIME_CMD} ${GENIE_PATH} run"
    COMMAND_ENCODE2="${TIME_CMD} ${GENIE_PATH} run"
    COMMAND_DECODE2="${TIME_CMD} ${GENIE_PATH} run"
fi

if ! [[ -f ${INPUT_FILE_FIRST} ]]
then 
    echo " First input file not found"
    exit -1
else

    filename=$(basename -- "$INPUT_FILE_FIRST")
    INPUT_FILE_FIRST_EXT="${filename##*.}"
    INPUT_FILE_FIRST="${filename%.*}"

    COMMAND_ENCODE="${COMMAND_ENCODE} -i ${INPUT_FILE_FIRST}.${INPUT_FILE_FIRST_EXT} -o ${INPUT_FILE_FIRST}_encoded.mgb"
    COMMAND_DECODE="${COMMAND_DECODE} -o ${INPUT_FILE_FIRST}_decoded.${INPUT_FILE_FIRST_EXT} -i ${INPUT_FILE_FIRST}_encoded.mgb"
    COMMAND_ENCODE2="${COMMAND_ENCODE2} -i ${INPUT_FILE_FIRST}_decoded.${INPUT_FILE_FIRST_EXT} -o ${INPUT_FILE_FIRST}_encoded2.mgb"
    COMMAND_DECODE2="${COMMAND_DECODE2} -o ${INPUT_FILE_FIRST}_decoded2.${INPUT_FILE_FIRST_EXT} -i ${INPUT_FILE_FIRST}_encoded2.mgb"
fi

if [[ -f "${INPUT_FILE_FIRST}_encoded.mgb" ]]
then
    echo "${INPUT_FILE_FIRST}_encoded.mgb already existent"
    exit -1
fi

if [[ -f "${INPUT_FILE_FIRST}_decoded.${INPUT_FILE_FIRST_EXT}" ]]
then
    echo "${INPUT_FILE_FIRST}_decoded.${INPUT_FILE_FIRST_EXT} already existent"
    exit -1
fi

if [[ -f "${INPUT_FILE_FIRST}_encoded2.mgb" ]]
then
    echo "${INPUT_FILE_FIRST}_encoded2.mgb already existent"
    exit -1
fi

if [[ -f "${INPUT_FILE_FIRST}_decoded2.${INPUT_FILE_FIRST_EXT}" ]]
then
    echo "${INPUT_FILE_FIRST}_decoded2.${INPUT_FILE_FIRST_EXT} already existent"
    exit -1
fi

if [[ $# -ge 2 ]]
then
    if [[ ! -f ${INPUT_FILE_SECOND} ]] 
    then 
        echo "Second input file not found"
        exit -1
    else

        filename=$(basename -- "$INPUT_FILE_SECOND")
        INPUT_FILE_SECOND_EXT="${filename##*.}"
        INPUT_FILE_SECOND="${filename%.*}"

        COMMAND_ENCODE="${COMMAND_ENCODE} --input-suppl-file ${INPUT_FILE_SECOND}.${INPUT_FILE_SECOND_EXT}"
        COMMAND_DECODE="${COMMAND_DECODE} --output-suppl-file ${INPUT_FILE_SECOND}_decoded.${INPUT_FILE_SECOND_EXT}"
        COMMAND_ENCODE2="${COMMAND_ENCODE2} --input-suppl-file ${INPUT_FILE_SECOND}_decoded.${INPUT_FILE_SECOND_EXT}"
        COMMAND_DECODE2="${COMMAND_DECODE2} --output-suppl-file ${INPUT_FILE_SECOND}_decoded2.${INPUT_FILE_SECOND_EXT}"

        if [[ -f "${INPUT_FILE_SECOND}_decoded.${INPUT_FILE_SECOND_EXT}" ]]
        then
            echo "${INPUT_FILE_SECOND}_decoded.${INPUT_FILE_SECOND_EXT} already existent"
            exit -1
        fi
        if [[ -f "${INPUT_FILE_SECOND}_decoded2.${INPUT_FILE_SECOND_EXT}" ]]
        then
            echo "${INPUT_FILE_SECOND}_decoded2.${INPUT_FILE_SECOND_EXT} already existent"
            exit -1
        fi
    fi
fi

echo ${COMMAND_ENCODE}
eval "${COMMAND_ENCODE}"
if ! [[ $? -eq 0 ]]
then
   echo "Aborting test..."
   exit -1
fi
echo ${COMMAND_DECODE}
eval "${COMMAND_DECODE}"
if ! [[ $? -eq 0 ]]
then
   echo "Aborting test..."
   exit -1
fi
echo ${COMMAND_ENCODE2}
eval "${COMMAND_ENCODE2}"
if ! [[ $? -eq 0 ]]
then
   echo "Aborting test..."
   exit -1
fi
echo ${COMMAND_DECODE2}
eval "${COMMAND_DECODE2}"
if ! [[ $? -eq 0 ]]
then
   echo "Aborting test..."
   exit -1
fi

echo "Success!"
