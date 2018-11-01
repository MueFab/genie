//
// Created by gencom on 25/07/17.
//

#include <stdlib.h>
#include <string.h>
#include <Boxes.h>
#include <treeUint64.h>
#include "../../Boxes.h"
#include "../../utils.h"
#include "../../DataStructures/BitStreams/InputBitstream.h"
#include "../../DataStructures/Signatures/SignatureIntegerInputStream.h"
#include "../../DataStructures/Signatures/SignatureSizeComputation.h"
#include "../../DataStructures/BitStreams/OutputBitstream.h"
#include "../../DataStructures/Signatures/SignatureIntegerOutputStream.h"

void initIndexUnaligned(DatasetMasterIndexTable* datasetMasterIndexTable){
    DatasetContainer* datasetContainer = datasetMasterIndexTable->datasetContainer;

    if(!isUnmappedIndexingFlagSet(getDatasetHeader(datasetContainer))){
        return;
    }
    uint32_t numberUAccessUnits = getNumberUAccessUnits(getDatasetHeader(datasetContainer));
    datasetMasterIndexTable->uAccessUnitsIndex.signatures = (Signatures **)malloc(numberUAccessUnits*sizeof(Signatures*));
    datasetMasterIndexTable->uAccessUnitsIndex.uAccessUnitOffsets = (uint64_t*)calloc(numberUAccessUnits,sizeof(uint64_t));
    datasetMasterIndexTable->uAccessUnitsIndex.uAccessUnitSizes = (uint32_t*)calloc(numberUAccessUnits,sizeof(uint32_t));

    uint8_t unalignedClassId = 5;
    uint16_t numberDescriptorsUnmapped =
            getNumberDescriptorsInClass(getDatasetHeader(datasetContainer), unalignedClassId);
    datasetMasterIndexTable->uAccessUnitsIndex.byteOffset =
            (uint64_t * *)malloc(numberDescriptorsUnmapped*sizeof(uint64_t*));
    bool allCorrect=true;
    if (datasetMasterIndexTable->uAccessUnitsIndex.byteOffset != NULL) {
        for (int descriptorUnalignedAccessUnit_i = 0;
             descriptorUnalignedAccessUnit_i < numberDescriptorsUnmapped; descriptorUnalignedAccessUnit_i++) {
            datasetMasterIndexTable->uAccessUnitsIndex.byteOffset[descriptorUnalignedAccessUnit_i] = (uint64_t *) malloc(
                    getNumberUAccessUnits(getDatasetHeader(datasetContainer)) * sizeof(uint64_t));
            if (datasetMasterIndexTable->uAccessUnitsIndex.byteOffset[descriptorUnalignedAccessUnit_i] == NULL) {
                allCorrect = false;
                break;
            }
        }
        if(!allCorrect){
            for(int descriptorUnalignedAccessUnit_i=0; descriptorUnalignedAccessUnit_i<numberDescriptorsUnmapped; descriptorUnalignedAccessUnit_i++){
                free(datasetMasterIndexTable->uAccessUnitsIndex.byteOffset[descriptorUnalignedAccessUnit_i]);
            }
            free(datasetMasterIndexTable->uAccessUnitsIndex.byteOffset);
            datasetMasterIndexTable->uAccessUnitsIndex.byteOffset = NULL;
        }
    }


    if(datasetMasterIndexTable->uAccessUnitsIndex.signatures!= NULL) {
        datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits = getNumberUAccessUnits(getDatasetHeader(datasetContainer));
    }else{
        datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits = 0;
    }

    for(uint32_t uAccessUnit_i=0; uAccessUnit_i<datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits; uAccessUnit_i++){
        datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i] = initSignatures(
                getMultipleSignatureBase(getDatasetHeader(datasetContainer))
        );
    }
}

