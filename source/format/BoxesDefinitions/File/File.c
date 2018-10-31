//
// Created by gencom on 26/07/17.
//

#include <stdlib.h>
#include <memory.h>
#include <Boxes.h>
#include "../../Boxes.h"
#include "../../utils.h"

File* initFile(){
    File* file = (File*)malloc(sizeof(File));
    file->fileHeader= NULL;
    file->datasetsGroups = initVector();
    return file;
}

void freeFile(File* file){
    freeFileHeader(file->fileHeader);
    size_t datasetsGroupContainer_size = getSize(file->datasetsGroups);
    for(
        size_t datasetsGroupContainer_i=0;
        datasetsGroupContainer_i<datasetsGroupContainer_size;
        datasetsGroupContainer_i++
    ){
        DatasetsGroupContainer* datasetsGroupContainer = getValue(file->datasetsGroups, datasetsGroupContainer_i);
        if(datasetsGroupContainer != NULL) {
            freeDatasetsGroupContainer(datasetsGroupContainer);
        }
    }
    freeVector(file->datasetsGroups);
    free(file);
}

void setFileHeaderToFile(File* file, FileHeader* fileHeader){
    file->fileHeader = fileHeader;
}

void addDatasetsGroupToFile(File *file, DatasetsGroupContainer *datasetsGroupContainer){
    pushBack(file->datasetsGroups, datasetsGroupContainer);
}

bool writeFile(File *file, FILE *outputFile) {
    if (!isFileValid(file)){
        fprintf(stderr,"File instance is not valid.\n");
        return false;
    }
    if(!writeFileHeader(file->fileHeader, outputFile)){
        fprintf(stderr,"Error writing file header.\n");
        return false;
    }
    size_t datasetsGroupContainer_size = getSize(file->datasetsGroups);
    for(
        size_t datasetsGroupContainer_i=0;
        datasetsGroupContainer_i<datasetsGroupContainer_size;
        datasetsGroupContainer_i++
    ){
        DatasetsGroupContainer* datasetsGroupContainer = getValue(file->datasetsGroups, datasetsGroupContainer_i);
        if (datasetsGroupContainer != NULL){
            if (!writeDatasetsGroupContainer(datasetsGroupContainer, outputFile)){
                fprintf(stderr,"Error writing dataset group index:%lu.\n",datasetsGroupContainer_i);
                return false;
            }
        }
    }
    return true;
}

bool generateFileSeekPoints(File *file) {
    size_t datasetsGroupContainer_size = getSize(file->datasetsGroups);
    for(
            size_t datasetsGroupContainer_i=0;
            datasetsGroupContainer_i<datasetsGroupContainer_size;
            datasetsGroupContainer_i++
            ){
        DatasetsGroupContainer* datasetsGroupContainer = getValue(file->datasetsGroups, datasetsGroupContainer_i);
        generateDatasetsGroupSeekPoints(datasetsGroupContainer);
    }
    return true;
}

bool isFileValid(File* file){
    if (file->fileHeader == NULL) return false;
    size_t datasetsGroupContainer_size = getSize(file->datasetsGroups);
    for(
        size_t datasetsGroupContainer_i=0;
        datasetsGroupContainer_i<datasetsGroupContainer_size;
        datasetsGroupContainer_i++
    ){
        DatasetsGroupContainer* datasetsGroupContainer = getValue(file->datasetsGroups, datasetsGroupContainer_i);
        if (datasetsGroupContainer!= NULL){
            if (!isDatasetsGroupContainerValid(datasetsGroupContainer)){
                return false;
            }
        }
    }
    return true;
}

File* parseFile(FILE *inputFile, char *filename){
    char boxType[5];
    uint64_t boxSize;
    bool successfulRead = readBoxHeader(inputFile, boxType, &boxSize);
    if (successfulRead){
        File* file = (File*)malloc(sizeof(File));

        file->fileHeader = NULL;
        file->datasetsGroups = initVector();

        if(strncmp(boxType,fileHeaderBoxName,4)==0){
            FileHeader* fileHeader = parseFileHeader(boxSize-12, inputFile);
            setFileHeaderToFile(file, fileHeader);
        }else{
            fprintf(stderr,"Cannot parse: expected file header not found.\n");
            return NULL;
        }
        while (readBoxHeader(inputFile,boxType,&boxSize)){
            if(strncmp(boxType,"dgcn",4)==0){
                DatasetsGroupContainer* datasetsGroupContainer =
                        parseDatasetsGroupContainer(inputFile, boxSize - 12, filename);
                if(datasetsGroupContainer == NULL){
                    fprintf(stderr, "Error reading datasets group container.\n");
                    return NULL;
                }
                addDatasetsGroupToFile(file, datasetsGroupContainer);
            }else{
                fprintf(stderr,"Cannot parse: expected dataset group not found.\n");
                freeFile(file);
                return NULL;
            }
        }
        return file;
    }
    fprintf(stderr,"Cannot read the file.\n");
    return NULL;
}

FileHeader* getFileHeader(File* file){
    return file->fileHeader;
}

DatasetsGroupContainer * getDatasetGroupContainer(File *file, size_t index) {
    if (index<0 || index >= getSize(file->datasetsGroups)){
        return NULL;
    }
    return getValue(file->datasetsGroups, index);

}
unsigned long getDatasetsGroupContainersSize(File *file) {
    return getSize(file->datasetsGroups);
}
