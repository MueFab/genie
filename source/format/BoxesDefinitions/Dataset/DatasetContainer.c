//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include <string.h>
#include <DataUnits/DataUnits.h>
#include <Boxes.h>
#include "../../Boxes.h"
#include "../../utils.h"

DatasetContainer *initDatasetContainer() {
    DatasetContainer* datasetContainer = (DatasetContainer*) calloc(1,sizeof(DatasetContainer));

    datasetContainer->datasetHeader = NULL;
    datasetContainer->datasetMasterIndexTable = NULL;
    datasetContainer->datasetParameters = NULL;
    datasetContainer->datasetMetadata = NULL;
    datasetContainer->datasetProtection = NULL;
    datasetContainer->streamContainers = initVector();
    datasetContainer->accessUnitContainers = initVector();
    datasetContainer->seekPosition = -1;
    datasetContainer->accessUnitsOffsets = NULL;
    return datasetContainer;
}

void freeDatasetParametersVector(Vector* parametersVector){
    for(size_t parameters_i = 0; parameters_i<getSize(parametersVector); parameters_i++){
        DatasetParameters* datasetParameters = getValue(parametersVector, parameters_i);
        if (datasetParameters != NULL){
            freeDatasetParameters(datasetParameters);
        }
    }
    freeVector(parametersVector);
}

void freeDatasetContainer(DatasetContainer *datasetContainer) {
    if(datasetContainer->datasetMasterIndexTable != NULL){
        freeDatasetMasterIndexTable(datasetContainer->datasetMasterIndexTable);
    }
    if(datasetContainer->datasetParameters != NULL){
        freeDatasetParametersVector(datasetContainer->datasetParameters);
    }
    if(datasetContainer->datasetMetadata != NULL){
        freeDatasetMetadata(datasetContainer->datasetMetadata);
    }
    if(datasetContainer->datasetProtection != NULL){
        freeDatasetProtection(datasetContainer->datasetProtection);
    }

    if(datasetContainer->streamContainers != NULL) {
        for (size_t streamContainer_i = 0; streamContainer_i < getSize(datasetContainer->streamContainers);
             streamContainer_i++) {
            StreamContainer *streamContainer = getValue(datasetContainer->streamContainers, streamContainer_i);
            if (streamContainer != NULL) {
                freeStreamContainer(streamContainer);
            }
        }
        freeVector(datasetContainer->streamContainers);
    }

    if(datasetContainer->accessUnitContainers != NULL) {
        for (size_t accessUnit_i = 0; accessUnit_i < getSize(datasetContainer->accessUnitContainers); accessUnit_i++) {
            AccessUnitContainer *accessUnitContainer= getValue(datasetContainer->accessUnitContainers, accessUnit_i);
            if (accessUnitContainer != NULL) {
                freeAccessUnitContainer(accessUnitContainer);
            }
        }
        freeVector(datasetContainer->accessUnitContainers);
    }

    if(datasetContainer->accessUnitsOffsets != NULL) {
        for (uint16_t seqId = 0; seqId < datasetContainer->datasetHeader->sequencesCount; seqId++) {
            uint8_t num_classes = getClassesCount(datasetContainer->datasetHeader);
            num_classes -= hasClassType(datasetContainer->datasetHeader, U_TYPE_AU);
            for (uint8_t class_i = 0; class_i < num_classes; class_i++) {
                free(datasetContainer->accessUnitsOffsets[seqId][class_i]);
            }
            free(datasetContainer->accessUnitsOffsets[seqId]);
        }
        free(datasetContainer->accessUnitsOffsets);
    }

    if(datasetContainer->accessUnitsContainers != NULL) {
        for(uint16_t seqId=0; seqId<datasetContainer->datasetHeader->sequencesCount; seqId++){
            uint8_t num_classes = getClassesCount(datasetContainer->datasetHeader);
            num_classes -=  hasClassType(datasetContainer->datasetHeader, U_TYPE_AU);
            for(uint8_t class_i=0; class_i<num_classes; class_i++){
                free(datasetContainer->accessUnitsContainers[seqId][class_i]);
            }
            free(datasetContainer->accessUnitsContainers[seqId]);
        }
        free(datasetContainer->accessUnitsContainers);
    }

    if(datasetContainer->datasetHeader != NULL){
        freeDatasetHeader(datasetContainer->datasetHeader);
    }
    free(datasetContainer);
}