DatasetMasterIndexTable* initDatasetMasterIndexTable(DatasetContainer *datasetContainer){
    DatasetMasterIndexTable* datasetMasterIndexTable = (DatasetMasterIndexTable*)malloc(sizeof(DatasetMasterIndexTable));
    datasetMasterIndexTable->seekPosition = -1;
    datasetMasterIndexTable->datasetContainer=datasetContainer;


    datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits = 0;
    datasetMasterIndexTable->uAccessUnitsIndex.signatures = NULL;
    datasetMasterIndexTable->uAccessUnitsIndex.uAccessUnitOffsets = NULL;
    datasetMasterIndexTable->uAccessUnitsIndex.uAccessUnitSizes = NULL;
    datasetMasterIndexTable->uAccessUnitsIndex.byteOffset = NULL;

    uint16_t sequences_count = getSequencesCount(getDatasetHeader(datasetContainer));
    datasetMasterIndexTable->sequence_indexes.sequence_indexes = (Sequence_index *)malloc(sequences_count*sizeof(Sequence_index));
    if (datasetMasterIndexTable->sequence_indexes.sequence_indexes == NULL){
        datasetMasterIndexTable->sequence_indexes.number_sequences = 0;
        return NULL;
    }
    datasetMasterIndexTable->sequence_indexes.number_sequences = sequences_count;

    for(uint16_t seq_id=0; seq_id<sequences_count; seq_id++){
        uint16_t classesCountForAligned = getClassesCount(getDatasetHeader(datasetContainer));
        if(hasClassType(getDatasetHeader(datasetContainer),CLASS_U)){
            classesCountForAligned--;
        }
        datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].number_of_classes = classesCountForAligned;
        datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex=
                (AUs_index *)malloc(classesCountForAligned*sizeof(AUs_index));

        uint32_t numberAUs = getBlocksInSequence(getDatasetHeader(datasetContainer), seq_id);
        for(uint8_t classId = 0; classId<classesCountForAligned; classId++){
            datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex[classId].au_indexings =
                    (AU_indexing*)calloc(numberAUs,sizeof(AU_indexing));
            datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex[classId].number_AUs=numberAUs;

            if(!isBlockHeaderFlagSet(getDatasetHeader(datasetContainer))) {
                uint16_t numberDescriptors = getNumberDescriptorsInClass(getDatasetHeader(datasetContainer), classId);
                for (typeof(numberAUs) auId = 0; auId < numberAUs; auId++) {
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex[classId].au_indexings[auId]
                            .block_byte_offset.descriptors_number = numberDescriptors;
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex[classId].au_indexings[auId]
                            .block_byte_offset.block_byte_offset = (uint64_t *) malloc(
                            numberDescriptors * sizeof(uint64_t));
                }
            } else {
                for (typeof(numberAUs) auId = 0; auId < numberAUs; auId++) {
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex[classId].au_indexings[auId]
                            .block_byte_offset.descriptors_number = 0;
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex[classId].au_indexings[auId]
                            .block_byte_offset.block_byte_offset = NULL;
                }
            }
        }
    }
    if(!isBlockHeaderFlagSet(getDatasetHeader(datasetContainer))) {
        datasetMasterIndexTable->offsetsPerClass = (OffsetsPerClass*)malloc(sizeof(OffsetsPerClass));
        uint16_t classesCountForAligned = getClassesCount(getDatasetHeader(datasetContainer));
        if(hasClassType(getDatasetHeader(datasetContainer),CLASS_U)){
            classesCountForAligned--;
        }
        datasetMasterIndexTable->offsetsPerClass->offsetsPerClass =
                malloc(classesCountForAligned* sizeof(OffsetsPerStream));
        datasetMasterIndexTable->offsetsPerClass->numberClasses = classesCountForAligned;
        for(uint8_t class_i = 0; class_i<classesCountForAligned; class_i++){
            uint16_t numberDescriptors = getNumberDescriptorsInClass(getDatasetHeader(datasetContainer), class_i);
            datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].offsetsPerStream =
                    (Tree**)calloc(numberDescriptors, sizeof(Tree*));
            datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].sizeTrees =
                    (uint64_t*)calloc(numberDescriptors, sizeof(uint64_t));
            datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].numberStreams = numberDescriptors;
            for(uint16_t stream_i = 0; stream_i<numberDescriptors; stream_i++){
                datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].offsetsPerStream[stream_i]=initTree();
            }
        }
    }else{
        datasetMasterIndexTable->offsetsPerClass = NULL;
    }

    initIndexUnaligned(datasetMasterIndexTable);
    return datasetMasterIndexTable;
}

void freeDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable){
    DatasetContainer* datasetContainer = datasetMasterIndexTable->datasetContainer;

    if(datasetMasterIndexTable->offsetsPerClass != NULL) {
        uint16_t classesCountForAligned = getClassesCount(getDatasetHeader(datasetContainer));
        if (hasClassType(getDatasetHeader(datasetContainer), CLASS_U)) {
            classesCountForAligned--;
        }

        datasetMasterIndexTable->offsetsPerClass->numberClasses = classesCountForAligned;
        for (uint8_t class_i = 0; class_i < classesCountForAligned; class_i++) {
            uint16_t numberDescriptors = getNumberDescriptorsInClass(getDatasetHeader(datasetContainer), class_i);

            for(uint16_t descriptor_i=0; descriptor_i<numberDescriptors; descriptor_i++){
                if(
                    numberElements(datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].offsetsPerStream[descriptor_i])
                    != datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].sizeTrees[descriptor_i]
                ){
                    fprintf(stderr, "lost elements.\n");
                }
                freeTree(datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].offsetsPerStream[descriptor_i]);
            }
            free(datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].offsetsPerStream);
            free(datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].sizeTrees);
        }
        free(datasetMasterIndexTable->offsetsPerClass->offsetsPerClass);
        free(datasetMasterIndexTable->offsetsPerClass);
    }

    for(uint16_t seq_id=0; seq_id<datasetMasterIndexTable->sequence_indexes.number_sequences; seq_id++){
        uint16_t classesCountForAligned = getClassesCount(getDatasetHeader(datasetMasterIndexTable->datasetContainer));
        if(hasClassType(getDatasetHeader(datasetMasterIndexTable->datasetContainer),CLASS_U)){
            classesCountForAligned--;
        }

        uint32_t numberAUs = getBlocksInSequence(getDatasetHeader(datasetMasterIndexTable->datasetContainer), seq_id);
        for(uint8_t classId = 0; classId<classesCountForAligned; classId++){
            for(typeof(numberAUs) auId =0; auId<numberAUs; auId++){
                free(datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex[classId].au_indexings[auId]
                                .block_byte_offset.block_byte_offset
                );
            }
            free(datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex[classId].au_indexings);
        }
        free(datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex);
    }
    free(datasetMasterIndexTable->sequence_indexes.sequence_indexes);

    for(uint32_t uAccessUnit_i=0; uAccessUnit_i<datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits; uAccessUnit_i++){
        freeSignatures(datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i]);
    }
    free(datasetMasterIndexTable->uAccessUnitsIndex.signatures);

    uint8_t unalignedClassId = U_TYPE_AU;

    uint8_t unalignedClass_index;
    if (
            getClassIndexForType(
                    getDatasetHeader(datasetMasterIndexTable->datasetContainer),
                    unalignedClassId,
                    &unalignedClass_index)
    ) {
        uint16_t numberDescriptorsUnmapped =
                getNumberDescriptorsInClass(getDatasetHeader(datasetMasterIndexTable->datasetContainer),
                                            unalignedClass_index);
        for (int descriptorUnalignedAccessUnit_i = 0;
             descriptorUnalignedAccessUnit_i < numberDescriptorsUnmapped; descriptorUnalignedAccessUnit_i++) {
            free(datasetMasterIndexTable->uAccessUnitsIndex.byteOffset[descriptorUnalignedAccessUnit_i]);
        }
        free(datasetMasterIndexTable->uAccessUnitsIndex.byteOffset);
        free(datasetMasterIndexTable->uAccessUnitsIndex.uAccessUnitOffsets);
        free(datasetMasterIndexTable->uAccessUnitsIndex.uAccessUnitSizes);
    }
    free(datasetMasterIndexTable);
}

