//
// Created by bscuser on 19/02/18.
//

#include <utils.h>
#include <DataStructures/BitStreams/OutputBitstream.h>
#include <DataStructures/BitStreams/InputBitstream.h>
#include "DataUnits.h"

bool parseParametersSet(ParametersSet** parametersSet, FILE* inputFile, uint32_t sizeContent){
    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    uint8_t parent_parameter_set_id = 0;
    uint8_t parameter_set_id = 0;
    if(
        !readNBitsShift(&inputBitstream, 8, (char*)&parent_parameter_set_id) ||
        !readNBitsShift(&inputBitstream, 8, (char*)&parameter_set_id)
    ){
        return false;
    }

    uint32_t sizeParameters = sizeContent - 2;

    uint8_t dataset_type;
    uint8_t alphabet_ID;
    uint32_t reads_length;
    uint8_t number_of_template_segments_minus1;
    uint8_t bufferReserved;
    uint32_t max_au_data_unit_size;
    uint8_t pos_40_bits;
    uint8_t qv_depth;
    uint8_t as_depth;

    if(
        !readNBitsShift(&inputBitstream, 4, (char*)&dataset_type) ||
        !readNBitsShift(&inputBitstream, 8, (char*)&alphabet_ID) ||
        !readNBitsBigToNativeEndian32(&inputBitstream, 24, &reads_length) ||
        !readNBitsShift(&inputBitstream, 2, (char*)&number_of_template_segments_minus1) ||
        !readNBitsShift(&inputBitstream, 6, (char*)&bufferReserved) ||
        !readNBitsBigToNativeEndian32(&inputBitstream, 29, &max_au_data_unit_size) ||
        !readBit(&inputBitstream, &pos_40_bits) ||
        !readNBitsShift(&inputBitstream, 3, (char*)&qv_depth) ||
        !readNBitsShift(&inputBitstream, 3, (char*)&as_depth)
    ){
        fprintf(stderr, "Error writing parameters set.\n");
        return false;
    }

    uint32_t remainingSize = sizeParameters - 10;
    char* buffer= (char*)malloc(sizeof(char)*(remainingSize));
    if(buffer == NULL){
        return false;
    }
    if(!readBytes(&inputBitstream, remainingSize, buffer)){
        free(buffer);
    }


    *parametersSet = initParametersSet(
        parent_parameter_set_id,
        parameter_set_id,
        dataset_type,
        alphabet_ID,
        reads_length,
        number_of_template_segments_minus1,
        max_au_data_unit_size,
        pos_40_bits,
        qv_depth,
        as_depth,
        initByteArrayPassData(remainingSize, (Byte *) buffer)
    );

    if(*parametersSet == NULL){
        free(buffer);
        return false;
    }
    return true;
}

bool writeParametersSet(ParametersSet *parametersSet, FILE* outputFile){
    bool typeSuccessfulWrite = utils_write(1,outputFile);
    bool parametersSetSuccessfulWrite = writeBigEndian32ToFile(
            (uint32_t) (8 + 9 + parametersSet->data->size),
            outputFile
    );
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);
    if(typeSuccessfulWrite || parametersSetSuccessfulWrite || initializeOutputBitstream(&outputBitstream, outputFile)) {
        bool parent_parameter_setIdSuccessfulWrite = writeNBitsShift(&outputBitstream, 8, (char*)&parametersSet->parent_parameter_setId);
        bool parameter_setIdSuccessfulWrite = writeNBitsShift(&outputBitstream, 8, (char*)&parametersSet->parameter_setId);

        bool datasetTypeSuccessfulWrite = writeNBitsShift(&outputBitstream, 4, (char*)&parametersSet->dataset_type);
        bool alphabetIdSuccessfulWrite = writeNBitsShift(&outputBitstream, 8, (char*)&parametersSet->alphabet_ID);
        bool readsLengthSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32_new(
                &outputBitstream, 24, parametersSet->reads_length
        );
        bool numberTemplateSuccessfulWrite = writeNBitsShift(
                &outputBitstream, 2,
                (char*)&parametersSet->number_of_template_segments_minus1
        );
        uint8_t bufferReserved = 0;
        bool bufferedReservedSuccessfulWrite = writeNBitsShift(
                &outputBitstream, 6,
                (char*)&bufferReserved
        );
        bool maxAuDataUnitSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32_new(
                &outputBitstream, 29,
                parametersSet->max_au_data_unit_size
        );
        bool pos40BitsSuccessfulWrite = writeBit(
                &outputBitstream,
                (uint8_t) (parametersSet->pos_40_bits ? 1 : 0)
        );
        bool qvDepthSuccessfulWrite = writeNBitsShift(
                &outputBitstream,
                3, (char*)&parametersSet->qv_depth
        );
        bool asDepthSuccessfulWrite = writeNBitsShift(
                &outputBitstream,
                3, (char*)&parametersSet->as_depth
        );

        bool bufferWriteSuccessful = writeBuffer(&outputBitstream);
        if(
            !parent_parameter_setIdSuccessfulWrite ||
            !parameter_setIdSuccessfulWrite ||
            !datasetTypeSuccessfulWrite ||
            !alphabetIdSuccessfulWrite ||
            !readsLengthSuccessfulWrite ||
            !numberTemplateSuccessfulWrite ||
            !bufferedReservedSuccessfulWrite ||
            !maxAuDataUnitSuccessfulWrite  ||
            !pos40BitsSuccessfulWrite ||
            !qvDepthSuccessfulWrite ||
            !asDepthSuccessfulWrite ||
            !bufferWriteSuccessful
        ){
            fprintf(stderr, "Error writing parameters set.\n");
            return false;
        }
        if(!writeByteArray(parametersSet->data, outputFile)){
            fprintf(stderr, "Error writing encoding parameters.\n");
            return false;
        }
        return true;
    }else{
        fprintf(stderr, "Error writing parameters set.\n");
        return false;
    }
}

ParametersSet * initParametersSet(
        uint16_t parent_parameter_setId,
        uint16_t parameter_setId,
        uint8_t dataset_type,
        uint8_t alphabet_ID,
        uint32_t reads_length,
        uint8_t number_of_template_segments_minus1,
        uint32_t max_au_data_unit_size,
        bool pos_40_bits,
        uint8_t qv_depth,
        uint8_t as_depth,
        ByteArray* data
){
    ParametersSet* parametersSet = (ParametersSet*)malloc(sizeof(ParametersSet));
    if(parametersSet == NULL){
        fprintf(stderr, "Error allocating ParametersSet.\n");
        return parametersSet;
    }
    parametersSet->parent_parameter_setId = parent_parameter_setId;
    parametersSet->parameter_setId = parameter_setId;

    parametersSet->dataset_type = dataset_type;
    parametersSet->alphabet_ID = alphabet_ID;
    parametersSet->reads_length = reads_length;
    parametersSet->number_of_template_segments_minus1 = number_of_template_segments_minus1;
    parametersSet->max_au_data_unit_size = max_au_data_unit_size;
    parametersSet->pos_40_bits = pos_40_bits;
    parametersSet->qv_depth = qv_depth;
    parametersSet->as_depth = as_depth;

    parametersSet->data = cloneByteArray(data);
    return parametersSet;
}

void freeDataUnitsParametersSet(ParametersSet* parameters){
    freeByteArray(parameters->data);
    free(parameters);
}

