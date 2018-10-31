//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include <string.h>
#include "../Boxes.h"

ByteArray* initByteArray(){
    ByteArray* byteArray = (ByteArray*)malloc(sizeof(ByteArray));
    byteArray->byte_array = NULL;
    byteArray->allocated_size = 0;
    byteArray->size = 0;
    return byteArray;
}
ByteArray* initByteArrayWithSize(uint64_t size){
    ByteArray* byteArray = (ByteArray*)malloc(sizeof(ByteArray));
    byteArray->byte_array = (Byte*)malloc(size*sizeof(Byte));
    if(byteArray->byte_array != NULL) {
        byteArray->allocated_size = size;
    }else{
        byteArray->allocated_size = 0;
    }
    byteArray->size = 0;
    return byteArray;
}

void freeByteArray(ByteArray* byteArray){
    free(byteArray->byte_array);
    free(byteArray);
}

bool writeByteArray(ByteArray* byteArray, FILE* outputFile){
    size_t written_size = fwrite(byteArray->byte_array,sizeof(Byte),byteArray->size,outputFile);
    return written_size == byteArray->size;
}

uint64_t getSizeByteArray(ByteArray* byteArray){
    return byteArray->size;
}

bool copyBytesSource(ByteArray* byteArray, Byte* source, uint64_t source_size){
    if( byteArray->allocated_size<source_size ){
        return false;
    }
    memcpy(byteArray->byte_array,source,source_size);
    byteArray->size=source_size;
    return true;

}