bool writeAlignedIndexing(DatasetMasterIndexTable* datasetMasterIndexTable, OutputBitstream* outputBitstream){
    DatasetHeader* datasetHeader = getDatasetHeader(datasetMasterIndexTable->datasetContainer);
    for(
        typeof(datasetMasterIndexTable->sequence_indexes.number_sequences) seqId = 0;
        seqId<datasetMasterIndexTable->sequence_indexes.number_sequences;
        seqId++
        ){
        Classes_count classNum = getClassesCount(datasetHeader);
        DatasetType datasetType = getDatasetType(datasetHeader);

        for(Classes_count class_it = 0; class_it<classNum; class_it++){
            Class_type classType = getClassType(datasetHeader, class_it);
            if(!(classType != CLASS_U || datasetType == 2)){
                continue;
            }
            AUs_count numberAUs =
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[seqId].classIndex[class_it].number_AUs;
            for(AUs_count au_id = 0; au_id<numberAUs; au_id++){
                AU_indexing au_position =
                        datasetMasterIndexTable->sequence_indexes.sequence_indexes[seqId].classIndex[class_it].au_indexings[au_id];

                if(isByteOffsetUint64(getDatasetHeader(datasetMasterIndexTable->datasetContainer))){
                    writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 64, au_position.AU_offset);
                }else{
                    uint32_t au_offset = (uint32_t)au_position.AU_offset;
                    writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 32, au_offset);
                }

                if(!isPosOffsetUint40(datasetHeader)) {
                    writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 32, (uint32_t) au_position.au_start_position);
                    writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 32, (uint32_t) au_position.au_end_position);
                }else{
                    writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 40, au_position.au_start_position);
                    writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 40, au_position.au_end_position);
                }

                if(isMultipleAlignment(getDatasetHeader(datasetMasterIndexTable->datasetContainer))){
                    if(!isPosOffsetUint40(datasetHeader)) {
                        writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 32, (uint32_t) au_position.extended_au_start_position);
                        writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 32, (uint32_t) au_position.extended_au_end_position);
                    }else{
                        writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 40, au_position.extended_au_start_position);
                        writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 40, au_position.extended_au_end_position);
                    }
                }

                if(!isBlockHeaderFlagSet(datasetHeader)){
                    Descriptors_count numberDescriptors =
                            au_position.block_byte_offset.descriptors_number;
                    for (Descriptors_count descriptorId = 0; descriptorId < numberDescriptors; descriptorId++) {
                        if(isByteOffsetUint64(getDatasetHeader(datasetMasterIndexTable->datasetContainer))){
                            writeNBitsShiftAndConvertToBigEndian64(
                                outputBitstream,
                                64,
                                au_position.block_byte_offset.block_byte_offset[descriptorId]
                            );
                        }else{
                            uint32_t blockByteOffset
                                    = (uint32_t) au_position.block_byte_offset.block_byte_offset[descriptorId];
                            writeNBitsShiftAndConvertToBigEndian32(
                                    outputBitstream,
                                    32,
                                    blockByteOffset
                            );
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool writeUnalignedIndexing(DatasetMasterIndexTable* datasetMasterIndexTable, OutputBitstream* outputBitstream){
    DatasetHeader* datasetHeader = getDatasetHeader(datasetMasterIndexTable->datasetContainer);

    Classes_count unalignedClass_it;
    if(!getClassIndexForType(datasetHeader, CLASS_U, &unalignedClass_it)){
        return true;
    }

    if(!(isUnmappedIndexingFlagSet(datasetHeader) && getDatasetType(datasetHeader)!=2)){
        return true;
    }

    uint32_t multipleSignatureBase = getMultipleSignatureBase(getDatasetHeader(datasetMasterIndexTable->datasetContainer));
    uint64_t signatureSize = getUSignatureSize(getDatasetHeader(datasetMasterIndexTable->datasetContainer));
    uint32_t signatureLength = getSignatureLength(datasetHeader);
    //todo update this value
    uint8_t signatureBitsPerSymbol = 3;


    Descriptors_count numberDescriptorsUnmapped =
            getNumberDescriptorsInClass(getDatasetHeader(datasetMasterIndexTable->datasetContainer), unalignedClass_it);


    for(uint32_t uAccessUnit_i=0; uAccessUnit_i<datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits; uAccessUnit_i++) {
        if (multipleSignatureBase != 0) {
            size_t actualSignatureNumber = datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i]->allocated_signatures;
            if (actualSignatureNumber != multipleSignatureBase) {
                for (uint64_t signatureBit = 0; signatureBit < signatureSize; signatureBit++) {
                    writeBit(outputBitstream, 1);
                }
                writeNBitsShiftAndConvertToBigEndian16(
                        outputBitstream, 16, (uint16_t) actualSignatureNumber);
            }
            SignatureIntegerOutputStream signatureIntegerOutputStream;


            initSignatureOutputStream(
                    &signatureIntegerOutputStream,
                    outputBitstream,
                    signatureLength,
                    (uint8_t) signatureSize,
                    signatureBitsPerSymbol
            );
            for (uint16_t signature_i = 0; signature_i < actualSignatureNumber; signature_i++) {
                writeSignature(
                        &signatureIntegerOutputStream,
                        datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i]->signature + signature_i
                );
            }
            writeBufferSignature(&signatureIntegerOutputStream);

            writeBuffer(outputBitstream);

            if (isBlockHeaderFlagSet(datasetHeader)) {
                uint32_t uAccessUnitOffset =
                        (uint32_t) datasetMasterIndexTable->uAccessUnitsIndex.uAccessUnitOffsets[uAccessUnit_i];
                writeBigEndian32ToBitstream(
                        outputBitstream,
                        uAccessUnitOffset
                );
            } else {
                for (int descriptorUnalignedAccessUnit_i = 0;
                     descriptorUnalignedAccessUnit_i < numberDescriptorsUnmapped; descriptorUnalignedAccessUnit_i++) {
                    uint64_t block_offset =
                            datasetMasterIndexTable->uAccessUnitsIndex.byteOffset[descriptorUnalignedAccessUnit_i][uAccessUnit_i];
                    if(isByteOffsetUint64(datasetHeader)){
                        writeBigEndian64ToBitstream(outputBitstream, block_offset);
                    }else {
                        uint32_t casted_block_offset = (uint32_t) block_offset;
                        writeBigEndian32ToBitstream(outputBitstream, casted_block_offset);
                    }
                }
            }
            writeBuffer(outputBitstream);
        }
    }
    return true;
}

bool writeContentDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable, FILE *outputFile){
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);

    writeAlignedIndexing(datasetMasterIndexTable, &outputBitstream);
    writeBuffer(&outputBitstream);
    return writeUnalignedIndexing(datasetMasterIndexTable, &outputBitstream);
}

