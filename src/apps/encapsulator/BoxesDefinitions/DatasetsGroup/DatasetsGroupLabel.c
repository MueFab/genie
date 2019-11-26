//
// Created by bscuser on 6/02/18.
//
#include <string.h>
#include "../../Boxes.h"
#include "../../utils.h"

DatasetsGroupLabelsList* initDatasetsGroupLabelsList(DatasetGroupId datasetGroupId) {
    DatasetsGroupLabelsList* datasetsGroupLabelsList =
        (DatasetsGroupLabelsList*)malloc(sizeof(DatasetsGroupLabelsList));
    datasetsGroupLabelsList->datasetsGroupId = datasetGroupId;
    datasetsGroupLabelsList->labels = initVector();
    datasetsGroupLabelsList->seekPosition = -1;
    return datasetsGroupLabelsList;
}

void freeDatasetsGroupLabelsList(DatasetsGroupLabelsList* datasetsGroupLabelsList) {
    size_t numberLabels = getSize(datasetsGroupLabelsList->labels);
    for (size_t label_i = 0; label_i < numberLabels; label_i++) {
        Label* label = getValue(datasetsGroupLabelsList->labels, label_i);
        freeLabel(label);
    }
    freeVector(datasetsGroupLabelsList->labels);
    free(datasetsGroupLabelsList);
}

void addLabel(DatasetsGroupLabelsList* datasetsGroupLabelsList, Label* label) {
    pushBack(datasetsGroupLabelsList->labels, label);
}

uint64_t getSizeContentDatasetsGroupLabelsList(DatasetsGroupLabelsList* datasetsGroupLabelsList) {
    uint64_t contentSize = sizeof(datasetsGroupLabelsList->datasetsGroupId);
    contentSize += sizeof(uint16_t);  // numberLabels
    size_t numberLabels = getSize(datasetsGroupLabelsList->labels);
    for (size_t label_i = 0; label_i < numberLabels; label_i++) {
        Label* label = getValue(datasetsGroupLabelsList->labels, label_i);
        contentSize += getSizeLabel(label);
    }
    return contentSize;
}

uint64_t getSizeDatasetsGroupLabelsList(DatasetsGroupLabelsList* datasetsGroupLabelsList) {
    return 12 + getSizeContentDatasetsGroupLabelsList(datasetsGroupLabelsList);
}

bool writeContentDatasetsGroupLabelsList(DatasetsGroupLabelsList* datasetsGroupLabelsList, FILE* outputFile) {
    bool datasetGroupIdSuccessfulWrite = utils_write(datasetsGroupLabelsList->datasetsGroupId, outputFile);
    size_t numberLabels = getSize(datasetsGroupLabelsList->labels);
    bool numberLabelsSuccessfulWrite = writeBigEndian16ToFile((uint16_t)numberLabels, outputFile);
    if (!datasetGroupIdSuccessfulWrite || !numberLabelsSuccessfulWrite) {
        fprintf(stderr, "Error writing dataset id or number of labels.\n");
        return false;
    }

    for (size_t label_i = 0; label_i < numberLabels; label_i++) {
        Label* label = getValue(datasetsGroupLabelsList->labels, label_i);
        if (!writeLabel(label, outputFile)) {
            fprintf(stderr, "Error writing label.\n");
            return false;
        };
    }
    return true;
}

bool writeDatasetsGroupLabelsList(DatasetsGroupLabelsList* datasetsGroupLabelsList, FILE* outputFile) {
    uint64_t datasetsGroupLabelsListSize = getSizeDatasetsGroupLabelsList(datasetsGroupLabelsList);
    bool datasetsGroupLabelsListHeaderSuccessfulWrite =
        writeBoxHeader(outputFile, datasetsGroupLabelsListName, datasetsGroupLabelsListSize);
    if (!datasetsGroupLabelsListHeaderSuccessfulWrite) {
        fprintf(stderr, "Error writing datasets group labels list header.\n");
        return false;
    }
    return writeContentDatasetsGroupLabelsList(datasetsGroupLabelsList, outputFile);
}

DatasetsGroupLabelsList* parseDatasetsGroupLabelsList(FILE* inputFile) {
    DatasetGroupId datasetGroupId;
    bool datasetGroupIdRead = utils_read(&datasetGroupId, inputFile);
    uint16_t numberLabels;
    bool numberLabelsRead = readBigEndian16FromFile(&numberLabels, inputFile);
    if (!datasetGroupIdRead || !numberLabelsRead) {
        fprintf(stderr, "Error reading dataset id or number of labels.\n");
        return NULL;
    }
    DatasetsGroupLabelsList* datasetsGroupLabelsList = initDatasetsGroupLabelsList(datasetGroupId);
    datasetsGroupLabelsList->seekPosition = ftell(inputFile);
    for (int label_i = 0; label_i < numberLabels; label_i++) {
        char boxType[4];
        uint64_t boxSize;
        bool labelBoxHeaderSuccessfulWrite = readBoxHeader(inputFile, boxType, &boxSize);
        if (!labelBoxHeaderSuccessfulWrite || strncmp(boxType, datasetsGroupLabelName, 4) != 0) {
            freeDatasetsGroupLabelsList(datasetsGroupLabelsList);
            fprintf(stderr, "Error reading label: could not read header lbll.\n");
            return NULL;
        }
        Label* label = parseLabel(inputFile);
        if (label == NULL) {
            freeLabel(label);
            freeDatasetsGroupLabelsList(datasetsGroupLabelsList);
            fprintf(stderr, "Error reading label.\n");
        } else {
            addLabel(datasetsGroupLabelsList, label);
        }
    }
    return datasetsGroupLabelsList;
}

long getDatasetsGroupLabelsListSeekPosition(DatasetsGroupLabelsList* datasetsGroupLabelsList) {
    return datasetsGroupLabelsList->seekPosition;
}