void addAccessUnitToDataset(DatasetContainer* datasetContainer, AccessUnitContainer* accessUnitContainer){
    if(datasetContainer->accessUnitContainers == NULL){
        datasetContainer->accessUnitContainers = initVector();
    }
    pushBack(datasetContainer->accessUnitContainers, accessUnitContainer);

}

void addStreamContainerToDataset(DatasetContainer* datasetContainer, StreamContainer* streamContainer){
    if(datasetContainer->streamContainers == NULL){
        datasetContainer->streamContainers = initVector();
    }
    pushBack(datasetContainer->streamContainers, streamContainer);
}

size_t getNumberStreamContainer(DatasetContainer* datasetContainer){
    return getSize(datasetContainer->streamContainers);
}

StreamContainer* getStreamContainer(DatasetContainer *datasetContainer, size_t index){
    return getValue(datasetContainer->streamContainers, index);
}

uint64_t getSizeDatasetParametersVector(Vector* parametersVector){
    uint64_t sizeParametersVector = 0;
    for(size_t parameters_i=0; parameters_i < getSize(parametersVector); parameters_i++){
        DatasetParameters* datasetParameters = getValue(parametersVector, parameters_i);
        if(datasetParameters != NULL){
            sizeParametersVector += getSizeDatasetParameters(datasetParameters);
        }
    }
    return sizeParametersVector;
}

uint64_t getSizeContentDatasetContainer(DatasetContainer* datasetContainer){
    uint64_t contentSize = datasetContainer->datasetHeader != NULL ? getSizeDatasetHeader(datasetContainer->datasetHeader) : 0;

    contentSize += datasetContainer->datasetParameters != NULL ?
            getSizeDatasetParametersVector(datasetContainer->datasetParameters) : 0;

    if(datasetContainer->datasetHeader!=NULL && isMITFlagSet(datasetContainer->datasetHeader)) {
        contentSize += datasetContainer->datasetMasterIndexTable != NULL ? getSizeDatasetMasterIndexTable(
                datasetContainer->datasetMasterIndexTable) : 0;
    }

    if (datasetContainer->accessUnitContainers != NULL){
        for (size_t accessUnit_i = 0; accessUnit_i < getSize(datasetContainer->accessUnitContainers); accessUnit_i++) {
            AccessUnitContainer *accessUnitContainer = getValue(datasetContainer->accessUnitContainers, accessUnit_i);
            contentSize += accessUnitContainer != NULL ? getAccessUnitContainerSize(accessUnitContainer) : 0;
        }
    }

    if (datasetContainer->streamContainers != NULL) {
        for (size_t streamContainer_i = 0; streamContainer_i < getSize(datasetContainer->streamContainers);
             streamContainer_i++) {
            StreamContainer *streamContainer = getValue(datasetContainer->streamContainers, streamContainer_i);
            contentSize += streamContainer != NULL ? getSizeStreamContainer(streamContainer) : 0;
        }
    }

    contentSize += datasetContainer->datasetMetadata != NULL ? getSizeDatasetMetadata(datasetContainer->datasetMetadata) : 0;
    contentSize += datasetContainer->datasetProtection != NULL ? getSizeDatasetProtection(datasetContainer->datasetProtection) : 0;

    return contentSize;
}

uint64_t getSizeDatasetContainer(DatasetContainer* datasetContainer){
    return BOX_HEADER_SIZE + getSizeContentDatasetContainer(datasetContainer);
}

bool writeDatasetContainer(DatasetContainer *datasetContainer, FILE *outputFile) {
    /*uint64_t auByteOffset;
    uint64_t descriptorByteOffset;
    bool auByteOffsetFound = getAccessUnitByteOffset(datasetContainer->datasetMasterIndexTable,0,0,0,&auByteOffset);
    bool descriptorByteOffsetFound = getBlockByteOffset(datasetContainer->datasetMasterIndexTable,0,0,0,0,&descriptorByteOffset);
    if (auByteOffsetFound) {
        fprintf(stdout, "AU byte offset: %li\n", auByteOffset);
    }
    if (descriptorByteOffsetFound) {
        fprintf(stdout, "block byte offset: %li\n", descriptorByteOffset);
    }*/

    uint64_t sizeDatasetContainer = getSizeDatasetContainer(datasetContainer);
    if (!writeBoxHeader(outputFile, datasetContainerName, sizeDatasetContainer)){
        fprintf(stderr,"Error writing dataset container header.\n");
        return false;
    };
    return writeContentDatasetContainer(datasetContainer, outputFile);
}