bool writeDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable, FILE *outputFile){
    uint64_t datasetMasterIndexTableSize = getSizeDatasetMasterIndexTable(datasetMasterIndexTable);
    if (!writeBoxHeader(outputFile, datasetMasterIndexTableName, datasetMasterIndexTableSize)){
        fprintf(stderr, "Error writing type and size of dataset master index table.");
        return false;
    }
    return writeContentDatasetMasterIndexTable(datasetMasterIndexTable, outputFile);
}

uint64_t getSizeContentUnalignedDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable){
    DatasetHeader* datasetHeader = getDatasetHeader(datasetMasterIndexTable->datasetContainer);
    uint64_t contentSize = 0;
    uint64_t contentSizeAsBits = 0;

    uint32_t multipleSignatureBase = getMultipleSignatureBase(getDatasetHeader(datasetMasterIndexTable->datasetContainer));
    uint64_t signatureSizeInBits = getUSignatureSize(getDatasetHeader(datasetMasterIndexTable->datasetContainer));
    int signatureLengthInSymbols = getSignatureLength(getDatasetHeader(datasetMasterIndexTable->datasetContainer));


    uint8_t class_it_for_u;
    bool classU_found = getClassIndexForType(datasetHeader, CLASS_U, &class_it_for_u);
    if (!classU_found){
        return contentSize;
    }

    uint16_t numberDescriptorsUnmapped =
            getNumberDescriptorsInClass(getDatasetHeader(datasetMasterIndexTable->datasetContainer), class_it_for_u);

    for(uint32_t uAccessUnit_i=0; uAccessUnit_i<datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits; uAccessUnit_i++) {
        uint64_t uAccessUnitsSignatureSizes = 0;
        if (multipleSignatureBase != 0) {
            size_t actualSignatureNumber = datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i]->allocated_signatures;
            if (actualSignatureNumber != multipleSignatureBase) {
                contentSize += bitsToBytes(signatureSizeInBits)+sizeof(uint16_t);

            }

            SignatureSizeComputation signatureSizeComputation;

            //todo find out bits per symbol based on alphabet
            uint8_t bitsPerSymbol = 3;
            initSignatureSizeComputation(
                    &signatureSizeComputation,
                    signatureLengthInSymbols,
                    (uint8_t) signatureSizeInBits,
                    bitsPerSymbol
            );
            for (uint16_t signature_i = 0; signature_i < actualSignatureNumber; signature_i++) {
                simulateSignature(
                        &signatureSizeComputation,
                        datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i]->signature + signature_i
                );
            }

            contentSize += bitsToBytes(getTotalSizeInBits(&signatureSizeComputation));

            if (isBlockHeaderFlagSet(getDatasetHeader(datasetMasterIndexTable->datasetContainer))) {
                contentSize += sizeof(uint32_t) * 2;
            } else {
                contentSize += numberDescriptorsUnmapped * sizeof(uint32_t);
            }

        }
    }
    return contentSize;
}

