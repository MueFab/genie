//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include <Boxes.h>
#include <string.h>
#include "../Boxes.h"
#include "../utils.h"

RegionsForDataset* initRegionsForDataset(DatasetId datasetId, ReferenceId referenceId){
    RegionsForDataset* regionsForDataset = (RegionsForDataset*)malloc(sizeof(RegionsForDataset));
    if (regionsForDataset == NULL){
        return NULL;
    }
    regionsForDataset->datasetId = datasetId;
    regionsForDataset->referenceId = referenceId;
    regionsForDataset->regions = initVector();
    return regionsForDataset;
}

void freeRegionsForDataset(RegionsForDataset *regionsForDataset) {
    size_t numberDatasets = getSize(regionsForDataset->regions);
    for(size_t i=0; i<numberDatasets; i++){
        freeRegion(getValue(regionsForDataset->regions, i));
    }
    freeVector(regionsForDataset->regions);
    free(regionsForDataset);
}

bool writeRegionsForDataset(RegionsForDataset* regionsForDataset, FILE* outputFile){
    //writeBigEndian16ToFile(regionsForDataset->datasetId, outputFile);
    bool datasetidSuccessfulWrite = write((uint8_t) regionsForDataset->datasetId, outputFile);
    bool referenceIdSuccesfulWrite = write(regionsForDataset->referenceId, outputFile);
    uint8_t numRegions = (uint8_t) getSize(regionsForDataset->regions);
    bool numRegionsSuccessfulWrite = write(numRegions, outputFile);

    if(!datasetidSuccessfulWrite || !referenceIdSuccesfulWrite || !numRegionsSuccessfulWrite){
        fprintf(stderr,"Error writing dataset id, reference id or number regions (RegionsForDataset).\n");
        return false;
    }
    for(uint8_t region_i=0; region_i<numRegions; region_i++){
        if(!writeRegion(getValue(regionsForDataset->regions,region_i),outputFile)){
            fprintf(stderr,"Error writing region number %i.\n",region_i);
            return false;
        };
    }
    return true;
}

RegionsForDataset* parseRegionsForDataset(FILE *inputFile){
    uint8_t datasetId;
    uint8_t referenceId;
    uint8_t numRegions;

    bool datasetIdSuccesfulRead = read(&datasetId, inputFile);
    bool referenceIdSuccesfulRead = read(&referenceId, inputFile);
    bool numRegionsSuccesfulRead = read(&numRegions, inputFile);

    RegionsForDataset* regionsForDataset = initRegionsForDataset(datasetId, referenceId);
    if(regionsForDataset == NULL){
        return NULL;
    }
    for(uint8_t region_i = 0; region_i<numRegions; region_i++){
        Region* region = parseRegion(inputFile);
        if(region != NULL){
            addRegion(regionsForDataset, region);
        }
    }
    return regionsForDataset;
}
uint64_t getRegionsForDatasetSize(RegionsForDataset* regionsForDataset){
    uint8_t num_regions = (uint8_t) getSize(regionsForDataset->regions);
    uint64_t sizeRegions = 0;
    for(uint8_t region_i = 0; region_i<num_regions; region_i++){
        Region* region = getValue(regionsForDataset->regions, region_i);
        if(region != NULL){
            sizeRegions += getContentSizeRegion(region);
        }
    }
    //return sizeof(DatasetId)+sizeof(ReferenceId)+sizeof(uint8_t)+sizeRegions;
    return sizeof(uint8_t)+sizeof(ReferenceId)+sizeof(uint8_t)+sizeRegions;
}

void addRegion(RegionsForDataset* regionsForDataset, Region* region){
    pushBack(regionsForDataset->regions, region);
}

Label *initLabel(char *labelId) {
    Label* label = (Label*)malloc(sizeof(Label));
    label->labelId = labelId;
    label->regionsForDataset = initVector();
    return label;
}

void freeLabel(Label* label){
    size_t numberRegions = getSize(label->regionsForDataset);
    for(size_t region_i=0; region_i<numberRegions; region_i++){
        RegionsForDataset* regionsForDataset = getValue(label->regionsForDataset, region_i);
        freeRegionsForDataset(regionsForDataset);
    }
    free(label->labelId);
    freeVector(label->regionsForDataset);
    free(label);
}

void addRegionsForDataset(Label* label, RegionsForDataset *regionsForDataset){
    pushBack(label->regionsForDataset,regionsForDataset);
}

unsigned long getNumberRegionsForDataset(Label *label){
    return getSize(label->regionsForDataset);
}

uint64_t getContentSizeLabel(Label* label){
    uint64_t contentSize = strlen(label->labelId)+1;
    contentSize += sizeof(uint16_t);
    size_t numberRegionsForDataset = getNumberRegionsForDataset(label);
    for(size_t region_i=0; region_i<numberRegionsForDataset; region_i++){
        RegionsForDataset* regionsForDataset = getValue(label->regionsForDataset, region_i);
        contentSize += getRegionsForDatasetSize(regionsForDataset);
    }
    return contentSize;
}

uint64_t getSizeLabel(Label* label){
    return BOX_HEADER_SIZE + getContentSizeLabel(label);
}

bool writeLabel(Label* label, FILE *outputFile){
    uint64_t labelBoxSize = BOX_HEADER_SIZE + getSizeLabel(label);
    bool headerBoxSuccessfulWrite = writeBoxHeader(outputFile,datasetsGroupLabelName, labelBoxSize);
    if (!headerBoxSuccessfulWrite){
        fprintf(stderr,"Error writing label box header.\n");
        return false;
    }
    return writeContentLabel(label, outputFile);
}

