//
// Created by bscuser on 19/02/18.
//

#include <utils.h>
#include <DataStructures/BitStreams/OutputBitstream.h>
#include <DataStructures/BitStreams/InputBitstream.h>
#include "DataUnits.h"

bool writeParametersSet(ParametersSet *parametersSet, FILE* outputFile){
    bool typeSuccessfulWrite = write(1,outputFile);
    bool parametersSetSuccessfulWrite = writeBigEndian32ToFile(8, outputFile);
    OutputBitstream outputBitstream;
    if(typeSuccessfulWrite || parametersSetSuccessfulWrite || initializeOutputBitstream(&outputBitstream, outputFile)) {
        bool parent_parameter_setIdSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian16(&outputBitstream, 12, parametersSet->parent_parameter_setId);
        bool parameter_setIdSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian16(&outputBitstream, 12, parametersSet->parameter_setId);
        bool bufferWriteSuccessful = writeBuffer(&outputBitstream);
        if(
            !parent_parameter_setIdSuccessfulWrite ||
            !parameter_setIdSuccessfulWrite ||
            !bufferWriteSuccessful
        ){
            fprintf(stderr, "Error writing parameters set.\n");
        }
    }else{
        fprintf(stderr, "Error writing parameters set.\n");
    }
}

ParametersSet* parseParametersSet(FILE* inputFile, uint64_t size){
    InputBitstream inputBitstream;
    if(initializeInputBitstream(&inputBitstream, inputFile)){
        uint16_t parent_parameter_setId, parameter_setId;
        bool parent_parameter_setIdSuccessfulRead =
                readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 12, (char*)&parent_parameter_setId);
        bool parameter_setIdSuccessfulRead =
                readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 12, (char*)&parameter_setId);
        if (parent_parameter_setIdSuccessfulRead && parameter_setIdSuccessfulRead){
            size -= 24/8;
            char* buffer = (char*)malloc(size * sizeof(char*));
            readBytes(&inputBitstream, (uint32_t) size, buffer);
            return initParametersSetWithBuffer(
                parent_parameter_setId,
                parameter_setId,
                buffer,
                (uint32_t) size
            );
        }else{
            fprintf(stderr, "Error reading parameters set IDs.\n");
            return NULL;
        }

    }else{
        fprintf(stderr, "Error opening input bitstream to read parameters.\n");
        return NULL;
    }



}


ParametersSet * initParametersSetWithBuffer(
        uint16_t parent_parameter_setId,
        uint16_t parameter_setId,
        char* buffer,
        uint32_t buffer_size
){
    ParametersSet* parametersSet = (ParametersSet*)malloc(sizeof(ParametersSet));
    if(parametersSet == NULL){
        fprintf(stderr, "Error allocating ParametersSet.\n");
        return parametersSet;
    }
    parametersSet->parent_parameter_setId = parent_parameter_setId;
    parametersSet->parameter_setId = parameter_setId;
    parametersSet->buffer_size = buffer_size;
    parametersSet->buffer = buffer;
    return parametersSet;
}

ParametersSet * initParametersSet(uint16_t parent_parameter_setId, uint16_t parameter_setId){
    ParametersSet* parametersSet = (ParametersSet*)malloc(sizeof(ParametersSet));
    if(parametersSet == NULL){
        fprintf(stderr, "Error allocating ParametersSet.\n");
        return parametersSet;
    }
    parametersSet->parent_parameter_setId = parent_parameter_setId;
    parametersSet->parameter_setId = parameter_setId;
    parametersSet->buffer_size = 0;
    parametersSet->buffer = NULL;
    return parametersSet;
}

void freeDataUnitsParametersSet(ParametersSet* parameters){
    free(parameters);
}

