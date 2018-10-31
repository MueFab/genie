//
// Created by gencom on 27/07/17.
//

#include <string.h>
#include <stdlib.h>
#include "../../Boxes.h"
#include "../../utils.h"
#include "../../DataStructures/FromFile.h"

StreamContainer *initStreamContainer() {
    StreamContainer* streamContainer=(StreamContainer*)calloc(1,sizeof(StreamContainer));
    streamContainer->streamHeader = NULL;
    streamContainer->streamMetadata = NULL;
    streamContainer->streamProtection = NULL;
    streamContainer->dataFromFile = initFromFile();
    streamContainer->seekPosition = -1;
    return streamContainer;
}

void freeStreamContainer(StreamContainer* streamContainer){
    if(streamContainer->streamHeader != NULL) {
        freeStreamHeader(streamContainer->streamHeader);
    }
    if(streamContainer->streamMetadata != NULL) {
        freeStreamMetadata(streamContainer->streamMetadata);
    }
    if(streamContainer->streamProtection != NULL) {
        freeStreamProtection(streamContainer->streamProtection);
    }
    if(streamContainer->dataFromFile != NULL) {
        freeFromFile(streamContainer->dataFromFile);
    }
    free(streamContainer);
}

uint64_t getSizeContentStreamContainer(StreamContainer* streamContainer){
    uint64_t contentSize = 0;
    if (streamContainer->streamHeader != NULL) {
        contentSize += getSizeStreamHeader();
    }
    if (streamContainer->streamMetadata != NULL) {
        contentSize += getSizeStreamMetadata(streamContainer->streamMetadata);
    }
    if (streamContainer->streamProtection != NULL) {
        contentSize += getSizeStreamProtection(streamContainer->streamProtection);
    }
    contentSize += getFromFileSize(streamContainer->dataFromFile);
    return contentSize;
}

uint64_t getSizeStreamContainer(StreamContainer* streamContainer){
    return BOX_HEADER_SIZE + getSizeContentStreamContainer(streamContainer);
}

bool writeContentStreamContainer(StreamContainer* streamContainer, FILE *outputFile){
    if(streamContainer->streamHeader != NULL) {
        if (!writeStreamHeader(streamContainer->streamHeader, outputFile)) {
            fprintf(stderr, "Error writing stream header.\n");
            return false;
        }
    }
    if(streamContainer->streamMetadata != NULL) {
        if (!writeStreamMetadata(streamContainer->streamMetadata, outputFile)) {
            fprintf(stderr, "Error writing metadata.\n");
            return false;
        }
    }
    if(streamContainer->streamProtection != NULL) {
        if (!writeStreamProtection(streamContainer->streamProtection, outputFile)) {
            fprintf(stderr, "Error writing stream protection.\n");
            return false;
        }
    }
    if(!writeFromFile(streamContainer->dataFromFile, outputFile)){
        fprintf(stderr, "Error writing stream content.\n");
    }
    return true;
}

bool writeStreamContainer(StreamContainer* streamContainer, FILE *outputFile){
    bool typeSuccessfulWrite = fwrite(streamContainerName,sizeof(char),4,outputFile)==4;
    uint64_t sizeStreamContainer = getSizeStreamContainer(streamContainer);
    uint64_t sizeStreamContainerBigEndian = nativeToBigEndian64(sizeStreamContainer);
    bool sizeSuccessfulWrite = fwrite(&sizeStreamContainerBigEndian, sizeof(uint64_t),1,outputFile)==1;
    if(!typeSuccessfulWrite || !sizeSuccessfulWrite){
        fprintf(stderr, "Error writing type and size of stream container.\n");
        return false;
    }
    return writeContentStreamContainer(streamContainer, outputFile);
}

bool isStreamContainerValid(StreamContainer* streamContainer){
    return streamContainer->streamHeader!= NULL;
}

void setStreamHeader(StreamContainer* streamContainer, StreamHeader *streamHeader){
    streamContainer->streamHeader = streamHeader;
}

void setStreamMetadata(StreamContainer* streamContainer, StreamMetadata *streamMetadata){
    streamContainer->streamMetadata = streamMetadata;
}

void setStreamProtection(StreamContainer* streamContainer, StreamProtection *streamProtection){
    streamContainer->streamProtection = streamProtection;
}

void setDataFromFilename(StreamContainer *streamContainer, char *filename){
    if (streamContainer->dataFromFile != NULL){
        freeFromFile(streamContainer->dataFromFile);
    }
    streamContainer->dataFromFile = initFromFileWithFilename(filename);
}

void setDataFromFilenameWithBoundaries(StreamContainer *streamContainer, char *filename, long startPos, long endPos){
    if (streamContainer->dataFromFile != NULL){
        freeFromFile(streamContainer->dataFromFile);
    }
    streamContainer->dataFromFile = initFromFileWithFilenameAndBoundaries(filename, startPos, endPos);
}