uint64_t getSizeContentDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable){
    uint64_t contentSize=0;
    DatasetHeader* datasetHeader = getDatasetHeader(datasetMasterIndexTable->datasetContainer);
    for(uint16_t seq_id=0; seq_id<datasetMasterIndexTable->sequence_indexes.number_sequences; seq_id++){
        uint16_t classesCount =
                getClassesCount(getDatasetHeader(datasetMasterIndexTable->datasetContainer));

        uint32_t numberAUs = getBlocksInSequence(getDatasetHeader(datasetMasterIndexTable->datasetContainer), seq_id);
        for(uint8_t class_i = 0; class_i<classesCount; class_i++){
            uint8_t classType = getClassType(datasetHeader, class_i);
            uint8_t datasetType = getDatasetType(datasetHeader);
            if (!(classType != CLASS_U || datasetType==2)){
                continue;
            }
            for(typeof(numberAUs) auId =0; auId<numberAUs; auId++){
                AU_indexing au_indexing =
                        datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex[class_i].au_indexings[auId];
                contentSize += 2*sizeof(uint32_t);
                if(isMultipleAlignment(getDatasetHeader(datasetMasterIndexTable->datasetContainer))){
                    contentSize += 2*sizeof(uint32_t);
                }
                if(isByteOffsetUint64(getDatasetHeader(datasetMasterIndexTable->datasetContainer))){
                    contentSize += sizeof(uint64_t);
                }else{
                    contentSize += sizeof(uint32_t);
                }
                if(!isBlockHeaderFlagSet(getDatasetHeader(datasetMasterIndexTable->datasetContainer))){
                    uint16_t numberDescriptors = au_indexing.block_byte_offset.descriptors_number;
                    if(isByteOffsetUint64(getDatasetHeader(datasetMasterIndexTable->datasetContainer))){
                        contentSize += numberDescriptors * sizeof(uint64_t);
                    }else{
                        contentSize += numberDescriptors * sizeof(uint32_t);
                    }
                }
            }
        }
    }

    contentSize += getSizeContentUnalignedDatasetMasterIndexTable(datasetMasterIndexTable);
    return contentSize;
}

uint64_t getSizeDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable){
    return BOX_HEADER_SIZE + getSizeContentDatasetMasterIndexTable(datasetMasterIndexTable);
}

uint64_t getAUStartPosition(
    DatasetMasterIndexTable* datasetMasterIndexTable,
    uint16_t sequenceId,
    uint16_t classId,
    uint32_t AU_id
){
    return datasetMasterIndexTable->sequence_indexes
                   .sequence_indexes[sequenceId]
                   .classIndex[classId]
                   .au_indexings[AU_id]
                   .au_start_position;
}

uint64_t getAUEndPosition(
        DatasetMasterIndexTable* datasetMasterIndexTable,
        uint16_t sequenceId,
        uint16_t classId,
        uint32_t AU_id
){
    return datasetMasterIndexTable->sequence_indexes
            .sequence_indexes[sequenceId]
            .classIndex[classId]
            .au_indexings[AU_id]
            .au_end_position;
}

uint64_t getAUExtendedStartPosition(
        DatasetMasterIndexTable* datasetMasterIndexTable,
        uint16_t sequenceId,
        uint16_t classId,
        uint32_t AU_id
){
    return datasetMasterIndexTable->sequence_indexes
            .sequence_indexes[sequenceId]
            .classIndex[classId]
            .au_indexings[AU_id]
            .extended_au_start_position;
}

uint64_t getAUExtendedEndPosition(
        DatasetMasterIndexTable* datasetMasterIndexTable,
        uint16_t sequenceId,
        uint16_t classId,
        uint32_t AU_id
){
    return datasetMasterIndexTable->sequence_indexes
            .sequence_indexes[sequenceId]
            .classIndex[classId]
            .au_indexings[AU_id]
            .extended_au_end_position;
}

void setStartEndAndOffset(DatasetMasterIndexTable *datasetMasterIndexTable, uint16_t sequenceId, uint16_t classId,
                          uint32_t AU_id, uint32_t start, uint32_t end, uint64_t offset) {
    if(sequenceId<datasetMasterIndexTable->sequence_indexes.number_sequences){
        if(classId<datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].number_of_classes){
            if(AU_id<datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].classIndex[classId].number_AUs){
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].classIndex[classId]
                        .au_indexings[AU_id].au_start_position= start;
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].classIndex[classId]
                        .au_indexings[AU_id].au_end_position = end;
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].classIndex[classId]
                        .au_indexings[AU_id].AU_offset = offset;
            }
        }
    }
}