bool writeContentLabel(Label *label, FILE *outputFile){


    size_t labelIdSize = strlen(label->labelId);
    bool labelIdSuccessfulWrite =
            fwrite(
                    label->labelId,
                    sizeof(char),labelIdSize+1,
                    outputFile
            )==labelIdSize+1;

    uint16_t numberRegionsForDataset = (uint16_t) getNumberRegionsForDataset(label);
    bool numberRegionsSuccessfulWrite = writeBigEndian16ToFile(numberRegionsForDataset, outputFile);
    if (!labelIdSuccessfulWrite || !numberRegionsSuccessfulWrite){
        fprintf(stderr, "Error writing label Id or number regions.\n");
        return false;
    }
    for(size_t region_i=0; region_i<numberRegionsForDataset; region_i++){
        RegionsForDataset* regionsForDataset = getValue(label->regionsForDataset, region_i);
        bool regionWrittenSuccessful = writeRegionsForDataset(regionsForDataset, outputFile);
        if(!regionWrittenSuccessful){
            fprintf(stderr, "Error writing region.\n");
            return false;
        }
    }
    return true;
}

Label* parseLabel(FILE* inputFile){
    char* labelId=NULL;
    size_t labelIdSize = 0;
    uint16_t numberRegionsForDataset;
    if (getdelim(&labelId,&labelIdSize,'\0',inputFile)==-1) {
        fprintf(stderr,"error reading label id.\n");
        return NULL;
    }
    bool numberRegionsForDatasetSuccessfulRead = readBigEndian16FromFile(&numberRegionsForDataset, inputFile);

    if(!numberRegionsForDatasetSuccessfulRead){
        fprintf(stderr,"error reading sequence id or number of regions.\n");
        return NULL;
    }

    Label* label = initLabel(labelId);
    for(int regionsForDataset_i = 0; regionsForDataset_i<numberRegionsForDataset; regionsForDataset_i++){
        RegionsForDataset* regionsForDataset = parseRegionsForDataset(inputFile);
        if(regionsForDataset != NULL){
            addRegionsForDataset(label, regionsForDataset);
        }else{
            freeLabel(label);
            fprintf(stderr,"error reading region.\n");
            return NULL;
        }
    }

    return label;
}





Region* initRegion(uint16_t sequence_Id, uint32_t start_pos, uint32_t end_pos){
    Region* region = (Region*)malloc(sizeof(Region));
    region->sequence_Id = sequence_Id;
    region->start_pos = start_pos;
    region->end_pos = end_pos;
    region->classes = initClassesSet();
    return region;
}

void freeRegion(Region* region){
    free(region);
}

void addClass(Region* region, uint8_t classId){
    addClassToSet(&(region->classes), classId);
}

unsigned long getNumberClasses(Region* region){
    return getSizeSet(region->classes);
}

bool writeRegion(Region* region, FILE* outputFile){
    bool sequenceIDSuccessfulWrite = writeBigEndian16ToFile(region->sequence_Id, outputFile);
    uint8_t number_classes = (uint8_t) getNumberClasses(region);
    bool numberClassesSuccessfulWrite = write(number_classes, outputFile);
    if (!sequenceIDSuccessfulWrite || !numberClassesSuccessfulWrite){
        fprintf(stderr, "Error writing sequence Id or number classes.\n");
        return false;
    }
    for(
        uint8_t class_i=0;
        class_i < 8;
        class_i ++
    ){
        if (checkClass(region->classes, class_i)){
            size_t classWrittenSize = fwrite(&class_i, sizeof(uint8_t), 1, outputFile);
            if (classWrittenSize != 1) {
                fprintf(stderr, "Error writing class.\n");
                return false;
            }
        }

    }
    bool startPositionSuccessfulWrite = writeBigEndian32ToFile(region->start_pos, outputFile);
    bool endPositionSuccessfulWrite = writeBigEndian32ToFile(region->end_pos, outputFile);
    if (!startPositionSuccessfulWrite || !endPositionSuccessfulWrite){
        fprintf(stderr, "Error writing start and end position.\n");
        return false;
    }

    return true;
}

uint64_t getContentSizeRegion(Region *region){
    uint64_t contentSize = sizeof(region->sequence_Id);
    contentSize += sizeof(uint8_t);
    contentSize += getNumberClasses(region)*sizeof(uint8_t);
    contentSize += sizeof(region->start_pos);
    contentSize += sizeof(region->end_pos);
    return contentSize;
}
Region * parseRegion(FILE *inputFile) {
    uint16_t sequence_Id;
    uint8_t numberClasses;
    bool sequenceIdRead = readBigEndian16FromFile(&sequence_Id, inputFile);
    bool numberClassesRead = read(&numberClasses, inputFile);
    if (!sequenceIdRead || ! numberClassesRead){
        fprintf(stderr,"Error reading sequenceId or number of classes.\n");
        return NULL;
    }
    ClassesSet classes = initClassesSet();
    for(unsigned int class_i = 0; class_i<numberClasses; class_i++){
        uint8_t class_id;
        if( fread(&class_id,sizeof(class_id),1,inputFile) != 1){
            fprintf(stderr,"Error reading class type.\n");
            return NULL;
        }
        addClassToSet(&classes, class_id);
    }
    uint32_t start_pos;
    uint32_t end_pos;
    bool startPosRead = readBigEndian32FromFile(&start_pos, inputFile);
    bool endPosRead = readBigEndian32FromFile(&end_pos, inputFile);
    if(!startPosRead || !endPosRead){
        fprintf(stderr,"Error reading start and end position.\n");
        return NULL;
    }
    Region* region = initRegion(sequence_Id,start_pos,end_pos);
    for(uint8_t class_i=0; class_i<8; class_i++){
        if (checkClass(classes, class_i)) {
            addClass(region, class_i);
        }
    }
    return region;
}