bool writeDatasetParametersVector(Vector* datasetParametersVector, FILE* outputFile){
    for(size_t parameter_i = 0; parameter_i < getSize(datasetParametersVector); parameter_i++){
        DatasetParameters* datasetParameters = getValue(datasetParametersVector, parameter_i);
        if(datasetParameters != NULL){
            if( !writeDatasetParameters(datasetParameters, outputFile) ){
                fprintf(stderr,"Error writing datasetParameters %zu.\n", parameter_i);
                return false;
            }
        }
    }

    return true;
}

bool writeContentDatasetContainer(DatasetContainer* datasetContainer, FILE *outputFile){
    if (datasetContainer->datasetHeader!= NULL) {
        if(!writeDatasetHeader(datasetContainer->datasetHeader, outputFile)){
            fprintf(stderr,"Error writing datasetHeader.\n");
            return false;
        }
    }

    if (datasetContainer->datasetParameters!= NULL) {
        if(!writeDatasetParametersVector(datasetContainer->datasetParameters, outputFile)){
            fprintf(stderr,"Error writing datasetParameters vector.\n");
            return false;
        }
    }

    if(datasetContainer->datasetHeader!=NULL && isMITFlagSet(datasetContainer->datasetHeader)) {
        if (datasetContainer->datasetMasterIndexTable != NULL) {
            if (!writeDatasetMasterIndexTable(datasetContainer->datasetMasterIndexTable, outputFile)) {
                fprintf(stderr, "Error writing datasetMasterIndexTable.\n");
                return false;
            }
        }
    }


    if (datasetContainer->accessUnitContainers != NULL){
        unsigned long numberAccessUnits = getSize(datasetContainer->accessUnitContainers);
        for (size_t accessUnit_i = 0; accessUnit_i < numberAccessUnits; accessUnit_i++) {
            AccessUnitContainer *accessUnitContainer = getValue(datasetContainer->accessUnitContainers, accessUnit_i);
            if (!writeAccessUnitContainer(outputFile, accessUnitContainer)){
                fprintf(stderr, "Error writing access unit container %lu.", accessUnit_i);
            }
        }
    }

    if(datasetContainer->streamContainers != NULL) {
        unsigned long numberStreamsContainers = getSize(datasetContainer->streamContainers);
        for (size_t streamContainer_i = 0; streamContainer_i < numberStreamsContainers; streamContainer_i++) {
            StreamContainer *streamContainer = getValue(datasetContainer->streamContainers, streamContainer_i);
            if (!writeStreamContainer(streamContainer, outputFile)) {
                fprintf(stderr, "Error writing stream container %lu.\n", streamContainer_i);
                return false;
            }
        }
    }

    if (datasetContainer->datasetMetadata!= NULL) {
        if(!writeDatasetMetadata(datasetContainer->datasetMetadata, outputFile)){
            fprintf(stderr,"Error writing datasetMetadata.\n");
            return false;
        }
    }
    if (datasetContainer->datasetProtection!= NULL){
        if(!writeDatasetProtection(datasetContainer->datasetProtection, outputFile)){
            fprintf(stderr,"Error writing datasetProtection.\n");
            return false;
        }
    }
    return true;
}

bool isDatasetContainerValid(DatasetContainer *datasetContainer){
    if (
        datasetContainer->datasetHeader== NULL ||
                (datasetContainer->datasetParameters == NULL && datasetContainer->datasetHeader->datasetType != 2)
    ){
        return false;
    }

    if(datasetContainer->streamContainers != NULL) {
        unsigned long numberStreamsContainers = getSize(datasetContainer->streamContainers);
        for (size_t streamContainer_i = 0; streamContainer_i < numberStreamsContainers; streamContainer_i++) {
            StreamContainer *streamContainer = getValue(datasetContainer->streamContainers, streamContainer_i);
            if (streamContainer != NULL) {
                if (!isStreamContainerValid(streamContainer)) {
                    fprintf(stderr, "Stream container %lu is not valid.\n", streamContainer_i);
                    return false;
                }
            }
        }
    }
    return true;
}

void setDatasetHeader(DatasetContainer* datasetContainer, DatasetHeader *datasetHeader){
    datasetContainer->datasetHeader = datasetHeader;
}

