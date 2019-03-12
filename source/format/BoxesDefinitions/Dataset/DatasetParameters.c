//
// Created by gencom on 27/07/17.
//

#include <DataStructures/BitStreams/OutputBitstream.h>
#include <DataStructures/BitStreams/InputBitstream.h>
#include "../../Boxes.h"
#include "../../utils.h"


DatasetParameters *initDatasetParameters(
    uint8_t dataset_group_ID,
    uint16_t dataset_ID,
    uint16_t parent_parameter_set_ID,
    uint16_t parameter_set_ID
) {
    DatasetParameters* datasetParameters = (DatasetParameters*) malloc(sizeof(DatasetParameters));
    datasetParameters->dataset_group_ID = dataset_group_ID;
    datasetParameters->dataset_ID = dataset_ID;
    datasetParameters->parent_parameter_set_ID = parent_parameter_set_ID;
    datasetParameters->parameter_set_ID = parameter_set_ID;

    datasetParameters->parameters = NULL;
    datasetParameters->hasSeek = false;
    datasetParameters->seekPosition = 0;
    return datasetParameters;
}

DatasetParameters *initDatasetParametersWithParameters(
        uint8_t dataset_group_ID,
        uint16_t dataset_ID,
        uint16_t parent_parameter_set_ID,
        uint16_t parameter_set_ID,
        uint8_t dataset_type,
        uint8_t alphabet_ID,
        uint32_t reads_length,
        uint8_t number_of_template_segments_minus1,
        uint32_t max_au_data_unit_size,
        bool pos_40_bits,
        uint8_t qv_depth,
        uint8_t as_depth,
        ByteArray* parameters
) {
    DatasetParameters* datasetParameters = (DatasetParameters*) malloc(sizeof(DatasetParameters));
    datasetParameters->dataset_group_ID = dataset_group_ID;
    datasetParameters->dataset_ID = dataset_ID;
    datasetParameters->parent_parameter_set_ID = parent_parameter_set_ID;
    datasetParameters->parameter_set_ID = parameter_set_ID;

    datasetParameters->dataset_type = dataset_type;
    datasetParameters->alphabet_ID = alphabet_ID;
    datasetParameters->reads_length = reads_length;
    datasetParameters->number_of_template_segments_minus1 = number_of_template_segments_minus1;
    datasetParameters->max_au_data_unit_size = max_au_data_unit_size;
    datasetParameters->pos_40_bits = pos_40_bits;
    datasetParameters->qv_depth = qv_depth;
    datasetParameters->as_depth = as_depth;

    datasetParameters->parameters = parameters;
    datasetParameters->hasSeek = false;
    datasetParameters->seekPosition = 0;
    return datasetParameters;
}

void freeDatasetParameters(DatasetParameters* datasetParameters){
    freeByteArray(datasetParameters->parameters);
    free(datasetParameters);
}

bool defineContentDatasetParameters(DatasetParameters* datasetParameters, char* filename){
    FILE* protectionInput = fopen(filename,"rb");
    if (protectionInput != NULL) {
        int errorSeekingEnd = fseek(protectionInput, 0, SEEK_END);
        long int size = ftell(protectionInput);
        int errorSeekingStart = fseek(protectionInput, 0, SEEK_SET);
        if(errorSeekingEnd!=0 || size<=0L || errorSeekingStart!=0){
            fprintf(stderr, "Error while determining parameters size.\n");
            return false;
        }
        datasetParameters->parameters = initByteArrayWithSize((uint64_t) size);
        Byte* buffer = (Byte*)malloc((uint64_t)size);
        if(buffer== NULL){
            fprintf(stderr, "Parameters memory buffer could not be created.\n");
            return false;
        }
        size_t protectionReadSize = fread(buffer,1,(uint64_t)size,protectionInput);
        if(protectionReadSize!=(uint64_t)size){
            fprintf(stderr, "Parameters input could not be read.\n");
            free(buffer);
            return false;
        }
        copyBytesSource(datasetParameters->parameters, buffer,(uint64_t)size);
        free(buffer);
        return true;
    }
    fprintf(stderr, "Parameters input could not be opened.\n");
    return false;

}

bool writeDatasetParameters(DatasetParameters* datasetParameters, FILE *outputFile){
    bool typeWriteSuccessfulWrite = fwrite(datasetParametersName, sizeof(char), 4, outputFile)==4;
    uint64_t sizeDatasetParameters = getSizeDatasetParameters(datasetParameters);
    uint64_t sizeDatasetParametersBigEndian = nativeToBigEndian64(sizeDatasetParameters);
    bool sizeSuccessfulWrite = fwrite(&sizeDatasetParametersBigEndian, sizeof(uint64_t),1,outputFile)==1;
    if (!typeWriteSuccessfulWrite || !sizeSuccessfulWrite){
        fprintf(stderr,"Error writing type or size of dataset parameters.\n");
        return false;
    }
    return writeContentDatasetParameters(datasetParameters, outputFile);
}

