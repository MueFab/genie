#!/bin/bash
# Executes the roundtrip genie encoder -> genie decoder -> genie encoder -> genie decoder
# No validation of the output file yet, only crashes and error codes are tested for now


INPUT_FILE_FIRST=$1
INPUT_FILE_SECOND=$2
TIME_PATH="/bin/time"

TIME_CMD="${TIME_PATH} -f \"time result\ncmd:%C\nreal %es\nuser %Us \nsys  %Ss \nmemory:%MKB \ncpu %P\""

# Check parameters and environment

if ! [[ -x ${GENIE_PATH} ]]
then
    echo "GENIE_PATH environment variable not set or invalid"
    exit -1
else
    COMMAND_ENCODE="${TIME_CMD} ${GENIE_PATH} run --read-ids none --qv none"
    COMMAND_CONVERT="${TIME_CMD} ${GENIE_PATH} run"
fi

if ! [[ -x ${REFCODER_PATH} ]]
then
    echo "REFCODER_PATH environment variable not set or invalid"
    exit -1
else
    COMMAND_DECODE="${TIME_CMD} ${REFCODER_PATH}"
fi

# Build commands for first input file

if ! [[ -f ${INPUT_FILE_FIRST} ]]
then 
    echo " First input file not found"
    exit -1
else

    filename=$(basename -- "$INPUT_FILE_FIRST")
    INPUT_FILE_FIRST_EXT="${filename##*.}"
    INPUT_FILE_FIRST="${filename%.*}"

    COMMAND_ENCODE="${COMMAND_ENCODE} -i ${INPUT_FILE_FIRST}.${INPUT_FILE_FIRST_EXT} -o ${INPUT_FILE_FIRST}_encoded.mgb"
    COMMAND_DECODE="${COMMAND_DECODE} -o ${INPUT_FILE_FIRST}_decoded.mgrec -i ${INPUT_FILE_FIRST}_encoded.mgb"
    COMMAND_CONVERT="${COMMAND_CONVERT} -i ${INPUT_FILE_FIRST}_decoded.mgrec -o ${INPUT_FILE_FIRST}_decoded.${INPUT_FILE_FIRST_EXT}"
fi

if [[ -f "${INPUT_FILE_FIRST}_encoded.mgb" ]]
then
    echo "${INPUT_FILE_FIRST}_encoded.mgb already existent"
    exit -1
fi

if [[ -f "${INPUT_FILE_FIRST}_decoded.mgrec" ]]
then
    echo "${INPUT_FILE_FIRST}_decoded.mgrec already existent"
    exit -1
fi

if [[ -f "${INPUT_FILE_FIRST}_decoded.${INPUT_FILE_FIRST_EXT}" ]]
then
    echo "${INPUT_FILE_FIRST}_decoded.${INPUT_FILE_FIRST_EXT} already existent"
    exit -1
fi

# Build commands for second input file

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
        COMMAND_CONVERT="${COMMAND_CONVERT} --output-suppl-file ${INPUT_FILE_SECOND}_decoded.${INPUT_FILE_SECOND_EXT}"

        if [[ -f "${INPUT_FILE_SECOND}_decoded.${INPUT_FILE_SECOND_EXT}" ]]
        then
            echo "${INPUT_FILE_SECOND}_decoded.${INPUT_FILE_SECOND_EXT} already existent"
            exit -1
        fi

    fi
fi

# Execute

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
echo ${COMMAND_CONVERT}
eval "${COMMAND_CONVERT}"
if ! [[ $? -eq 0 ]]
then
   echo "Aborting test..."
   exit -1
fi

echo "Success!"
