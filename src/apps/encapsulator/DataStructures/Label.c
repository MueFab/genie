//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include <string.h>
#include "../Boxes.h"
#include "../utils.h"

RegionsForDataset* initRegionsForDataset(DatasetId datasetId) {
    RegionsForDataset* regionsForDataset = (RegionsForDataset*)malloc(sizeof(RegionsForDataset));
    if (regionsForDataset == NULL) {
        return NULL;
    }
    regionsForDataset->datasetId = datasetId;
    regionsForDataset->regions = initVector();
    return regionsForDataset;
}

void freeRegionsForDataset(RegionsForDataset* regionsForDataset) {
    size_t numberDatasets = getSize(regionsForDataset->regions);
    for (size_t i = 0; i < numberDatasets; i++) {
        freeRegion(getValue(regionsForDataset->regions, i));
    }
    freeVector(regionsForDataset->regions);
    free(regionsForDataset);
}

bool writeRegionsForDataset(RegionsForDataset* regionsForDataset, OutputBitstream* outputFile) {
    bool datasetidSuccessfulWrite =
        writeNBitsShiftAndConvertToBigEndian16(outputFile, 16, regionsForDataset->datasetId);
    uint8_t numRegions = (uint8_t)getSize(regionsForDataset->regions);
    bool numRegionsSuccessfulWrite = writeToBitstream(outputFile, numRegions);

    if (!datasetidSuccessfulWrite || !numRegionsSuccessfulWrite) {
        fprintf(stderr, "Error writing dataset id, reference id or number regions (RegionsForDataset).\n");
        return false;
    }
    for (uint8_t region_i = 0; region_i < numRegions; region_i++) {
        if (!writeRegion(getValue(regionsForDataset->regions, region_i), outputFile)) {
            fprintf(stderr, "Error writing region number %i.\n", region_i);
            return false;
        };
    }
    return true;
}

RegionsForDataset* parseRegionsForDataset(InputBitstream* inputFile) {
    DatasetId datasetId;
    uint8_t numRegions;

    bool datasetIdSuccesfulRead = readNBitsBigToNativeEndian16(inputFile, 16, &datasetId);
    bool numRegionsSuccesfulRead = readNBits8(inputFile, 8, &numRegions);

    RegionsForDataset* regionsForDataset = initRegionsForDataset(datasetId);
    if (regionsForDataset == NULL) {
        return NULL;
    }
    for (uint8_t region_i = 0; region_i < numRegions; region_i++) {
        Region* region = parseRegion(inputFile);
        if (region != NULL) {
            addRegion(regionsForDataset, region);
        }
    }
    return regionsForDataset;
}
uint64_t getRegionsForDatasetSize(RegionsForDataset* regionsForDataset) {
    uint64_t contentSize = 16;  // datasetid
    contentSize += 8;           // num_regions
    uint8_t num_regions = (uint8_t)getSize(regionsForDataset->regions);
    for (uint8_t region_i = 0; region_i < num_regions; region_i++) {
        Region* region = getValue(regionsForDataset->regions, region_i);
        if (region != NULL) {
            contentSize += getContentSizeRegion(region);
        }
    }
    return contentSize;
}

void addRegion(RegionsForDataset* regionsForDataset, Region* region) { pushBack(regionsForDataset->regions, region); }

Label* initLabel(char* labelId) {
    Label* label = (Label*)malloc(sizeof(Label));
    label->labelId = labelId;
    label->regionsForDataset = initVector();
    return label;
}

void freeLabel(Label* label) {
    size_t numberRegions = getSize(label->regionsForDataset);
    for (size_t region_i = 0; region_i < numberRegions; region_i++) {
        RegionsForDataset* regionsForDataset = getValue(label->regionsForDataset, region_i);
        freeRegionsForDataset(regionsForDataset);
    }
    free(label->labelId);
    freeVector(label->regionsForDataset);
    free(label);
}

void addRegionsForDataset(Label* label, RegionsForDataset* regionsForDataset) {
    pushBack(label->regionsForDataset, regionsForDataset);
}

unsigned long getNumberRegionsForDataset(Label* label) { return getSize(label->regionsForDataset); }

uint64_t getContentSizeLabel(Label* label) {
    uint64_t contentSize = (strlen(label->labelId) + 1) * 8;
    contentSize += 16;  // num_datasets
    size_t numberRegionsForDataset = getNumberRegionsForDataset(label);
    for (size_t region_i = 0; region_i < numberRegionsForDataset; region_i++) {
        RegionsForDataset* regionsForDataset = getValue(label->regionsForDataset, region_i);
        contentSize += getRegionsForDatasetSize(regionsForDataset);
    }
    return bitsToBytes(contentSize);
}

uint64_t getSizeLabel(Label* label) { return BOX_HEADER_SIZE + getContentSizeLabel(label); }

bool writeLabel(Label* label, FILE* outputFile) {
    uint64_t labelBoxSize = getSizeLabel(label);
    bool headerBoxSuccessfulWrite = writeBoxHeader(outputFile, datasetsGroupLabelName, labelBoxSize);
    if (!headerBoxSuccessfulWrite) {
        fprintf(stderr, "Error writing label box header.\n");
        return false;
    }
    return writeContentLabel(label, outputFile);
}

