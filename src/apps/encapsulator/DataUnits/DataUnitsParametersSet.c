//
// Created by bscuser on 19/02/18.
//

#include "../DataStructures/BitStreams/InputBitstream.h"
#include "../DataStructures/BitStreams/OutputBitstream.h"
#include "../utils.h"
#include "DataUnits.h"

bool parseDataUnitsParametersSet(DataUnitParametersSet** parametersSet, FILE* inputFile, uint32_t sizeContent) {
    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    uint8_t parent_parameter_set_id = 0;
    uint8_t parameter_set_id = 0;
    if (!readNBitsShift(&inputBitstream, 8, (char*)&parent_parameter_set_id) ||
        !readNBitsShift(&inputBitstream, 8, (char*)&parameter_set_id)) {
        return false;
    }

    Encoding_ParametersType* encoding_parameters = readEncodingParameters(&inputBitstream);
    if (encoding_parameters == NULL) {
        fprintf(stderr, "Encoding parameters could not be read.\n");
        return NULL;
    }

    *parametersSet = initParametersSet(parent_parameter_set_id, parameter_set_id, encoding_parameters);

    return true;
}

bool writeParametersSet(DataUnitParametersSet* parametersSet, FILE* outputFile) {
    bool typeSuccessfulWrite = utils_write(1, outputFile);
    bool parametersSetSuccessfulWrite = writeBigEndian32ToFile(
        (uint32_t)(5 + 2 + getSizeEncodingParameters(parametersSet->encodingParameters)), outputFile);
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);
    if (typeSuccessfulWrite || parametersSetSuccessfulWrite ||
        initializeOutputBitstream(&outputBitstream, outputFile)) {
        bool parent_parameter_setIdSuccessfulWrite =
            writeNBitsShift(&outputBitstream, 8, (char*)&parametersSet->parent_parameter_setId);
        bool parameter_setIdSuccessfulWrite =
            writeNBitsShift(&outputBitstream, 8, (char*)&parametersSet->parameter_setId);

        writeEncoding_parameters(parametersSet->encodingParameters, &outputBitstream);
        writeBuffer(&outputBitstream);
        return true;
    } else {
        fprintf(stderr, "Error writing parameters set.\n");
        return false;
    }
}

DataUnitParametersSet* initParametersSet(uint16_t parent_parameter_setId, uint16_t parameter_setId,
                                         Encoding_ParametersType* encoding_parameters) {
    DataUnitParametersSet* parametersSet = (DataUnitParametersSet*)malloc(sizeof(DataUnitParametersSet));
    if (parametersSet == NULL) {
        fprintf(stderr, "Error allocating DataUnitParametersSet.\n");
        return parametersSet;
    }
    parametersSet->parent_parameter_setId = parent_parameter_setId;
    parametersSet->parameter_setId = parameter_setId;
    parametersSet->encodingParameters = encoding_parameters;

    return parametersSet;
}

void freeDataUnitsParametersSet(DataUnitParametersSet* parameters) {
    freeEncodingParameters(parameters->encodingParameters);
    free(parameters);
}