StreamContainer *
parseStreamContainer(uint64_t boxContentSize, FILE *inputFile, char *fileName, DatasetContainer* datasetContainer) {
    StreamContainer* streamContainer = initStreamContainer();
    streamContainer->seekPosition = ftell(inputFile);

    char type[5];
    uint64_t boxSize;
    uint64_t dataContentSize = boxContentSize;


    while(getSizeContentStreamContainer(streamContainer)<boxContentSize){
        if(!readBoxHeader(inputFile,type, &boxSize)){
            fprintf(stderr,"Stream container content could not be read.\n");
            freeStreamContainer(streamContainer);
            return NULL;
        }
        if(strncmp(type,streamHeaderName,4)==0){
            StreamHeader* streamHeader = parseStreamHeader(streamContainer,inputFile);
            if(streamHeader!= NULL){
                setStreamHeader(streamContainer, streamHeader);
            }else{
                fprintf(stderr,"Error reading stream header.\n");
                freeStreamContainer(streamContainer);
                return NULL;
            }
            dataContentSize -= getSizeStreamHeader();
        }else if(strncmp(type,streamMetadataName,4)==0){
            StreamMetadata* streamMetadata = parseStreamMetadata(streamContainer,boxSize-12,inputFile);
            if(streamMetadata!= NULL){
                setStreamMetadata(streamContainer, streamMetadata);
            }else{
                fprintf(stderr,"Error reading stream metadata.\n");
                freeStreamContainer(streamContainer);
                return NULL;
            }
            dataContentSize -= getSizeStreamMetadata(streamMetadata);
        }else if(strncmp(type,streamProtectionName,4)==0){
            StreamProtection* streamProtection = parseStreamProtection(streamContainer,boxSize-12,inputFile);
            if(streamProtection!= NULL){
                setStreamProtection(streamContainer, streamProtection);
            }else{
                fprintf(stderr,"Error reading stream metadata.\n");
                freeStreamContainer(streamContainer);
                return NULL;
            }
            dataContentSize -= getSizeStreamProtection(streamProtection);
        }else{
            fseek(inputFile,-12,SEEK_CUR);
            long startPosition =  ftell(inputFile);
            long endPosition = startPosition + (long)dataContentSize;
            setDataFromFilenameWithBoundaries(streamContainer, fileName, startPosition, endPosition);
            fseek(inputFile,endPosition-startPosition,SEEK_CUR);
            dataContentSize-=getFromFileSize(streamContainer->dataFromFile);
            break;
        }
    }
    if(dataContentSize!=0 || getFromFileSize(streamContainer->dataFromFile)==0){
        fprintf(stderr,"Error reading stream container.\n");
        freeStreamContainer(streamContainer);
        return NULL;
    }

    if(datasetContainer != NULL && datasetContainer->datasetMasterIndexTable != NULL){
        DatasetMasterIndexTable* datasetMasterIndexTable = datasetContainer->datasetMasterIndexTable;
        uint64_t finalOffset = (uint64_t) ftell(inputFile)-datasetContainer->seekPosition;

        uint8_t streamClass = getStreamClass(streamContainer->streamHeader);
        uint8_t streamDescriptor = getStreamDescriptorId(streamContainer->streamHeader);

        uint8_t class_index;
        if(!getClassIndexForType(datasetContainer->datasetHeader, streamClass, &class_index)){
            fprintf(stderr,"Error unknown class.\n");
            freeStreamContainer(streamContainer);
            return NULL;
        }

        uint8_t descriptor_index;
        if(!getDescriptorIndexForType(datasetContainer->datasetHeader, class_index, streamDescriptor, &descriptor_index)){
            fprintf(stderr,"Error unknown descriptor.\n");
            freeStreamContainer(streamContainer);
            return NULL;
        }

        if(datasetMasterIndexTable->offsetsPerClass != NULL) {
            insertFinalOffset(
                    datasetMasterIndexTable,
                    class_index,
                    descriptor_index,
                    finalOffset
            );
        }
    }


    return streamContainer;
}


FromFile* getDataFromStreamContainer(StreamContainer* streamContainer){
    return streamContainer->dataFromFile;
}

long getStreamContainerSeekPosition(StreamContainer* streamContainer){
    return streamContainer->seekPosition;
}

bool generateStreamSeekPoints(StreamContainer* streamContainer){
    fprintf(
            stdout,
            "\tstream @ %li-%li\n",
            getStreamContainerSeekPosition(streamContainer),
            getStreamContainerSeekPosition(streamContainer)
            + getSizeContentStreamContainer(streamContainer)
    );
    if(streamContainer->streamHeader != NULL) {
        fprintf(
                stdout,
                "\t\tstream header @ %li-%li\n",
                getStreamHeaderSeekPosition(streamContainer->streamHeader),
                getStreamHeaderSeekPosition(streamContainer->streamHeader)
                + getSizeContentStreamHeader()
        );
    }
    if(streamContainer->streamMetadata != NULL) {
        fprintf(
                stdout,
                "\t\tstream metadata @ %li-%li\n",
                getStreamMetadataSeekPosition(streamContainer->streamMetadata),
                getStreamMetadataSeekPosition(streamContainer->streamMetadata)
                + getSizeContentStreamMetadata(streamContainer->streamMetadata)
        );
    }
    if(streamContainer->streamProtection != NULL) {
        fprintf(
                stdout,
                "\t\tstream protection @ %li-%li\n",
                getStreamProtectionSeekPosition(streamContainer->streamProtection),
                getStreamProtectionSeekPosition(streamContainer->streamProtection)
                + getSizeContentStreamProtection(streamContainer->streamProtection)
        );
    }
    fprintf(
            stdout,
            "\t\tstream data @ %li-%li\n",
            streamContainer->dataFromFile->startPos,
            streamContainer->dataFromFile->endPos
    );
    return true;
}