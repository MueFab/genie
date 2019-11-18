//
// Created by gencom on 27/07/17.
//

#include "../../Boxes.h"
#include "../../DataStructures/BitStreams/InputBitstream.h"
#include "../../DataStructures/BitStreams/OutputBitstream.h"
#include "../../utils.h"

DatasetParameters* initDatasetParameters(uint8_t dataset_group_ID, uint16_t dataset_ID,
                                         uint16_t parent_parameter_set_ID, uint16_t parameter_set_ID) {
    DatasetParameters* datasetParameters = (DatasetParameters*)malloc(sizeof(DatasetParameters));
    datasetParameters->dataset_group_ID = dataset_group_ID;
    datasetParameters->dataset_ID = dataset_ID;
    datasetParameters->parent_parameter_set_ID = parent_parameter_set_ID;
    datasetParameters->parameter_set_ID = parameter_set_ID;

    datasetParameters->encoding_parameters = NULL;
    datasetParameters->hasSeek = false;
    datasetParameters->seekPosition = 0;
    return datasetParameters;
}

DatasetParameters* initDatasetParametersWithParameters(uint8_t dataset_group_ID, uint16_t dataset_ID,
                                                       uint16_t parent_parameter_set_ID, uint16_t parameter_set_ID,
                                                       Encoding_ParametersType* encoding_parameters) {
    DatasetParameters* datasetParameters = (DatasetParameters*)malloc(sizeof(DatasetParameters));
    datasetParameters->dataset_group_ID = dataset_group_ID;
    datasetParameters->dataset_ID = dataset_ID;
    datasetParameters->parent_parameter_set_ID = parent_parameter_set_ID;
    datasetParameters->parameter_set_ID = parameter_set_ID;

    datasetParameters->encoding_parameters = encoding_parameters;
    datasetParameters->hasSeek = false;
    datasetParameters->seekPosition = 0;
    return datasetParameters;
}

void freeDatasetParameters(DatasetParameters* datasetParameters) {
    freeEncodingParameters(datasetParameters->encoding_parameters);
    free(datasetParameters);
}

bool writeDatasetParameters(DatasetParameters* datasetParameters, FILE* outputFile) {
    bool typeWriteSuccessfulWrite = fwrite(datasetParametersName, sizeof(char), 4, outputFile) == 4;
    uint64_t sizeDatasetParameters = getSizeDatasetParameters(datasetParameters);
    uint64_t sizeDatasetParametersBigEndian = nativeToBigEndian64(sizeDatasetParameters);
    bool sizeSuccessfulWrite = fwrite(&sizeDatasetParametersBigEndian, sizeof(uint64_t), 1, outputFile) == 1;
    if (!typeWriteSuccessfulWrite || !sizeSuccessfulWrite) {
        fprintf(stderr, "Error writing type or size of dataset parameters.\n");
        return false;
    }
    return writeContentDatasetParameters(datasetParameters, outputFile);
}

bool writeContentDatasetParameters(DatasetParameters* datasetParameters, FILE* outputFile) {
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);
    writeToBitstream(&outputBitstream, datasetParameters->dataset_group_ID);
    writeBigEndian16ToBitstream(&outputBitstream, datasetParameters->dataset_ID);
    writeNBitsShift(&outputBitstream, 8, (char*)&datasetParameters->parent_parameter_set_ID);
    writeNBitsShift(&outputBitstream, 8, (char*)&datasetParameters->parameter_set_ID);

    writeEncoding_parameters(datasetParameters->encoding_parameters, &outputBitstream);
    writeBuffer(&outputBitstream);

    return true;
}

uint64_t getSizeContentDatasetParameters(DatasetParameters* datasetParameters) {
    uint64_t contentSize = 0;
    contentSize += 1;  // dataset_group_ID
    contentSize += 2;  // dataset_ID
    contentSize += 1;  // parameter_set_ID
    contentSize += 1;  // parent_parameter_set_ID
    contentSize += getSizeEncodingParameters(datasetParameters->encoding_parameters);

    return contentSize;
}

uint64_t getSizeDatasetParameters(DatasetParameters* datasetParameters) {
    return BOX_HEADER_SIZE + getSizeContentDatasetParameters(datasetParameters);
}

DatasetParameters* parseDatasetParameters(uint64_t boxContentSize, FILE* inputFile) {
    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    uint8_t dataset_group_ID;
    uint16_t dataset_ID;
    uint16_t parent_parameter_set_ID;
    uint16_t parameter_set_ID;

    readBytes(&inputBitstream, 1, (char*)&dataset_group_ID);
    readNBitsBigToNativeEndian16(&inputBitstream, 16, &dataset_ID);
    readNBitsBigToNativeEndian16(&inputBitstream, 8, &parent_parameter_set_ID);
    readNBitsBigToNativeEndian16(&inputBitstream, 8, &parameter_set_ID);

    Encoding_ParametersType* encodingParameters = readEncodingParameters(&inputBitstream);

    uint64_t encodingParametersSize = boxContentSize - 1 - 2 - 2 - 10;
    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not get file position.\n");
        return NULL;
    }

    DatasetParameters* datasetParameters =
        initDatasetParameters(dataset_group_ID, dataset_ID, parent_parameter_set_ID, parameter_set_ID);
    datasetParameters->hasSeek = true;
    datasetParameters->seekPosition = (size_t)seekPosition;
    datasetParameters->encoding_parameters = encodingParameters;

    return datasetParameters;
}

size_t getDatasetParametersSeekPosition(DatasetParameters* datasetParameters) {
    return datasetParameters->seekPosition;
}