void setExtendedStartAndEnd(DatasetMasterIndexTable* datasetMasterIndexTable,
                            uint16_t sequenceId, uint16_t classId, uint32_t AU_id, uint32_t extended_start,
                            uint32_t extended_end){
    if(sequenceId<datasetMasterIndexTable->sequence_indexes.number_sequences){
        if(classId<datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].number_of_classes){
            if(AU_id<datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].classIndex[classId].number_AUs){
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].classIndex[classId].au_indexings[AU_id].
                        extended_au_start_position= extended_start;
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].classIndex[classId].au_indexings[AU_id].
                        extended_au_end_position= extended_end;
            }
        }
    }
}

void setOffset(DatasetMasterIndexTable *datasetMasterIndexTable,
               uint16_t sequenceId, uint16_t classId, uint32_t AU_id, uint16_t descriptorId, uint64_t offset){
    if(sequenceId<datasetMasterIndexTable->sequence_indexes.number_sequences){
        if(classId<datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].number_of_classes){
            if(AU_id<datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].classIndex[classId].number_AUs){
                if(descriptorId< datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId]
                                         .classIndex[classId].au_indexings[AU_id].block_byte_offset.descriptors_number
                ) {
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].classIndex[classId].
                        au_indexings[AU_id].block_byte_offset.block_byte_offset[descriptorId] = offset;
                    if(!isBlockHeaderFlagSet(getDatasetHeader(datasetMasterIndexTable->datasetContainer))) {
                        TreeNode *treeNode = initTreeNode(offset);
                        if (insert(
                                datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[classId].offsetsPerStream[descriptorId],
                                treeNode
                        )){
                            datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[classId].sizeTrees[descriptorId]++;
                        }else{
                            free(treeNode);
                        }
                    }
                }
            }
        }
    }
}

void insertFinalOffset(
    DatasetMasterIndexTable *datasetMasterIndexTable,
    uint16_t classId,
    uint16_t descriptorId,
    uint64_t offset
){
    TreeNode *treeNode = initTreeNode(offset);
    if (insert(
            datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[classId].offsetsPerStream[descriptorId],
            treeNode
    )){
        datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[classId].sizeTrees[descriptorId]++;
    }else{
        free(treeNode);
    }
}

void setUnalignedOffset(DatasetMasterIndexTable* datasetMasterIndexTable, uint32_t uAU_id, uint32_t uDescriptorId,
                        uint64_t offset){
    uint8_t unalignedClassId = 5;
    uint16_t numberDescriptorsUnmapped =
            getNumberDescriptorsInClass(getDatasetHeader(datasetMasterIndexTable->datasetContainer), unalignedClassId);
    if (datasetMasterIndexTable->uAccessUnitsIndex.byteOffset!= NULL){
        if(uAU_id<datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits && uDescriptorId<numberDescriptorsUnmapped){
            datasetMasterIndexTable->uAccessUnitsIndex.byteOffset[uDescriptorId][uAU_id]=offset;
        }
    }
}

Signatures * getSignatures(DatasetMasterIndexTable* datasetMasterIndexTable, int uAccessUnit_i){
    return datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i];
}

void parseDatasetMasterIndexTableAlignedAUs(DatasetMasterIndexTable* datasetMasterIndexTable, FILE* inputFile){
    DatasetContainer* datasetContainer = datasetMasterIndexTable->datasetContainer;
    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    for(uint16_t seqId = 0; seqId<datasetMasterIndexTable->sequence_indexes.number_sequences; seqId++){
        uint16_t numberClassesForAligned =
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[seqId].number_of_classes;
        for(uint16_t classId = 0; classId<numberClassesForAligned; classId++){
            uint32_t numberAUs =
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[seqId].classIndex[classId].number_AUs;
            for(uint32_t au_id = 0; au_id<numberAUs; au_id++){
                AU_indexing* au_position =
                        &(datasetMasterIndexTable->sequence_indexes.sequence_indexes[seqId].classIndex[classId].au_indexings[au_id]);

                if(isByteOffsetUint64(getDatasetHeader(datasetContainer))){
                    readNBitsShiftAndConvertBigToNativeEndian64(
                            &inputBitstream,
                            64,
                            (char*)&(au_position->AU_offset)
                    );
                }else{
                    uint32_t au_offset;
                    readNBitsShiftAndConvertBigToNativeEndian32(
                            &inputBitstream,
                            32,
                            (char*)&au_offset
                    );
                    au_position->AU_offset = au_offset;
                }

                if(isPosOffsetUint40(getDatasetHeader(datasetContainer))) {
                    readNBitsShiftAndConvertBigToNativeEndian64(
                            &inputBitstream,
                            40,
                            (char *)&(au_position->au_start_position)
                    );
                    readNBitsShiftAndConvertBigToNativeEndian64(
                            &inputBitstream,
                            40,
                            (char *)&(au_position->au_end_position)
                    );
                }else{
                    uint32_t  au_start_position;
                    uint32_t  au_end_position;
                    readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char*)&au_start_position);
                    au_position->au_start_position = au_start_position;
                    readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char*)&au_end_position);
                    au_position->au_end_position = au_end_position;
                }


                if(isMultipleAlignment(getDatasetHeader(datasetContainer))){
                    if(isPosOffsetUint40(getDatasetHeader(datasetContainer))){
                        readNBitsShiftAndConvertBigToNativeEndian64(&inputBitstream, 40,
                                                                    (char *) &(au_position->extended_au_start_position));
                        readNBitsShiftAndConvertBigToNativeEndian64(&inputBitstream, 40,
                                                                    (char *) &(au_position->extended_au_end_position));
                    }else {
                        uint32_t extended_au_start_position;
                        uint32_t extended_au_end_position;
                        readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32,
                                                                    (char *) &extended_au_start_position);
                        au_position->extended_au_start_position = extended_au_start_position;
                        readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32,
                                                                    (char *) &extended_au_end_position);
                        au_position->extended_au_end_position = extended_au_end_position;
                    }

                }

                datasetContainer->accessUnitsOffsets[seqId][classId][au_id]=au_position->AU_offset;
                if(!isBlockHeaderFlagSet(getDatasetHeader(datasetContainer))){
                    uint16_t numberDescriptors = au_position->block_byte_offset.descriptors_number;
                    for(uint16_t descriptorId = 0; descriptorId<numberDescriptors; descriptorId++){
                        uint64_t block_offset;
                        if(isByteOffsetUint64(getDatasetHeader(datasetContainer))){
                            readNBitsShiftAndConvertBigToNativeEndian64(
                                    &inputBitstream,
                                    64,
                                    (char*)&block_offset
                            );
                        }else{
                            uint32_t block_offset32;
                            readNBitsShiftAndConvertBigToNativeEndian32(
                                    &inputBitstream,
                                    32,
                                    (char*)&block_offset32
                            );
                            block_offset = block_offset32;
                        }
                        setOffset(datasetMasterIndexTable, seqId, classId, au_id, descriptorId, block_offset);
                    }
                }
            }
        }
    }
    forwardUntilAligned(&inputBitstream);
}

