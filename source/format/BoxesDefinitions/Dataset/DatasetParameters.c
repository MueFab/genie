//
// Created by gencom on 27/07/17.
//

#include "../../Boxes.h"
#include "../../utils.h"


DatasetParameters *initDatasetParameters() {
    DatasetParameters* datasetParameters = (DatasetParameters*) malloc(sizeof(DatasetParameters));
    datasetParameters->parameters = NULL;
    datasetParameters->seekPosition = -1;
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
    if (datasetParameters->parameters != NULL) {
        return writeByteArray(datasetParameters->parameters, outputFile);
    }
    fprintf(stderr,"Dataset parameters data not set.\n");
    return false;

}

uint64_t getSizeContentDatasetParameters(DatasetParameters* datasetParameters){
    if (datasetParameters->parameters != NULL) {
        return getSizeByteArray(datasetParameters->parameters);
    }
    return 0;

}

uint64_t getSizeDatasetParameters(DatasetParameters* datasetParameters){
    return BOX_HEADER_SIZE + getSizeContentDatasetParameters(datasetParameters);
}

DatasetParameters *
parseDatasetParameters(uint64_t boxContentSize, FILE *inputFile) {
    Byte* parameterBuffer = (Byte*) malloc(boxContentSize*sizeof(Byte));
    if(parameterBuffer== NULL){
        fprintf(stderr, "Error while reserving memory for parameters buffer.\n");
        return NULL;
    }
    long seekPosition = ftell(inputFile);
    size_t protectionBufferReadSize = fread(parameterBuffer,sizeof(Byte),boxContentSize,inputFile);
    if(protectionBufferReadSize!=boxContentSize){
        fprintf(stderr, "Could not read requested number of bytes.\n");
        free(parameterBuffer);
        return NULL;
    }
    DatasetParameters* datasetParameters = initDatasetParameters();
    datasetParameters->seekPosition = seekPosition;
    copyContentDatasetParameters(datasetParameters, (char*)parameterBuffer, boxContentSize);
    free(parameterBuffer);
    return datasetParameters;
}

bool copyContentDatasetParameters(DatasetParameters* datasetParameters, char *content, uint64_t contentSize){
    datasetParameters->parameters = initByteArrayWithSize((uint64_t) contentSize);
    return copyBytesSource(datasetParameters->parameters,(Byte*)content, contentSize);
}

long getDatasetParametersSeekPosition(DatasetParameters *datasetParameters){
    return datasetParameters->seekPosition;
}