bool writeContentLabel(Label* label, FILE* outputFile) {
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);

    size_t labelIdSize = strlen(label->labelId);
    bool labelIdSuccessfulWrite = writeBytes(&outputBitstream, labelIdSize + 1, label->labelId);

    uint16_t numberRegionsForDataset = (uint16_t)getNumberRegionsForDataset(label);
    bool numberRegionsSuccessfulWrite =
        writeNBitsShiftAndConvertToBigEndian16(&outputBitstream, 16, numberRegionsForDataset);
    if (!labelIdSuccessfulWrite || !numberRegionsSuccessfulWrite) {
        fprintf(stderr, "Error writing label Id or number regions.\n");
        return false;
    }
    for (size_t region_i = 0; region_i < numberRegionsForDataset; region_i++) {
        RegionsForDataset* regionsForDataset = getValue(label->regionsForDataset, region_i);
        bool regionWrittenSuccessful = writeRegionsForDataset(regionsForDataset, &outputBitstream);
        if (!regionWrittenSuccessful) {
            fprintf(stderr, "Error writing region.\n");
            return false;
        }
    }
    return writeBuffer(&outputBitstream);
}

Label* parseLabel(FILE* inputFile) {
    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    char* labelId = calloc(16384, (sizeof(char)));
    uint8_t nextChar;
    int currentPosInLabelId = 0;
    readNBits8(&inputBitstream, 8, &nextChar);
    while (nextChar != 0) {
        labelId[currentPosInLabelId] = nextChar;
        currentPosInLabelId++;
        readNBits8(&inputBitstream, 8, &nextChar);
    }

    uint16_t numberRegionsForDataset;
    bool numberRegionsForDatasetSuccessfulRead =
        readNBitsBigToNativeEndian16(&inputBitstream, 16, &numberRegionsForDataset);

    if (!numberRegionsForDatasetSuccessfulRead) {
        fprintf(stderr, "error reading sequence id or number of regions.\n");
        return NULL;
    }

    Label* label = initLabel(labelId);
    for (int regionsForDataset_i = 0; regionsForDataset_i < numberRegionsForDataset; regionsForDataset_i++) {
        RegionsForDataset* regionsForDataset = parseRegionsForDataset(&inputBitstream);
        if (regionsForDataset != NULL) {
            addRegionsForDataset(label, regionsForDataset);
        } else {
            freeLabel(label);
            fprintf(stderr, "error reading region.\n");
            return NULL;
        }
    }

    return label;
}

Region* initRegion(uint16_t sequence_Id, uint64_t start_pos, uint64_t end_pos) {
    Region* region = (Region*)malloc(sizeof(Region));
    region->sequence_Id = sequence_Id;
    region->start_pos = start_pos;
    region->end_pos = end_pos;
    region->classes = initClassesSet();
    return region;
}

void freeRegion(Region* region) { free(region); }

void addClass(Region* region, uint8_t classId) { addClassToSet(&(region->classes), classId); }

unsigned long getNumberClasses(Region* region) { return getSizeSet(region->classes); }

bool writeRegion(Region* region, OutputBitstream* outputFile) {
    bool sequenceIDSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian16(outputFile, 16, region->sequence_Id);
    uint8_t number_classes = (uint8_t)getNumberClasses(region);
    bool numberClassesSuccessfulWrite = writeNBitsShift(outputFile, 4, (char*)&number_classes);
    if (!sequenceIDSuccessfulWrite || !numberClassesSuccessfulWrite) {
        fprintf(stderr, "Error writing sequence Id or number classes.\n");
        return false;
    }
    for (uint8_t class_i = 1; class_i < 6; class_i++) {
        if (checkClass(region->classes, class_i)) {
            if (!writeNBitsShift(outputFile, 4, (char*)&class_i)) {
                fprintf(stderr, "Error writing class.\n");
                return false;
            }
        }
    }
    bool startPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64_new(outputFile, 40, region->start_pos);
    bool endPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64_new(outputFile, 40, region->end_pos);
    if (!startPositionSuccessfulWrite || !endPositionSuccessfulWrite) {
        fprintf(stderr, "Error writing start and end position.\n");
        return false;
    }

    return true;
}

uint64_t getContentSizeRegion(Region* region) {
    uint64_t contentSize = 16;                    // seq_ID
    contentSize += 4;                             // numClasses
    contentSize += getNumberClasses(region) * 4;  // classIds
    contentSize += 40;
    contentSize += 40;
    return contentSize;
}
Region* parseRegion(InputBitstream* inputFile) {
    uint16_t sequence_Id;
    uint8_t numberClasses;
    bool sequenceIdRead = readNBitsBigToNativeEndian16(inputFile, 16, &sequence_Id);
    bool numberClassesRead = readNBits8(inputFile, 4, &numberClasses);
    if (!sequenceIdRead || !numberClassesRead) {
        fprintf(stderr, "Error reading sequenceId or number of classes.\n");
        return NULL;
    }
    ClassesSet classes = initClassesSet();
    for (unsigned int class_i = 0; class_i < numberClasses; class_i++) {
        uint8_t class_id;
        if (!readNBits8(inputFile, 4, &class_id)) {
            fprintf(stderr, "Error reading class type.\n");
            return NULL;
        }
        addClassToSet(&classes, class_id);
    }
    uint64_t start_pos;
    uint64_t end_pos;
    bool startPosRead = readNBitsBigToNativeEndian64(inputFile, 40, &start_pos);
    bool endPosRead = readNBitsBigToNativeEndian64(inputFile, 40, &end_pos);
    if (!startPosRead || !endPosRead) {
        fprintf(stderr, "Error reading start and end position.\n");
        return NULL;
    }
    Region* region = initRegion(sequence_Id, start_pos, end_pos);
    for (uint8_t class_i = 1; class_i < 6; class_i++) {
        if (checkClass(classes, class_i)) {
            addClass(region, class_i);
        }
    }
    return region;
}