void parseDatasetMasterIndexTableUnalignedAUs(DatasetMasterIndexTable* datasetMasterIndexTable, FILE* inputFile){
    DatasetContainer* datasetContainer=datasetMasterIndexTable->datasetContainer;
    DatasetHeader* datasetHeader = getDatasetHeader(datasetContainer);
    uint32_t multipleSignatureBase = getMultipleSignatureBase(datasetHeader);
    uint32_t signatureLengthInBits = getUSignatureSize(datasetHeader);
    uint64_t bytesNeeded =  bitsToBytes(signatureLengthInBits);

    uint8_t* signatureBuffer = (uint8_t*) malloc(bytesNeeded*sizeof(uint8_t));

    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);
    uint8_t unalignedClassId = CLASS_U;
    uint8_t unalignedClass_i;
    bool foundUnalignedClassId = getClassIndexForType(datasetHeader, unalignedClassId, &unalignedClass_i);
    if (!foundUnalignedClassId){
        free(signatureBuffer);
        return;
    }
    uint16_t numberDescriptorsUnmapped =
            getNumberDescriptorsInClass(datasetHeader, unalignedClass_i);
    for(uint32_t uAccessUnit_i=0; uAccessUnit_i<datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits; uAccessUnit_i++){
        uint64_t blockUAccessUnitOffset;
        if(isByteOffsetUint64(getDatasetHeader(datasetContainer))){
            readNBitsShiftAndConvertBigToNativeEndian64(&inputBitstream, 64, (char*)&blockUAccessUnitOffset);
        }else {
            uint32_t blockUAccessUnitOffset32;
            readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char*)&blockUAccessUnitOffset32);
            blockUAccessUnitOffset = blockUAccessUnitOffset32;
        }
        datasetMasterIndexTable->uAccessUnitsIndex.uAccessUnitOffsets[uAccessUnit_i] = blockUAccessUnitOffset;

        if (multipleSignatureBase!=0){
            uint16_t actualSignatureNumber = (uint16_t) multipleSignatureBase;
            readNBitsShift(&inputBitstream,signatureLengthInBits,(char*)signatureBuffer);
            bool allBitsToTrue = firstNBitsSetTo((char*)signatureBuffer, signatureLengthInBits, true);


            SignatureArbitraryLengthIntegerInputStream signatureIntegerInputStream;
            if(allBitsToTrue) {
                readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 16, (char *) &actualSignatureNumber);
                changeSizeSignatures(
                        datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i],
                        actualSignatureNumber
                );

                initSignatureInputStream(&signatureIntegerInputStream,
                                         &inputBitstream, (uint8_t) signatureLengthInBits, 3, 0);
            }else {
                initSignatureInputStreamAndFirstInteger(&signatureIntegerInputStream,
                                                        &inputBitstream, (uint8_t) signatureLengthInBits, 3, 0, signatureBuffer);
            }
            for(uint16_t signature_i=0; signature_i<actualSignatureNumber; signature_i++) {
                Signature signature = getSignatureFromInputStream(&signatureIntegerInputStream);
                *getSignature(
                        datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i],
                        signature_i
                ) = signature;
                if(signature_i != actualSignatureNumber-1){
                    getNewInteger(&signatureIntegerInputStream);
                }
            }
            freeSignatureInputStream(&signatureIntegerInputStream);
        }

        forwardUntilAligned(&inputBitstream);
        if (!isBlockHeaderFlagSet(datasetHeader)){
            for (uint32_t descriptorUnalignedAccessUnit_i = 0;
                 descriptorUnalignedAccessUnit_i < numberDescriptorsUnmapped; descriptorUnalignedAccessUnit_i++) {

                uint64_t offset_buffer;
                if(isByteOffsetUint64(getDatasetHeader(datasetContainer))){
                    readNBitsShiftAndConvertBigToNativeEndian64(&inputBitstream, 64, (char*)&offset_buffer);
                }else{
                    uint32_t offset_buffer32;
                    readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char*)&offset_buffer32);
                    offset_buffer = offset_buffer32;
                }
                setUnalignedOffset(datasetMasterIndexTable, uAccessUnit_i, descriptorUnalignedAccessUnit_i,
                                   offset_buffer);

            }
        }
    }
    free(signatureBuffer);
}

