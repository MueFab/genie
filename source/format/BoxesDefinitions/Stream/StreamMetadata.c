//
// Created by gencom on 26/07/17.
//

#include <stdlib.h>
#include "../../Boxes.h"
#include "../../utils.h"

StreamMetadata* initStreamMetadata(StreamContainer* streamContainer){
    StreamMetadata* streamMetadata = (StreamMetadata*)malloc(sizeof(StreamMetadata));
    streamMetadata->streamContainer=streamContainer;
    streamMetadata->metadata = NULL;
    streamMetadata->seekPosition = -1;
    return streamMetadata;
}

void freeStreamMetadata(StreamMetadata* streamMetadata){
    if(streamMetadata != NULL) {
        freeByteArray(streamMetadata->metadata);
        free(streamMetadata);
    }
}

bool defineContentStreamMetadata(StreamMetadata* streamMetadata, char* filename){
    FILE* metadataInput = fopen(filename,"rb");
    if (metadataInput != NULL) {
        int errorSeekingEnd = fseek(metadataInput, 0, SEEK_END);
        long int size = ftell(metadataInput);
        int errorSeekingStart = fseek(metadataInput, 0, SEEK_SET);
        if(errorSeekingEnd!=0 || size<=0L || errorSeekingStart!=0){
            fprintf(stderr, "Error while determining metadata size.\n");
            return false;
        }
        streamMetadata->metadata = initByteArrayWithSize((uint64_t) size);
        Byte* buffer = (Byte*)malloc((uint64_t)size);
        if(buffer== NULL){
            fprintf(stderr, "Stream group metadata memory buffer could not be created.\n");
            return false;
        }
        size_t metadataReadSize = fread(buffer,1,(uint64_t)size,metadataInput);
        if(metadataReadSize!=(uint64_t)size){
            fprintf(stderr, "Stream group metadata input could not be read.\n");
            free(buffer);
            return false;
        }
        copyBytesSource(streamMetadata->metadata,buffer,(uint64_t)size);
        free(buffer);
        return true;
    }
    fprintf(stderr, "Stream group metadata input could not be opened.\n");
    return false;

}

bool writeContentStreamMetadata(StreamMetadata* streamMetadata, FILE *outputFile){
    if (streamMetadata->metadata!= NULL) {
        return writeByteArray(streamMetadata->metadata, outputFile);
    }
    fprintf(stderr,"Stream metadata data not set.\n");
    return false;

}

bool writeStreamMetadata(StreamMetadata* streamMetadata, FILE *outputFile){
    bool typeWriteSuccessful = fwrite(streamMetadataName, sizeof(char),4,outputFile)==4;
    uint64_t streamMetadataSize = getSizeStreamMetadata(streamMetadata);
    uint64_t streamMetadataSizeBigEndian = nativeToBigEndian64(streamMetadataSize);
    bool sizeWriteSuccessfulWrite = fwrite(&streamMetadataSizeBigEndian, sizeof(uint64_t), 1, outputFile)==1;
    if (!typeWriteSuccessful || !sizeWriteSuccessfulWrite){
        fprintf(stderr,"Error writing type and size stream metadata.\n");
    }
    return writeContentStreamMetadata(streamMetadata, outputFile);
}

uint64_t getSizeContentStreamMetadata(StreamMetadata* streamMetadata ){
    if (streamMetadata->metadata!= NULL) {
        return getSizeByteArray(streamMetadata->metadata);
    }
    return 0;

}

uint64_t getSizeStreamMetadata(StreamMetadata* streamMetadata ){
    return 12 + getSizeContentStreamMetadata(streamMetadata);
}


StreamMetadata *parseStreamMetadata(StreamContainer* streamContainer, uint64_t boxContentSize, FILE *inputFile){
    long seekPoint = ftell(inputFile);
    Byte* metadataBuffer = (Byte*) malloc(boxContentSize*sizeof(Byte));
    if(metadataBuffer== NULL){
        fprintf(stderr, "Error while reserving memory for stream metadata buffer.\n");
        return NULL;
    }
    size_t metadataBufferReadSize = fread(metadataBuffer,sizeof(Byte),boxContentSize,inputFile);
    if(metadataBufferReadSize!=boxContentSize){
        fprintf(stderr, "Could not read requested number of bytes.\n");
        free(metadataBuffer);
        return NULL;
    }
    StreamMetadata* streamMetadata = initStreamMetadata(streamContainer);
    streamMetadata->seekPosition = seekPoint;
    copyContentStreamMetadata(streamMetadata, (char*)metadataBuffer, boxContentSize);
    free(metadataBuffer);
    return streamMetadata;
}

bool copyContentStreamMetadata(StreamMetadata* streamMetadata, char *content, uint64_t contentSize){
    streamMetadata->metadata = initByteArrayWithSize((uint64_t) contentSize);
    return copyBytesSource(streamMetadata->metadata, (Byte*)content, contentSize);
}

long getStreamMetadataSeekPosition(StreamMetadata* streamMetadata){
    return streamMetadata->seekPosition;
}