bool writeContentDatasetParameters(DatasetParameters* datasetParameters, FILE *outputFile){
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);
    writeToBitstream(&outputBitstream, datasetParameters->dataset_group_ID);
    writeBigEndian16ToBitstream(&outputBitstream, datasetParameters->dataset_ID);
    writeNBitsShift(&outputBitstream, 8, (char*)&datasetParameters->parent_parameter_set_ID);
    writeNBitsShift(&outputBitstream, 8, (char*)&datasetParameters->parameter_set_ID);


    writeNBitsShift(&outputBitstream, 4, (char*)&datasetParameters->dataset_type);
    writeNBitsShift(&outputBitstream, 8, (char*)&datasetParameters->alphabet_ID);
    writeNBitsShiftAndConvertToBigEndian32_new(&outputBitstream, 24, datasetParameters->reads_length);
    writeNBitsShift(&outputBitstream, 2, (char*)&datasetParameters->number_of_template_segments_minus1);
    uint8_t bufferReserved = 0;
    writeNBitsShift(&outputBitstream, 6, (char*)&bufferReserved);
    writeNBitsShiftAndConvertToBigEndian32_new(&outputBitstream, 29, datasetParameters->max_au_data_unit_size);
    writeBit(&outputBitstream, (uint8_t) (datasetParameters->pos_40_bits ? 1 : 0));
    writeNBitsShift(&outputBitstream, 3, (char*)&datasetParameters->qv_depth);
    writeNBitsShift(&outputBitstream, 3, (char*)&datasetParameters->as_depth);


    writeBuffer(&outputBitstream);
    if (datasetParameters->parameters != NULL) {
        return writeByteArray(datasetParameters->parameters, outputFile);
    }
    fprintf(stderr,"Dataset parameters data not set.\n");
    return false;

}

uint64_t getSizeContentDatasetParameters(DatasetParameters* datasetParameters){
    uint64_t contentSize = 1+2+2+10;
    if (datasetParameters->parameters != NULL) {
        return contentSize + getSizeByteArray(datasetParameters->parameters);
    }
    return contentSize;

}

uint64_t getSizeDatasetParameters(DatasetParameters* datasetParameters){
    return BOX_HEADER_SIZE + getSizeContentDatasetParameters(datasetParameters);
}

DatasetParameters *
parseDatasetParameters(uint64_t boxContentSize, FILE *inputFile) {
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

    uint64_t encodingParametersSize = boxContentSize - 1 - 2 - 2 - 10;
    Byte* parameterBuffer = (Byte*) malloc(encodingParametersSize*sizeof(Byte));
    if(parameterBuffer== NULL){
        fprintf(stderr, "Error while reserving memory for parameters buffer.\n");
        return NULL;
    }
    long seekPosition = ftell(inputFile);
    if(seekPosition == -1){
        fprintf(stderr, "Could not get file position.\n");
        free(parameterBuffer);
        return NULL;
    }

    size_t parameterBufferReadSize = fread(parameterBuffer,sizeof(Byte),encodingParametersSize,inputFile);
    if(parameterBufferReadSize!=encodingParametersSize){
        fprintf(stderr, "Could not read requested number of bytes.\n");
        free(parameterBuffer);
        return NULL;
    }
    DatasetParameters* datasetParameters = initDatasetParameters(
            dataset_group_ID, dataset_ID, parent_parameter_set_ID, parameter_set_ID
    );
    datasetParameters->hasSeek = true;
    datasetParameters->seekPosition = (size_t)seekPosition;

    datasetParameters->dataset_type = dataset_type;
    datasetParameters->alphabet_ID = alphabet_ID;
    datasetParameters->reads_length = reads_length;
    datasetParameters->number_of_template_segments_minus1 = number_of_template_segments_minus1;
    datasetParameters->max_au_data_unit_size = max_au_data_unit_size;
    datasetParameters->pos_40_bits = pos_40_bits;
    datasetParameters->qv_depth = qv_depth;
    datasetParameters->as_depth = as_depth;

    copyContentDatasetParameters(datasetParameters, (char*)parameterBuffer, encodingParametersSize);
    free(parameterBuffer);
    return datasetParameters;
}

bool copyContentDatasetParameters(DatasetParameters* datasetParameters, char *content, uint64_t contentSize){
    datasetParameters->parameters = initByteArrayWithSize((uint64_t) contentSize);
    return copyBytesSource(datasetParameters->parameters,(Byte*)content, contentSize);
}

size_t getDatasetParametersSeekPosition(DatasetParameters *datasetParameters){
    return datasetParameters->seekPosition;
}