DatasetMasterIndexTable *parseDatasetMasterIndexTable(DatasetContainer *datasetContainer, FILE *inputFile) {
    DatasetMasterIndexTable* datasetMasterIndexTable = initDatasetMasterIndexTable(datasetContainer);
    datasetMasterIndexTable->seekPosition = ftell(inputFile);

    parseDatasetMasterIndexTableAlignedAUs(datasetMasterIndexTable, inputFile);
    parseDatasetMasterIndexTableUnalignedAUs(datasetMasterIndexTable, inputFile);
    return datasetMasterIndexTable;
}

long getDatasetMasterIndexTableSeekPosition(DatasetMasterIndexTable* datasetMasterIndexTable){
    return datasetMasterIndexTable->seekPosition;
}

bool getAccessUnitByteOffsetFromSequenceClass(AUs_index* aus_index, uint32_t auId, uint64_t* byteOffset) {
    if(auId < aus_index->number_AUs){
        *byteOffset = aus_index->au_indexings[auId].AU_offset;
        return true;
    }else{
        return false;
    }
}

bool getAccessUnitByteOffsetFromSequence(
        Sequence_index *sequence_index,
        uint16_t classId,
        uint32_t auId,
        uint64_t *byteOffset
) {
    if(classId < sequence_index->number_of_classes){
        return getAccessUnitByteOffsetFromSequenceClass(sequence_index->classIndex+classId, auId, byteOffset);
    }else{
        return false;
    }
}

bool getAccessUnitByteOffset(DatasetMasterIndexTable *datasetMasterIndexTable, uint16_t sequenceId, uint16_t classId,
                             uint32_t auId, uint64_t *byteOffset) {
    if(sequenceId < datasetMasterIndexTable->sequence_indexes.number_sequences){
        return getAccessUnitByteOffsetFromSequence(
                datasetMasterIndexTable->sequence_indexes.sequence_indexes + sequenceId,
                classId,
                auId,
                byteOffset
        );
    }else{
        return false;
    }
}


bool getBlockByteOffsetFromSequenceClassAu(AU_indexing *au_indexing, uint32_t descriptorId, uint64_t *byteOffset) {
    if(descriptorId < au_indexing->block_byte_offset.descriptors_number){
        *byteOffset = au_indexing->block_byte_offset.block_byte_offset[descriptorId];
        return true;
    }else{
        return false;
    }
}

bool getBlockByteOffsetFromSequenceClass(AUs_index* aus_index, uint32_t auId, uint32_t descriptorId, uint64_t* byteOffset) {
    if(auId < aus_index->number_AUs){
        return getBlockByteOffsetFromSequenceClassAu(aus_index->au_indexings + auId, descriptorId, byteOffset);
    }else{
        return false;
    }
}

bool getBlockByteOffsetFromSequence(Sequence_index *sequenceIndex, uint16_t classId, uint32_t auId, uint32_t descriptorId,
                               uint64_t *byteOffset) {
    if(classId < sequenceIndex->number_of_classes){
        return getBlockByteOffsetFromSequenceClass(
                sequenceIndex->classIndex + classId,
                auId,
                descriptorId,
                byteOffset
        );
    }else{
        return false;
    }
}

bool getBlockByteOffset(DatasetMasterIndexTable *datasetMasterIndexTable, uint16_t sequenceId, uint16_t classId,
                        uint32_t auId, uint32_t descriptorId, uint64_t *byteOffset){
    if(sequenceId < datasetMasterIndexTable->sequence_indexes.number_sequences){
        return getBlockByteOffsetFromSequence(
                datasetMasterIndexTable->sequence_indexes.sequence_indexes + sequenceId,
                classId,
                auId,
                descriptorId,
                byteOffset
        );
    }else{
        return false;
    }
}

bool getNextBlockByteOffset(DatasetMasterIndexTable *datasetMasterIndexTable, uint16_t sequenceId, uint16_t classId,
                        uint32_t auId, uint32_t descriptorId, uint64_t *nextByteOffset){
    uint64_t blockStart;
    if(!getBlockByteOffset(datasetMasterIndexTable, sequenceId, classId, auId, descriptorId, &blockStart)){
        return NULL;
    }
    Tree* byteOffsetsTree = NULL;
    if(classId < datasetMasterIndexTable->offsetsPerClass->numberClasses){
        if(descriptorId < datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[classId].numberStreams){
            byteOffsetsTree =
                    datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[classId].offsetsPerStream[descriptorId];
        }
    }
    if(byteOffsetsTree == NULL){
        return NULL;
    }
    TreeNode* currentBlockOffsetTreeNode = findNode(byteOffsetsTree, blockStart);
    if(currentBlockOffsetTreeNode == NULL){
        return NULL;
    }
    TreeNode* nextBlockOffsetTreeNode = getNext(currentBlockOffsetTreeNode);
    if(nextBlockOffsetTreeNode == NULL){
        return NULL;
    }
    *nextByteOffset = nextBlockOffsetTreeNode->value;
    return true;
}