DatasetHeader* getDatasetHeader(DatasetContainer* datasetContainer){
    return datasetContainer->datasetHeader;
}

void setDatasetMasterIndexTable(DatasetContainer* datasetContainer, DatasetMasterIndexTable *datasetMasterIndexTable){
    datasetContainer->datasetMasterIndexTable = datasetMasterIndexTable;
}

void setDatasetParameters(DatasetContainer* datasetContainer, Vector *datasetParameters){
    datasetContainer->datasetParameters = datasetParameters;
}

void setDatasetMetadata(DatasetContainer* datasetContainer, DatasetMetadata *datasetMetadata){
    datasetContainer->datasetMetadata = datasetMetadata;
}

void setDatasetProtection(DatasetContainer* datasetContainer, DatasetProtection *datasetProtection){
    datasetContainer->datasetProtection = datasetProtection;
}

DatasetContainer *parseDatasetContainer(uint64_t boxContentSize, FILE *inputFile, char *fileName) {
    char type[5];
    uint64_t boxSize;
    DatasetContainer* datasetContainer = initDatasetContainer();
    datasetContainer->seekPosition = ftell(inputFile);

    enum PreviousState {
        init,
        header,
        master_index,
        parameter,
        labels,
        descriptor_stream,
        dataset_metadata,
        dataset_protection,
        accessUnit_container
    }  previousState = init;

    FILE* debugMemory = fopen("/tmp/debugMemory","wb");
    FILE* debugMemory2 = fopen("/tmp/debugMemory2","wb");

    while(getSizeContentDatasetContainer(datasetContainer)<boxContentSize){
        if(!readBoxHeader(inputFile,type, &boxSize)){
            fprintf(stderr,"Dataset container content could not be read.\n");
            freeDatasetContainer(datasetContainer);
            return NULL;
        }
        if (strncmp(type,datasetHeaderName,4)==0){
            if (previousState != init){
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            previousState = header;
            DatasetHeader* datasetHeader = parseDatasetHeader(inputFile);
            if(datasetHeader== NULL){
                fprintf(stderr,"Error reading dataset header.\n");
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            setDatasetHeader(datasetContainer, datasetHeader);
            datasetContainer->accessUnitsOffsets =
                    (uint64_t***)calloc(datasetHeader->sequencesCount, sizeof(uint64_t**));
            fprintf(debugMemory,"%hu\n", datasetHeader->sequencesCount);
            datasetContainer->accessUnitsContainers =
                    (AccessUnitContainer****)calloc(datasetHeader->sequencesCount, sizeof(AccessUnitContainer***));
            for(uint16_t seqId=0; seqId<datasetHeader->sequencesCount; seqId++){
                uint8_t num_classes = getClassesCount(datasetHeader);
                num_classes -=  hasClassType(datasetHeader, U_TYPE_AU);

                datasetContainer->accessUnitsOffsets[seqId] =
                        (uint64_t**)calloc(
                                num_classes,
                                sizeof(uint64_t*)
                        );
                fprintf(debugMemory,"%hu: %hu\n", seqId, num_classes);
                datasetContainer->accessUnitsContainers[seqId] =
                        (AccessUnitContainer***)calloc(
                                num_classes,
                                sizeof(AccessUnitContainer**)
                        );
                for(uint8_t class_i=0; class_i<num_classes; class_i++){
                    uint32_t numBlocks = getBlocksInSequence(datasetHeader, seqId);
                    datasetContainer->accessUnitsOffsets[seqId][class_i] =
                            (uint64_t*)calloc(
                                    numBlocks,
                                    sizeof(uint64_t)
                            );
                    fprintf(debugMemory,"%hu, %hu: %hu\n", seqId, class_i, numBlocks);
                    datasetContainer->accessUnitsContainers[seqId][class_i] =
                            (AccessUnitContainer**)calloc(
                                    numBlocks,
                                    sizeof(AccessUnitContainer*)
                            );
                }
            }

        }else if (strncmp(type,datasetMasterIndexTableName,4)==0){
            if (previousState != parameter){
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            previousState = master_index;
            DatasetMasterIndexTable* datasetMasterIndexTable = parseDatasetMasterIndexTable(datasetContainer, inputFile);
            if(datasetMasterIndexTable== NULL){
                fprintf(stderr,"Error reading dataset header.\n");
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            setDatasetMasterIndexTable(datasetContainer, datasetMasterIndexTable);

        }else if (strncmp(type,datasetParametersName,4)==0){
            if (!(previousState == header || previousState == parameter)){
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            previousState = parameter;

            DatasetParameters* datasetParameters = parseDatasetParameters(boxSize - 12, inputFile);
            if(datasetParameters== NULL){
                fprintf(stderr,"Error reading dataset parameters.\n");
                freeDatasetContainer(datasetContainer);
                return NULL;
            }

            Vector* datasetParametersVector = initVector();
            pushBack(datasetParametersVector, datasetParameters);

            setDatasetParameters(datasetContainer, datasetParametersVector);

        }else if (strncmp(type,datasetMetadataName,4)==0){
            if (!(previousState == descriptor_stream || previousState == accessUnit_container)){
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            previousState = dataset_metadata;
            DatasetMetadata* datasetMetadata = parseDatasetMetadata(boxSize - 12, inputFile);
            if(datasetMetadata== NULL){
                fprintf(stderr,"Error reading dataset metadata.\n");
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            setDatasetMetadata(datasetContainer, datasetMetadata);

        }else if (strncmp(type,datasetProtectionName,4)==0){
            if (!(previousState == descriptor_stream || previousState == accessUnit_container ||
                    previousState == dataset_metadata)){
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            previousState = dataset_protection;
            DatasetProtection* datasetProtection =
                    parseDatasetProtection(boxSize - 12, inputFile);
            if(datasetProtection== NULL){
                fprintf(stderr,"Error reading dataset protection.\n");
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            setDatasetProtection(datasetContainer, datasetProtection);

        }else if (strncmp(type,accessUnitContainerName,4)==0){
            if (!(previousState==parameter || previousState == master_index || previousState == accessUnit_container)){
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            size_t aucn_offset = ftell(inputFile)-(size_t)12;
            previousState = accessUnit_container;
            AccessUnitContainer* accessUnitContainer = parseAccessUnitContainer(boxSize - 12, inputFile, fileName, datasetContainer);
            if(accessUnitContainer == NULL){
                fprintf(stderr,"Error reading access unit container.\n");
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            addAccessUnitToDataset(datasetContainer, accessUnitContainer);
            uint8_t auType = (uint8_t) (accessUnitContainer->accessUnitHeader->au_type - 1);
            uint32_t accessUnitID = accessUnitContainer->accessUnitHeader->access_unit_ID;

            uint16_t sequencesCount = getSequencesCount(datasetContainer->datasetHeader);
            bool foundOffset = true;
            for(uint16_t sequence_i=0; sequence_i<sequencesCount; sequence_i++){
                if(accessUnitID < getBlocksInSequence(datasetContainer->datasetHeader, sequence_i)) {
                    if (datasetContainer->accessUnitsOffsets[sequence_i][auType][accessUnitID] ==
                        aucn_offset - datasetContainer->seekPosition
                            ) {
                        foundOffset = true;
                        datasetContainer->accessUnitsContainers[sequence_i][auType][accessUnitID] = accessUnitContainer;
                        break;
                    }
                }
            }
            if(!foundOffset){
                fprintf(stderr,"access unit container offset was not found.\n");
                return NULL;
            }

        }else if (strncmp(type,streamContainerName,4)==0){
            if (!(previousState == accessUnit_container || previousState == descriptor_stream ||
                    previousState == master_index || previousState == parameter)){
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            previousState = descriptor_stream;
            StreamContainer* streamContainer = parseStreamContainer(boxSize - 12, inputFile, fileName, datasetContainer);
            if(streamContainer== NULL){
                fprintf(stderr,"Error reading stream container.\n");
                freeDatasetContainer(datasetContainer);
                return NULL;
            }
            addStreamContainerToDataset(datasetContainer, streamContainer);

        } else {
            fprintf(stderr,"Dataset container: read unknown structure.\n");
            freeDatasetContainer(datasetContainer);
            return NULL;
        }
    }
    fclose(debugMemory);
    fclose(debugMemory2);
    return datasetContainer;
}

long getDatasetContainerSeekPosition(DatasetContainer* datasetContainer){
    return datasetContainer->seekPosition;
}

bool generateDatasetSeekPoints(DatasetContainer* datasetContainer){
    fprintf(
            stdout,
            "dataset @ %li-%li\n",
            getDatasetContainerSeekPosition(datasetContainer),
            getDatasetContainerSeekPosition(datasetContainer) +
                    getSizeContentDatasetContainer(datasetContainer)
    );
    if (datasetContainer->datasetHeader!= NULL) {
        fprintf(
                stdout,
                "\tdataset header @ %li-%li\n",
                getDatasetHeaderSeekPosition(datasetContainer->datasetHeader),
                getDatasetHeaderSeekPosition(datasetContainer->datasetHeader) +
                    getSizeContentDatasetHeader(datasetContainer->datasetHeader)
        );
    }
    if (datasetContainer->datasetMasterIndexTable!= NULL) {
        fprintf(
                stdout,
                "\tmaster index table: @ %li-%li\n",
                getDatasetMasterIndexTableSeekPosition(datasetContainer->datasetMasterIndexTable),
                getDatasetMasterIndexTableSeekPosition(datasetContainer->datasetMasterIndexTable) +
                getSizeContentDatasetMasterIndexTable(datasetContainer->datasetMasterIndexTable)
        );
    }
    if (datasetContainer->datasetParameters!= NULL) {
        for(
                size_t datasetParameter_i=0;
                datasetParameter_i<getSize(datasetContainer->datasetParameters);
                datasetParameter_i++
        ) {
            DatasetParameters* datasetParameters = getValue(datasetContainer->datasetParameters, datasetParameter_i);
            if(datasetParameters == NULL) {
                fprintf(
                        stdout,
                        "\tdataset parameters @ %li-%li\n",
                        getDatasetParametersSeekPosition(datasetParameters),
                        getDatasetParametersSeekPosition(datasetParameters) +
                        getSizeContentDatasetParameters(datasetParameters)
                );
            }
        }
    }

    if (datasetContainer->streamContainers != NULL) {
        unsigned long numberStreamsContainers = getSize(datasetContainer->streamContainers);
        for (size_t streamContainer_i = 0; streamContainer_i < numberStreamsContainers; streamContainer_i++) {
            StreamContainer *streamContainer = getValue(datasetContainer->streamContainers, streamContainer_i);
            generateStreamSeekPoints(streamContainer);
        }
    }


    if (datasetContainer->datasetMetadata!= NULL) {
        fprintf(
                stdout,
                "\tdataset metadata @ %li-%li\n",
                getDatasetMetadataSeekPosition(datasetContainer->datasetMetadata),
                getDatasetMetadataSeekPosition(datasetContainer->datasetMetadata) +
                getSizeContentDatasetMetadata(datasetContainer->datasetMetadata)
        );
    }
    if (datasetContainer->datasetProtection!= NULL){
        fprintf(
                stdout,
                "\tdataset protection @ %li-%li\n",
                getDatasetProtectionSeekPosition(datasetContainer->datasetProtection),
                getDatasetProtectionSeekPosition(datasetContainer->datasetProtection) +
                getSizeContentDatasetProtection(datasetContainer->datasetProtection)
        );
    }
    return true;
}

Vector* getDataUnitAccessUnits(DatasetContainer* datasetContainer){
    if (datasetContainer == NULL){
        return NULL;
    }

    for(uint16_t sequence_i=0; sequence_i<getSequencesCount(datasetContainer->datasetHeader); sequence_i++){
        uint8_t numClasses = getClassesCount(datasetContainer->datasetHeader);
        numClasses -= hasClassType(getDatasetHeader(datasetContainer), U_TYPE_AU) ? 1 : 0;
        for(uint8_t class_i=0; class_i<numClasses; class_i++){
            uint32_t numberAUs = getBlocksInSequence(datasetContainer->datasetHeader, sequence_i);
            for(uint32_t au_i=0; au_i<numberAUs; au_i++){
                DataUnitAccessUnit* dataUnitAccessUnit = getDataUnitAccessUnit(datasetContainer, sequence_i, class_i, au_i);
            }
        }
    }
}

DataUnitAccessUnit* getDataUnitAccessUnit(DatasetContainer *datasetContainer, uint16_t sequence_index, uint8_t class_index,
                                          uint32_t au_id){
    DatasetMasterIndexTable* datasetMasterIndexTable = datasetContainer->datasetMasterIndexTable;
    if (datasetMasterIndexTable == NULL){
        return NULL;
    }

    AccessUnitContainer* accessUnitContainer =
            datasetContainer->accessUnitsContainers[sequence_index][class_index][au_id];
    if(accessUnitContainer == NULL) {
        return NULL;
    }
    AccessUnitHeader* accessUnitHeader = accessUnitContainer->accessUnitHeader;
    uint32_t accessUnitId = getAccessUnitId(accessUnitHeader);
    uint8_t numBlocks = getNumBlocks(accessUnitHeader);
    uint16_t parameterSetId = getParametersSetId(accessUnitHeader);
    uint32_t readsCount = getReadsCount(accessUnitHeader);
    uint16_t mmThreshold = getMMThreshold(accessUnitHeader);
    uint32_t mmCount = getMMCount(accessUnitHeader);
    uint64_t auStartPosition = getAUStartPosition(datasetMasterIndexTable, sequence_index, class_index, au_id);
    uint64_t auEndPosition = getAUEndPosition(datasetMasterIndexTable, sequence_index, class_index, au_id);
    uint64_t auExtendedStartPosition = getAUExtendedStartPosition(datasetMasterIndexTable, sequence_index, class_index, au_id);
    uint64_t auExtendedEndPosition = getAUExtendedEndPosition(datasetMasterIndexTable, sequence_index, class_index, au_id);

    uint8_t class_Id = getClassType(getDatasetHeader(datasetContainer), class_index);
    uint16_t sequence_Id = getSequenceId(getDatasetHeader(datasetContainer), sequence_index);

    DataUnitAccessUnit* dataUnitAccessUnit = initDataUnitAccessUnit(
            accessUnitId,
            numBlocks,
            parameterSetId,
            class_Id,
            readsCount,
            mmThreshold,
            mmCount,
            sequence_Id,
            auStartPosition,
            auEndPosition,
            auExtendedStartPosition,
            auExtendedEndPosition
    );

    Vector* blocks = getBlocks(accessUnitContainer);
    if(blocks != NULL){
        for(size_t block_i=0; block_i<getSize(blocks); block_i++){
            Block* block = getValue(blocks, block_i);
            uint32_t blockSize = (uint32_t) (block->payload->endPos - block->payload->startPos);
            DataUnitBlockHeader* blockHeader = initDataUnitBlockHeader(block->blockHeader->descriptorId, blockSize);
            if(block != NULL && blockHeader != NULL){
                addBlockToDataUnitAccessUnit(dataUnitAccessUnit, block, blockHeader);
            }else{
                freeBlock(block);
                free(blockHeader);
            }
        }
    }else{
        uint8_t numBlocksToAdd = getNumberDescriptorsInClass(getDatasetHeader(datasetContainer), class_index);
        for(uint8_t block_i=0; block_i<numBlocksToAdd; block_i++){
            uint8_t descriptorId = getDescriptorIdInClass(getDatasetHeader(datasetContainer), class_index, block_i);

            uint64_t block_start_position;
            bool block_start_found = getBlockByteOffset(
                    datasetMasterIndexTable,
                    sequence_index,
                    class_index,
                    au_id,
                    block_i,
                    &block_start_position
            );
            if(!block_start_found){
                fprintf(stderr, "start block offset for requested block cannot be found.\n");
                return NULL;
            }

            uint64_t block_end_position;
            bool block_end_found = getNextBlockByteOffset(
                    datasetMasterIndexTable,
                    sequence_index,
                    class_index,
                    au_id,
                    block_i,
                    &block_end_position
            );
            if(!block_end_found){
                fprintf(stderr, "end block offset for requested block cannot be found.\n");
                return NULL;
            }
            FromFile* fromFile = initFromFileWithFilenameAndBoundaries(
                    /*todo change this hardcoded 0*/
                    ((StreamContainer*)getValue(datasetContainer->streamContainers,0))->dataFromFile->filename,
                    block_start_position + datasetContainer->seekPosition,
                    block_end_position + datasetContainer->seekPosition
            );

            Block* block = initBlock(datasetContainer, fromFile);
            uint32_t blockSize = (uint32_t) getFromFileSize(fromFile);
            DataUnitBlockHeader* blockHeader = initDataUnitBlockHeader(descriptorId, blockSize);
            if(block != NULL && blockHeader != NULL){
                addBlockToDataUnitAccessUnit(dataUnitAccessUnit, block, blockHeader);
            }
        }
    }
    return dataUnitAccessUnit;

}