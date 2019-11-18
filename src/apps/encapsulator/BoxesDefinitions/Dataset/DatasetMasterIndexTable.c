//
// Created by gencom on 25/07/17.
//

#include <stdlib.h>
#include <string.h>
#include "../../Boxes.h"
#include "../../DataStructures/BitStreams/InputBitstream.h"
#include "../../DataStructures/BitStreams/OutputBitstream.h"
#include "../../DataStructures/Signatures/SignatureSizeComputation.h"
#include "../../DataStructures/Signatures/SignaturesOutputStream.h"
#include "../../treeUint64.h"
#include "../../utils.h"
void initIndexUnaligned(DatasetMasterIndexTable* datasetMasterIndexTable);
bool writeAlignedIndexing(DatasetMasterIndexTable* datasetMasterIndexTable, OutputBitstream* outputBitstream);
bool writeUnalignedIndexing(DatasetMasterIndexTable* datasetMasterIndexTable, OutputBitstream* outputBitstream);
uint64_t getSizeContentUnalignedDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable);
void parseDatasetMasterIndexTableAlignedAUs(DatasetMasterIndexTable* datasetMasterIndexTable, FILE* inputFile);
void parseDatasetMasterIndexTableUnalignedAUs(DatasetMasterIndexTable* datasetMasterIndexTable, FILE* inputFile);
bool getAccessUnitByteOffsetFromSequenceClass(AUs_index* aus_index, uint32_t auId, uint64_t* byteOffset);
bool getAccessUnitByteOffsetFromSequence(Sequence_indexation* sequence_index, uint16_t classId, uint32_t auId,
                                         uint64_t* byteOffset);
bool getBlockByteOffsetFromSequenceClassAu(AU_indexing* au_indexing, uint32_t descriptorId, uint64_t* byteOffset);
bool getBlockByteOffsetFromSequenceClass(AUs_index* aus_index, uint32_t auId, uint32_t descriptorId,
                                         uint64_t* byteOffset);
bool getBlockByteOffsetFromSequence(Sequence_indexation* sequenceIndex, ClassTypeEnum classId, uint32_t auId,
                                    uint32_t descriptorId, uint64_t* byteOffset);

void initIndexUnaligned(DatasetMasterIndexTable* datasetMasterIndexTable) {
    DatasetContainer* datasetContainer = datasetMasterIndexTable->datasetContainer;

    uint32_t numberUAccessUnits = getNumberUAccessUnits(getDatasetHeader(datasetContainer));
    if (numberUAccessUnits == 0) {
        return;
    }
    datasetMasterIndexTable->uAccessUnitsIndex.signatures =
        (Signatures**)malloc(numberUAccessUnits * sizeof(Signatures*));
    datasetMasterIndexTable->uAccessUnitsIndex.AU_byte_offset = (uint64_t*)calloc(numberUAccessUnits, sizeof(uint64_t));
    datasetMasterIndexTable->uAccessUnitsIndex.ref_sequence_id =
        (uint16_t*)calloc(numberUAccessUnits, sizeof(uint16_t));
    datasetMasterIndexTable->uAccessUnitsIndex.ref_start_position =
        (uint64_t*)calloc(numberUAccessUnits, sizeof(uint64_t));
    datasetMasterIndexTable->uAccessUnitsIndex.ref_end_position =
        (uint64_t*)calloc(numberUAccessUnits, sizeof(uint64_t));
    datasetMasterIndexTable->uAccessUnitsIndex.uAccessUnitSizes =
        (uint32_t*)calloc(numberUAccessUnits, sizeof(uint32_t));

    uint8_t unalignedClassId = 5;
    uint16_t numberDescriptorsUnmapped =
        getNumberDescriptorsInClass(getDatasetHeader(datasetContainer), unalignedClassId);
    datasetMasterIndexTable->uAccessUnitsIndex.byteOffset =
        (uint64_t**)malloc(numberDescriptorsUnmapped * sizeof(uint64_t*));
    bool allCorrect = true;
    if (datasetMasterIndexTable->uAccessUnitsIndex.byteOffset != NULL) {
        for (int descriptorUnalignedAccessUnit_i = 0; descriptorUnalignedAccessUnit_i < numberDescriptorsUnmapped;
             descriptorUnalignedAccessUnit_i++) {
            datasetMasterIndexTable->uAccessUnitsIndex.byteOffset[descriptorUnalignedAccessUnit_i] =
                (uint64_t*)malloc(getNumberUAccessUnits(getDatasetHeader(datasetContainer)) * sizeof(uint64_t));
            if (datasetMasterIndexTable->uAccessUnitsIndex.byteOffset[descriptorUnalignedAccessUnit_i] == NULL) {
                allCorrect = false;
                break;
            }
        }
        if (!allCorrect) {
            for (int descriptorUnalignedAccessUnit_i = 0; descriptorUnalignedAccessUnit_i < numberDescriptorsUnmapped;
                 descriptorUnalignedAccessUnit_i++) {
                free(datasetMasterIndexTable->uAccessUnitsIndex.byteOffset[descriptorUnalignedAccessUnit_i]);
            }
            free(datasetMasterIndexTable->uAccessUnitsIndex.byteOffset);
            datasetMasterIndexTable->uAccessUnitsIndex.byteOffset = NULL;
        }

        datasetMasterIndexTable->uAccessUnitsIndex.offsetsDescriptorUnmappedTree =
            (Tree**)calloc(numberDescriptorsUnmapped, sizeof(Tree*));
        for (uint16_t stream_i = 0; stream_i < numberDescriptorsUnmapped; stream_i++) {
            datasetMasterIndexTable->uAccessUnitsIndex.offsetsDescriptorUnmappedTree[stream_i] = initTree();
        }
    }

    if (datasetMasterIndexTable->uAccessUnitsIndex.signatures != NULL) {
        datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits =
            getNumberUAccessUnits(getDatasetHeader(datasetContainer));
    } else {
        datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits = 0;
    }

    for (uint32_t uAccessUnit_i = 0; uAccessUnit_i < datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits;
         uAccessUnit_i++) {
        datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i] =
            initSignatures(getMultipleSignatureBase(getDatasetHeader(datasetContainer)));
    }
}

DatasetMasterIndexTable* initDatasetMasterIndexTable(DatasetContainer* datasetContainer) {
    DatasetMasterIndexTable* datasetMasterIndexTable =
        (DatasetMasterIndexTable*)malloc(sizeof(DatasetMasterIndexTable));
    datasetMasterIndexTable->hasSeek = false;
    datasetMasterIndexTable->seekPosition = 0;
    datasetMasterIndexTable->datasetContainer = datasetContainer;

    datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits = 0;
    datasetMasterIndexTable->uAccessUnitsIndex.signatures = NULL;
    datasetMasterIndexTable->uAccessUnitsIndex.AU_byte_offset = NULL;
    datasetMasterIndexTable->uAccessUnitsIndex.uAccessUnitSizes = NULL;
    datasetMasterIndexTable->uAccessUnitsIndex.byteOffset = NULL;

    uint16_t sequences_count = getSequencesCount(getDatasetHeader(datasetContainer));
    datasetMasterIndexTable->sequence_indexes.sequence_indexes =
        (Sequence_indexation*)malloc(sequences_count * sizeof(Sequence_indexation));
    if (datasetMasterIndexTable->sequence_indexes.sequence_indexes == NULL) {
        datasetMasterIndexTable->sequence_indexes.number_sequences = 0;
        return NULL;
    }
    datasetMasterIndexTable->sequence_indexes.number_sequences = sequences_count;

    for (uint16_t seq_i = 0; seq_i < sequences_count; seq_i++) {
        uint16_t classesCountForAligned = getClassesCount(getDatasetHeader(datasetContainer));
        ClassType uType;
        uType.classType = CLASS_TYPE_CLASS_U;
        if (hasClassType(getDatasetHeader(datasetContainer), uType)) {
            classesCountForAligned--;
        }
        datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_i].number_of_classes = classesCountForAligned;
        datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_i].classIndex =
            (AUs_index*)malloc(classesCountForAligned * sizeof(AUs_index));

        uint32_t numberAUs = getBlocksInSequence(getDatasetHeader(datasetContainer), seq_i);
        for (uint8_t classId = 0; classId < classesCountForAligned; classId++) {
            datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_i].classIndex[classId].au_indexings =
                (AU_indexing*)calloc(numberAUs, sizeof(AU_indexing));
            datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_i].classIndex[classId].number_AUs =
                numberAUs;

            if (!isBlockHeaderFlagSet(getDatasetHeader(datasetContainer))) {
                uint16_t numberDescriptors = getNumberDescriptorsInClass(getDatasetHeader(datasetContainer), classId);
                for (typeof(numberAUs) auId = 0; auId < numberAUs; auId++) {
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_i]
                        .classIndex[classId]
                        .au_indexings[auId]
                        .block_byte_offset.descriptors_number = numberDescriptors;
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_i]
                        .classIndex[classId]
                        .au_indexings[auId]
                        .block_byte_offset.block_byte_offset = (uint64_t*)malloc(numberDescriptors * sizeof(uint64_t));
                }
            } else {
                for (typeof(numberAUs) auId = 0; auId < numberAUs; auId++) {
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_i]
                        .classIndex[classId]
                        .au_indexings[auId]
                        .block_byte_offset.descriptors_number = 0;
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_i]
                        .classIndex[classId]
                        .au_indexings[auId]
                        .block_byte_offset.block_byte_offset = NULL;
                }
            }
        }
    }
    if (!isBlockHeaderFlagSet(getDatasetHeader(datasetContainer))) {
        datasetMasterIndexTable->offsetsPerClass = (OffsetsPerClass*)malloc(sizeof(OffsetsPerClass));
        uint16_t classesCountForAligned = getClassesCount(getDatasetHeader(datasetContainer));
        ClassType uType;
        uType.classType = CLASS_TYPE_CLASS_U;
        if (hasClassType(getDatasetHeader(datasetContainer), uType)) {
            classesCountForAligned--;
        }
        datasetMasterIndexTable->offsetsPerClass->offsetsPerClass =
            malloc(classesCountForAligned * sizeof(OffsetsPerStream));
        datasetMasterIndexTable->offsetsPerClass->numberClasses = classesCountForAligned;
        for (uint8_t class_i = 0; class_i < classesCountForAligned; class_i++) {
            uint16_t numberDescriptors = getNumberDescriptorsInClass(getDatasetHeader(datasetContainer), class_i);
            datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].offsetsPerStream =
                (Tree**)calloc(numberDescriptors, sizeof(Tree*));
            datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].sizeTrees =
                (uint64_t*)calloc(numberDescriptors, sizeof(uint64_t));
            datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].numberStreams = numberDescriptors;
            for (uint16_t stream_i = 0; stream_i < numberDescriptors; stream_i++) {
                datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].offsetsPerStream[stream_i] =
                    initTree();
            }
        }
    } else {
        datasetMasterIndexTable->offsetsPerClass = NULL;
    }

    initIndexUnaligned(datasetMasterIndexTable);
    return datasetMasterIndexTable;
}

void freeDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable) {
    DatasetContainer* datasetContainer = datasetMasterIndexTable->datasetContainer;

    if (datasetMasterIndexTable->offsetsPerClass != NULL) {
        uint16_t classesCountForAligned = getClassesCount(getDatasetHeader(datasetContainer));
        ClassType uType;
        uType.classType = CLASS_TYPE_CLASS_U;
        if (hasClassType(getDatasetHeader(datasetContainer), uType)) {
            classesCountForAligned--;
        }

        datasetMasterIndexTable->offsetsPerClass->numberClasses = classesCountForAligned;
        for (uint8_t class_i = 0; class_i < classesCountForAligned; class_i++) {
            uint16_t numberDescriptors = getNumberDescriptorsInClass(getDatasetHeader(datasetContainer), class_i);

            for (uint16_t descriptor_i = 0; descriptor_i < numberDescriptors; descriptor_i++) {
                if (numberElements(datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i]
                                       .offsetsPerStream[descriptor_i]) !=
                    datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].sizeTrees[descriptor_i]) {
                    fprintf(stderr, "lost elements.\n");
                }
                freeTree(
                    datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].offsetsPerStream[descriptor_i]);
            }
            free(datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].offsetsPerStream);
            free(datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i].sizeTrees);
        }
        free(datasetMasterIndexTable->offsetsPerClass->offsetsPerClass);
        free(datasetMasterIndexTable->offsetsPerClass);
    }

    for (uint16_t seq_id = 0; seq_id < datasetMasterIndexTable->sequence_indexes.number_sequences; seq_id++) {
        uint16_t classesCountForAligned = getClassesCount(getDatasetHeader(datasetMasterIndexTable->datasetContainer));
        ClassType uType;
        uType.classType = CLASS_TYPE_CLASS_U;

        if (hasClassType(getDatasetHeader(datasetMasterIndexTable->datasetContainer), uType)) {
            classesCountForAligned--;
        }

        uint32_t numberAUs = getBlocksInSequence(getDatasetHeader(datasetMasterIndexTable->datasetContainer), seq_id);
        for (uint8_t classId = 0; classId < classesCountForAligned; classId++) {
            for (typeof(numberAUs) auId = 0; auId < numberAUs; auId++) {
                free(datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id]
                         .classIndex[classId]
                         .au_indexings[auId]
                         .block_byte_offset.block_byte_offset);
            }
            free(datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex[classId].au_indexings);
        }
        free(datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id].classIndex);
    }
    free(datasetMasterIndexTable->sequence_indexes.sequence_indexes);

    for (uint32_t uAccessUnit_i = 0; uAccessUnit_i < datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits;
         uAccessUnit_i++) {
        freeSignatures(datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i]);
    }
    free(datasetMasterIndexTable->uAccessUnitsIndex.signatures);

    ClassType unalignedClassId;
    unalignedClassId.classType = CLASS_TYPE_CLASS_U;

    uint8_t unalignedClass_index;
    if (getClassIndexForType(getDatasetHeader(datasetMasterIndexTable->datasetContainer), unalignedClassId,
                             &unalignedClass_index)) {
        uint16_t numberDescriptorsUnmapped = getNumberDescriptorsInClass(
            getDatasetHeader(datasetMasterIndexTable->datasetContainer), unalignedClass_index);
        for (int descriptorUnalignedAccessUnit_i = 0; descriptorUnalignedAccessUnit_i < numberDescriptorsUnmapped;
             descriptorUnalignedAccessUnit_i++) {
            free(datasetMasterIndexTable->uAccessUnitsIndex.byteOffset[descriptorUnalignedAccessUnit_i]);
        }
        free(datasetMasterIndexTable->uAccessUnitsIndex.byteOffset);
        free(datasetMasterIndexTable->uAccessUnitsIndex.ref_sequence_id);
        free(datasetMasterIndexTable->uAccessUnitsIndex.ref_start_position);
        free(datasetMasterIndexTable->uAccessUnitsIndex.ref_end_position);
        free(datasetMasterIndexTable->uAccessUnitsIndex.AU_byte_offset);
        free(datasetMasterIndexTable->uAccessUnitsIndex.uAccessUnitSizes);
    }
    free(datasetMasterIndexTable);
}

bool writeAlignedIndexing(DatasetMasterIndexTable* datasetMasterIndexTable, OutputBitstream* outputBitstream) {
    DatasetHeader* datasetHeader = getDatasetHeader(datasetMasterIndexTable->datasetContainer);
    for (typeof(datasetMasterIndexTable->sequence_indexes.number_sequences) seqId = 0;
         seqId < datasetMasterIndexTable->sequence_indexes.number_sequences; seqId++) {
        Classes_count classNum = getClassesCount(datasetHeader);
        DatasetType datasetType = getDatasetType(datasetHeader);

        for (Classes_count class_it = 0; class_it < classNum; class_it++) {
            ClassType classType = getClassType(datasetHeader, class_it);
            if (!(classType.classType != CLASS_TYPE_CLASS_U || datasetType == 2)) {
                continue;
            }
            AUs_count numberAUs =
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[seqId].classIndex[class_it].number_AUs;
            for (AUs_count au_id = 0; au_id < numberAUs; au_id++) {
                AU_indexing au_position = datasetMasterIndexTable->sequence_indexes.sequence_indexes[seqId]
                                              .classIndex[class_it]
                                              .au_indexings[au_id];

                if (isByteOffsetUint64(getDatasetHeader(datasetMasterIndexTable->datasetContainer))) {
                    writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 64, au_position.AU_byte_offset);
                } else {
                    uint32_t au_offset = (uint32_t)au_position.AU_byte_offset;
                    writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 32, au_offset);
                }

                if (!isPosOffsetUint40(datasetHeader)) {
                    writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 32,
                                                           (uint32_t)au_position.au_start_position);
                    writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 32, (uint32_t)au_position.au_end_position);
                } else {
                    writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 40, au_position.au_start_position);
                    writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 40, au_position.au_end_position);
                }

                if (datasetType == 2) {
                    writeBigEndian16ToBitstream(outputBitstream, au_position.ref_sequence_id);
                    if (!isPosOffsetUint40(datasetHeader)) {
                        writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 32, au_position.ref_start_position);
                        writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 32, au_position.ref_end_position);
                    } else {
                        writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 40, au_position.ref_start_position);
                        writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 40, au_position.ref_end_position);
                    }
                }

                if (isMultipleAlignment(getDatasetHeader(datasetMasterIndexTable->datasetContainer))) {
                    if (!isPosOffsetUint40(datasetHeader)) {
                        writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 32,
                                                               (uint32_t)au_position.extended_au_start_position);
                        writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 32,
                                                               (uint32_t)au_position.extended_au_end_position);
                    } else {
                        writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 40,
                                                               au_position.extended_au_start_position);
                        writeNBitsShiftAndConvertToBigEndian64(outputBitstream, 40,
                                                               au_position.extended_au_end_position);
                    }
                }

                if (!isBlockHeaderFlagSet(datasetHeader)) {
                    Descriptors_count numberDescriptors = au_position.block_byte_offset.descriptors_number;
                    for (Descriptors_count descriptorId = 0; descriptorId < numberDescriptors; descriptorId++) {
                        if (isByteOffsetUint64(getDatasetHeader(datasetMasterIndexTable->datasetContainer))) {
                            writeNBitsShiftAndConvertToBigEndian64(
                                outputBitstream, 64, au_position.block_byte_offset.block_byte_offset[descriptorId]);
                        } else {
                            uint32_t blockByteOffset =
                                (uint32_t)au_position.block_byte_offset.block_byte_offset[descriptorId];
                            writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 32, blockByteOffset);
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool writeUnalignedIndexing(DatasetMasterIndexTable* datasetMasterIndexTable, OutputBitstream* outputBitstream) {
    DatasetHeader* datasetHeader = getDatasetHeader(datasetMasterIndexTable->datasetContainer);

    ClassType uType;
    uType.classType = CLASS_TYPE_CLASS_U;

    Classes_count unalignedClass_it;
    if (!getClassIndexForType(datasetHeader, uType, &unalignedClass_it)) {
        return true;
    }

    if (datasetHeader->numberUAccessUnits == 0) {
        return true;
    }

    uint32_t multipleSignatureBase =
        getMultipleSignatureBase(getDatasetHeader(datasetMasterIndexTable->datasetContainer));
    uint64_t signatureSize = getUSignatureSize(getDatasetHeader(datasetMasterIndexTable->datasetContainer));
    uint32_t signatureLength = getSignatureLength(datasetHeader);
    // todo update this value
    uint8_t signatureBitsPerSymbol = 3;

    Descriptors_count numberDescriptorsUnmapped =
        getNumberDescriptorsInClass(getDatasetHeader(datasetMasterIndexTable->datasetContainer), unalignedClass_it);

    for (uint32_t uAccessUnit_i = 0; uAccessUnit_i < datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits;
         uAccessUnit_i++) {
        uint32_t uAccessUnitOffset = (uint32_t)datasetMasterIndexTable->uAccessUnitsIndex.AU_byte_offset[uAccessUnit_i];
        writeBigEndian32ToBitstream(outputBitstream, uAccessUnitOffset);

        if (getDatasetType(datasetHeader) == 2) {
            writeNBitsShiftAndConvertToBigEndian16(
                outputBitstream, 16, datasetMasterIndexTable->uAccessUnitsIndex.ref_sequence_id[uAccessUnit_i]);
            if (isPosOffsetUint40(datasetHeader)) {
                writeNBitsShiftAndConvertToBigEndian64(
                    outputBitstream, 40, datasetMasterIndexTable->uAccessUnitsIndex.ref_start_position[uAccessUnit_i]);
                writeNBitsShiftAndConvertToBigEndian64(
                    outputBitstream, 40, datasetMasterIndexTable->uAccessUnitsIndex.ref_end_position[uAccessUnit_i]);
            } else {
                writeNBitsShiftAndConvertToBigEndian32(
                    outputBitstream, 32,
                    (uint32_t)datasetMasterIndexTable->uAccessUnitsIndex.ref_start_position[uAccessUnit_i]);
                writeNBitsShiftAndConvertToBigEndian32(
                    outputBitstream, 32,
                    (uint32_t)datasetMasterIndexTable->uAccessUnitsIndex.ref_end_position[uAccessUnit_i]);
            }
        }

        if (multipleSignatureBase != 0) {
            size_t actualSignatureNumber =
                datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i]->allocated_signatures;
            if (actualSignatureNumber != multipleSignatureBase) {
                for (uint64_t signatureBit = 0; signatureBit < signatureSize; signatureBit++) {
                    writeBit(outputBitstream, 1);
                }
                writeNBitsShiftAndConvertToBigEndian16(outputBitstream, 16, (uint16_t)actualSignatureNumber);
            }
            SignaturesOutputStream signatureIntegerOutputStream;

            /*initSignaturesOutputStream(
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

            writeBuffer(outputBitstream);*/
        }
        if (!isBlockHeaderFlagSet(datasetHeader)) {
            for (int descriptorUnalignedAccessUnit_i = 0; descriptorUnalignedAccessUnit_i < numberDescriptorsUnmapped;
                 descriptorUnalignedAccessUnit_i++) {
                uint64_t block_offset = datasetMasterIndexTable->uAccessUnitsIndex
                                            .byteOffset[descriptorUnalignedAccessUnit_i][uAccessUnit_i];
                if (isByteOffsetUint64(datasetHeader)) {
                    writeBigEndian64ToBitstream(outputBitstream, block_offset);
                } else {
                    uint32_t casted_block_offset = (uint32_t)block_offset;
                    writeBigEndian32ToBitstream(outputBitstream, casted_block_offset);
                }
            }
        }
    }
    return true;
}

bool writeContentDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable, FILE* outputFile) {
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);

    writeAlignedIndexing(datasetMasterIndexTable, &outputBitstream);
    writeBuffer(&outputBitstream);
    writeUnalignedIndexing(datasetMasterIndexTable, &outputBitstream);
    return writeBuffer(&outputBitstream);
}

bool writeDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable, FILE* outputFile) {
    uint64_t datasetMasterIndexTableSize = getSizeDatasetMasterIndexTable(datasetMasterIndexTable);
    if (!writeBoxHeader(outputFile, datasetMasterIndexTableName, datasetMasterIndexTableSize)) {
        fprintf(stderr, "Error writing type and size of dataset master index table.");
        return false;
    }
    return writeContentDatasetMasterIndexTable(datasetMasterIndexTable, outputFile);
}

uint64_t getSizeContentUnalignedDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable) {
    DatasetHeader* datasetHeader = getDatasetHeader(datasetMasterIndexTable->datasetContainer);
    uint64_t contentSize = 0;
    uint64_t contentSizeAsBits = 0;

    uint32_t multipleSignatureBase =
        getMultipleSignatureBase(getDatasetHeader(datasetMasterIndexTable->datasetContainer));
    uint64_t signatureSizeInBits = getUSignatureSize(getDatasetHeader(datasetMasterIndexTable->datasetContainer));
    uint32_t signatureLengthInSymbols = getSignatureLength(getDatasetHeader(datasetMasterIndexTable->datasetContainer));

    uint8_t class_it_for_u;
    ClassType uType;
    uType.classType = CLASS_TYPE_CLASS_U;
    bool classU_found = getClassIndexForType(datasetHeader, uType, &class_it_for_u);
    if (!classU_found) {
        return contentSize;
    }

    uint16_t numberDescriptorsUnmapped =
        getNumberDescriptorsInClass(getDatasetHeader(datasetMasterIndexTable->datasetContainer), class_it_for_u);

    for (uint32_t uAccessUnit_i = 0; uAccessUnit_i < datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits;
         uAccessUnit_i++) {
        contentSize += bitsToBytes(isByteOffsetUint64(datasetHeader) ? 64 : 32);  // au_byte_offset

        if (getDatasetType(datasetHeader) == 2) {
            contentSize += 2;                                                            // ref_sequence_id
            contentSize += 2 * bitsToBytes(isPosOffsetUint40(datasetHeader) ? 40 : 32);  // ref_start_pos, ref_end_pos
        }

        uint64_t uAccessUnitsSignatureSizes = 0;
        if (multipleSignatureBase != 0) {
            size_t actualSignatureNumber =
                datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i]->allocated_signatures;
            if (actualSignatureNumber != multipleSignatureBase) {
                contentSize += bitsToBytes(signatureSizeInBits) + sizeof(uint16_t);
            }

            SignatureSizeComputation signatureSizeComputation;

            // todo find out bits per symbol based on alphabet
            uint8_t bitsPerSymbol = 3;
            initSignatureSizeComputation(&signatureSizeComputation, signatureLengthInSymbols,
                                         (uint8_t)signatureSizeInBits, bitsPerSymbol);
            /*for (uint16_t signature_i = 0; signature_i < actualSignatureNumber; signature_i++) {
                simulateSignature(
                        &signatureSizeComputation,
                        datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i]->signature + signature_i
                );
            }*/

            contentSize += bitsToBytes(getTotalSizeInBits(&signatureSizeComputation));
        }
        if (!isBlockHeaderFlagSet(getDatasetHeader(datasetMasterIndexTable->datasetContainer))) {
            contentSize +=
                (uint64_t)(numberDescriptorsUnmapped * (uint16_t)(isByteOffsetUint64(datasetHeader) ? 8 : 4));
        }
    }
    return contentSize;
}

uint64_t getSizeContentDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable) {
    uint64_t contentSize = 0;
    DatasetHeader* datasetHeader = getDatasetHeader(datasetMasterIndexTable->datasetContainer);
    for (uint16_t seq_id = 0; seq_id < datasetMasterIndexTable->sequence_indexes.number_sequences; seq_id++) {
        uint16_t classesCount = getClassesCount(getDatasetHeader(datasetMasterIndexTable->datasetContainer));

        uint32_t numberAUs = getBlocksInSequence(getDatasetHeader(datasetMasterIndexTable->datasetContainer), seq_id);
        for (uint8_t class_i = 0; class_i < classesCount; class_i++) {
            ClassType classType = getClassType(datasetHeader, class_i);
            uint8_t datasetType = getDatasetType(datasetHeader);
            if (classType.classType == CLASS_TYPE_CLASS_U) {
                continue;
            }
            for (typeof(numberAUs) auId = 0; auId < numberAUs; auId++) {
                AU_indexing au_indexing = datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_id]
                                              .classIndex[class_i]
                                              .au_indexings[auId];
                if (isByteOffsetUint64(datasetHeader)) {
                    contentSize += sizeof(uint64_t);
                } else {
                    contentSize += sizeof(uint32_t);
                }
                if (isPosOffsetUint40(datasetHeader)) {
                    contentSize += 2 * (bitsToBytes(40));
                } else {
                    contentSize += 2 * sizeof(uint32_t);
                }
                if (getDatasetType(datasetHeader) == 2) {
                    contentSize += sizeof(uint16_t);
                    if (isPosOffsetUint40(datasetHeader)) {
                        contentSize += 2 * (sizeof(uint32_t) + 1);
                    } else {
                        contentSize += 2 * sizeof(uint32_t);
                    }
                }

                if (isMultipleAlignment(getDatasetHeader(datasetMasterIndexTable->datasetContainer))) {
                    if (isPosOffsetUint40(datasetHeader)) {
                        contentSize += 2 * (sizeof(uint32_t) + 1);
                    } else {
                        contentSize += 2 * sizeof(uint32_t);
                    }
                }
                if (!isBlockHeaderFlagSet(getDatasetHeader(datasetMasterIndexTable->datasetContainer))) {
                    uint16_t numberDescriptors = au_indexing.block_byte_offset.descriptors_number;
                    if (isByteOffsetUint64(getDatasetHeader(datasetMasterIndexTable->datasetContainer))) {
                        contentSize += numberDescriptors * sizeof(uint64_t);
                    } else {
                        contentSize += numberDescriptors * sizeof(uint32_t);
                    }
                }
            }
        }
    }

    contentSize += getSizeContentUnalignedDatasetMasterIndexTable(datasetMasterIndexTable);
    return contentSize;
}

uint64_t getSizeDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable) {
    return BOX_HEADER_SIZE + getSizeContentDatasetMasterIndexTable(datasetMasterIndexTable);
}

uint64_t getAUStartPosition(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, uint16_t classId,
                            uint32_t AU_id) {
    uint16_t sequenceIndex;
    getSequenceIndex(getDatasetHeader(datasetMasterIndexTable->datasetContainer), sequenceId, &sequenceIndex);
    return datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex]
        .classIndex[classId]
        .au_indexings[AU_id]
        .au_start_position;
}

uint64_t getAUEndPosition(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, uint16_t classId,
                          uint32_t AU_id) {
    uint16_t sequenceIndex;
    getSequenceIndex(getDatasetHeader(datasetMasterIndexTable->datasetContainer), sequenceId, &sequenceIndex);
    return datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex]
        .classIndex[classId]
        .au_indexings[AU_id]
        .au_end_position;
}
uint16_t getRefSequenceId(DatasetMasterIndexTable* datasetMasterIndexTable, uint16_t sequenceIndex, uint16_t classIndex,
                          uint32_t AU_id) {
    return datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex]
        .classIndex[classIndex]
        .au_indexings[AU_id]
        .ref_sequence_id;
}
uint64_t getRefStartPosition(DatasetMasterIndexTable* datasetMasterIndexTable, uint16_t sequenceIndex,
                             uint16_t classIndex, uint32_t AU_id) {
    return datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex]
        .classIndex[classIndex]
        .au_indexings[AU_id]
        .ref_start_position;
}
uint64_t getRefEndPosition(DatasetMasterIndexTable* datasetMasterIndexTable, uint16_t sequenceIndex, uint16_t classId,
                           uint32_t AU_id) {
    return datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex]
        .classIndex[classId]
        .au_indexings[AU_id]
        .ref_end_position;
}
uint64_t getAUExtendedStartPosition(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId,
                                    uint16_t classId, uint32_t AU_id) {
    uint16_t sequenceIndex;
    getSequenceIndex(getDatasetHeader(datasetMasterIndexTable->datasetContainer), sequenceId, &sequenceIndex);
    return datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex]
        .classIndex[classId]
        .au_indexings[AU_id]
        .extended_au_start_position;
}

uint64_t getAUExtendedEndPosition(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId,
                                  uint16_t classId, uint32_t AU_id) {
    uint16_t sequenceIndex;
    getSequenceIndex(getDatasetHeader(datasetMasterIndexTable->datasetContainer), sequenceId, &sequenceIndex);
    return datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex]
        .classIndex[classId]
        .au_indexings[AU_id]
        .extended_au_end_position;
}

void setAUOffset(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, ClassType classType,
                 uint64_t AU_id, uint64_t offset) {
    uint8_t classIndex;
    getClassIndexForType(getDatasetHeader(datasetMasterIndexTable->datasetContainer), classType, &classIndex);

    uint16_t sequenceIndex;
    getSequenceIndex(getDatasetHeader(datasetMasterIndexTable->datasetContainer), sequenceId, &sequenceIndex);

    datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex]
        .classIndex[classIndex]
        .au_indexings[AU_id]
        .AU_byte_offset = offset;
}

void setStartAndEnd(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, ClassType classType,
                    uint64_t AU_id, uint64_t start, uint64_t end) {
    uint16_t sequenceIndex;
    getSequenceIndex(getDatasetHeader(datasetMasterIndexTable->datasetContainer), sequenceId, &sequenceIndex);
    if (sequenceIndex < datasetMasterIndexTable->sequence_indexes.number_sequences) {
        uint8_t classIndex;
        getClassIndexForType(getDatasetHeader(datasetMasterIndexTable->datasetContainer), classType, &classIndex);
        if (classIndex < datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex].number_of_classes) {
            if (AU_id < datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex]
                            .classIndex[classIndex]
                            .number_AUs) {
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex]
                    .classIndex[classIndex]
                    .au_indexings[AU_id]
                    .au_start_position = start;
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceIndex]
                    .classIndex[classIndex]
                    .au_indexings[AU_id]
                    .au_end_position = end;
            }
        }
    }
}

void setExtendedStartAndEnd(DatasetMasterIndexTable* datasetMasterIndexTable, uint16_t sequenceId, uint16_t classId,
                            uint32_t AU_id, uint32_t extended_start, uint32_t extended_end) {
    if (sequenceId < datasetMasterIndexTable->sequence_indexes.number_sequences) {
        if (classId < datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].number_of_classes) {
            if (AU_id <
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId].classIndex[classId].number_AUs) {
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId]
                    .classIndex[classId]
                    .au_indexings[AU_id]
                    .extended_au_start_position = extended_start;
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequenceId]
                    .classIndex[classId]
                    .au_indexings[AU_id]
                    .extended_au_end_position = extended_end;
            }
        }
    }
}

void setOffset(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceID, ClassType classType,
               uint32_t AU_i, uint16_t descriptor_i, uint64_t offset) {
    uint16_t sequence_i;
    getSequenceIndex(getDatasetHeader(datasetMasterIndexTable->datasetContainer), sequenceID, &sequence_i);
    uint8_t class_i;
    getClassIndexForType(getDatasetHeader(datasetMasterIndexTable->datasetContainer), classType, &class_i);
    if (sequence_i < datasetMasterIndexTable->sequence_indexes.number_sequences) {
        if (class_i < datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequence_i].number_of_classes) {
            if (AU_i <
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequence_i].classIndex[class_i].number_AUs) {
                if (descriptor_i < datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequence_i]
                                       .classIndex[class_i]
                                       .au_indexings[AU_i]
                                       .block_byte_offset.descriptors_number) {
                    datasetMasterIndexTable->sequence_indexes.sequence_indexes[sequence_i]
                        .classIndex[class_i]
                        .au_indexings[AU_i]
                        .block_byte_offset.block_byte_offset[descriptor_i] = offset;
                    if (!isBlockHeaderFlagSet(getDatasetHeader(datasetMasterIndexTable->datasetContainer))) {
                        TreeNode* treeNode = initTreeNode(offset);
                        if (insert(datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i]
                                       .offsetsPerStream[descriptor_i],
                                   treeNode)) {
                            datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[class_i]
                                .sizeTrees[descriptor_i]++;
                        } else {
                            free(treeNode);
                        }
                    }
                }
            }
        }
    }
}

void insertFinalOffset(DatasetMasterIndexTable* datasetMasterIndexTable, ClassType classType, uint16_t classIndex,
                       uint16_t descriptorIndex, uint64_t offset) {
    TreeNode* treeNode = initTreeNode(offset);
    if (classType.classType != CLASS_TYPE_CLASS_U) {
        if (insert(
                datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[classIndex].offsetsPerStream[descriptorIndex],
                treeNode)) {
            datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[classIndex].sizeTrees[descriptorIndex]++;
        } else {
            free(treeNode);
        }
    } else {
        if (!insert(datasetMasterIndexTable->uAccessUnitsIndex.offsetsDescriptorUnmappedTree[descriptorIndex],
                    treeNode)) {
            free(treeNode);
        }
    }
}

void setUnalignedOffset(DatasetMasterIndexTable* datasetMasterIndexTable, uint32_t uAU_id, uint32_t uDescriptorId,
                        uint64_t offset) {
    uint8_t unalignedClassId = 5;
    uint16_t numberDescriptorsUnmapped =
        getNumberDescriptorsInClass(getDatasetHeader(datasetMasterIndexTable->datasetContainer), unalignedClassId);
    if (datasetMasterIndexTable->uAccessUnitsIndex.byteOffset != NULL) {
        if (uAU_id < datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits &&
            uDescriptorId < numberDescriptorsUnmapped) {
            datasetMasterIndexTable->uAccessUnitsIndex.byteOffset[uDescriptorId][uAU_id] = offset;
        }
    }
}

Signatures* getSignatures(DatasetMasterIndexTable* datasetMasterIndexTable, int uAccessUnit_i) {
    return datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i];
}

void parseDatasetMasterIndexTableAlignedAUs(DatasetMasterIndexTable* datasetMasterIndexTable, FILE* inputFile) {
    DatasetContainer* datasetContainer = datasetMasterIndexTable->datasetContainer;
    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    for (uint16_t seq_i = 0; seq_i < datasetMasterIndexTable->sequence_indexes.number_sequences; seq_i++) {
        SequenceID seqId = getSequenceId(getDatasetHeader(datasetContainer), seq_i);
        uint16_t numberClassesForAligned =
            datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_i].number_of_classes;
        for (uint16_t class_i = 0; class_i < numberClassesForAligned; class_i++) {
            ClassType classType = getClassType(getDatasetHeader(datasetContainer), class_i);

            uint32_t numberAUs =
                datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_i].classIndex[class_i].number_AUs;
            for (uint32_t au_id = 0; au_id < numberAUs; au_id++) {
                AU_indexing* au_position = &(datasetMasterIndexTable->sequence_indexes.sequence_indexes[seq_i]
                                                 .classIndex[class_i]
                                                 .au_indexings[au_id]);

                if (isByteOffsetUint64(getDatasetHeader(datasetContainer))) {
                    readNBitsBigToNativeEndian64(&inputBitstream, 64, &(au_position->AU_byte_offset));
                } else {
                    uint32_t au_offset;
                    readNBitsBigToNativeEndian32(&inputBitstream, 32, &au_offset);
                    au_position->AU_byte_offset = au_offset;
                }

                if (isPosOffsetUint40(getDatasetHeader(datasetContainer))) {
                    readNBitsBigToNativeEndian64(&inputBitstream, 40, &(au_position->au_start_position));
                    readNBitsBigToNativeEndian64(&inputBitstream, 40, &(au_position->au_end_position));
                } else {
                    uint32_t au_start_position;
                    uint32_t au_end_position;
                    readNBitsBigToNativeEndian32(&inputBitstream, 32, &au_start_position);
                    au_position->au_start_position = au_start_position;
                    readNBitsBigToNativeEndian32(&inputBitstream, 32, &au_end_position);
                    au_position->au_end_position = au_end_position;
                }

                if (getDatasetType(getDatasetHeader(datasetContainer)) == 2) {
                    uint16_t ref_sequence_id;
                    uint64_t ref_start_position;
                    uint64_t ref_end_position;

                    readNBitsBigToNativeEndian16(&inputBitstream, 16, &ref_sequence_id);
                    if (isPosOffsetUint40(getDatasetHeader(datasetContainer))) {
                        readNBitsBigToNativeEndian64(&inputBitstream, 40, &ref_start_position);
                        readNBitsBigToNativeEndian64(&inputBitstream, 40, &ref_end_position);
                    } else {
                        readNBitsBigToNativeEndian64(&inputBitstream, 32, &ref_start_position);
                        readNBitsBigToNativeEndian64(&inputBitstream, 32, &ref_end_position);
                    }
                    au_position->ref_sequence_id = ref_sequence_id;
                    au_position->ref_start_position = ref_start_position;
                    au_position->ref_end_position = ref_end_position;
                }

                if (isMultipleAlignment(getDatasetHeader(datasetContainer))) {
                    if (isPosOffsetUint40(getDatasetHeader(datasetContainer))) {
                        readNBitsBigToNativeEndian64(&inputBitstream, 40, &(au_position->extended_au_start_position));
                        readNBitsBigToNativeEndian64(&inputBitstream, 40, &(au_position->extended_au_end_position));
                    } else {
                        uint32_t extended_au_start_position;
                        uint32_t extended_au_end_position;
                        readNBitsBigToNativeEndian32(&inputBitstream, 32, &extended_au_start_position);
                        au_position->extended_au_start_position = extended_au_start_position;
                        readNBitsBigToNativeEndian32(&inputBitstream, 32, &extended_au_end_position);
                        au_position->extended_au_end_position = extended_au_end_position;
                    }
                }

                datasetContainer->accessUnitsOffsetsInitiatedWithValues = true;
                datasetContainer->accessUnitsOffsets[seq_i][class_i][au_id] = au_position->AU_byte_offset;
                if (!isBlockHeaderFlagSet(getDatasetHeader(datasetContainer))) {
                    uint16_t numberDescriptors = au_position->block_byte_offset.descriptors_number;
                    for (uint16_t descriptorId = 0; descriptorId < numberDescriptors; descriptorId++) {
                        uint64_t block_offset;
                        if (isByteOffsetUint64(getDatasetHeader(datasetContainer))) {
                            readNBitsBigToNativeEndian64(&inputBitstream, 64, &block_offset);
                        } else {
                            uint32_t block_offset32;
                            readNBitsBigToNativeEndian32(&inputBitstream, 32, &block_offset32);
                            block_offset = block_offset32;
                        }
                        setOffset(datasetMasterIndexTable, seqId, classType, au_id, descriptorId, block_offset);
                    }
                }
            }
        }
    }
    forwardUntilAligned(&inputBitstream);
}

void parseDatasetMasterIndexTableUnalignedAUs(DatasetMasterIndexTable* datasetMasterIndexTable, FILE* inputFile) {
    DatasetContainer* datasetContainer = datasetMasterIndexTable->datasetContainer;
    DatasetHeader* datasetHeader = getDatasetHeader(datasetContainer);
    uint32_t multipleSignatureBase = getMultipleSignatureBase(datasetHeader);
    uint32_t signatureLengthInBits = getUSignatureSize(datasetHeader);
    uint64_t bytesNeeded = bitsToBytes(signatureLengthInBits);

    uint8_t* signatureBuffer = (uint8_t*)malloc(bytesNeeded * sizeof(uint8_t));

    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    ClassType uType;
    uType.classType = CLASS_TYPE_CLASS_U;

    uint8_t unalignedClass_i;
    bool foundUnalignedClassId = getClassIndexForType(datasetHeader, uType, &unalignedClass_i);
    if (!foundUnalignedClassId) {
        free(signatureBuffer);
        return;
    }
    uint16_t numberDescriptorsUnmapped = getNumberDescriptorsInClass(datasetHeader, unalignedClass_i);
    for (uint32_t uAccessUnit_i = 0; uAccessUnit_i < datasetMasterIndexTable->uAccessUnitsIndex.numberUAccessUnits;
         uAccessUnit_i++) {
        uint64_t blockUAccessUnitOffset;
        if (isByteOffsetUint64(getDatasetHeader(datasetContainer))) {
            readNBitsBigToNativeEndian64(&inputBitstream, 64, &blockUAccessUnitOffset);
        } else {
            uint32_t blockUAccessUnitOffset32;
            readNBitsBigToNativeEndian32(&inputBitstream, 32, &blockUAccessUnitOffset32);
            blockUAccessUnitOffset = blockUAccessUnitOffset32;
        }
        datasetMasterIndexTable->uAccessUnitsIndex.AU_byte_offset[uAccessUnit_i] = blockUAccessUnitOffset;

        if (getDatasetType(datasetHeader) == 2) {
            uint16_t ref_sequence_id;
            uint64_t ref_start_position;
            uint64_t ref_end_position;
            readNBitsBigToNativeEndian16(&inputBitstream, 16, &ref_sequence_id);
            if (isPosOffsetUint40(datasetHeader)) {
                readNBitsBigToNativeEndian64(&inputBitstream, 40, &ref_start_position);
                readNBitsBigToNativeEndian64(&inputBitstream, 40, &ref_end_position);
            } else {
                readNBitsBigToNativeEndian64(&inputBitstream, 32, &ref_start_position);
                readNBitsBigToNativeEndian64(&inputBitstream, 32, &ref_end_position);
            }

            datasetMasterIndexTable->uAccessUnitsIndex.ref_sequence_id[uAccessUnit_i] = ref_sequence_id;
            datasetMasterIndexTable->uAccessUnitsIndex.ref_start_position[uAccessUnit_i] = ref_start_position;
            datasetMasterIndexTable->uAccessUnitsIndex.ref_end_position[uAccessUnit_i] = ref_end_position;
        }

        if (multipleSignatureBase != 0) {
            uint16_t actualSignatureNumber = (uint16_t)multipleSignatureBase;
            readNBitsShift(&inputBitstream, signatureLengthInBits, (char*)signatureBuffer);
            bool allBitsToTrue = firstNBitsSetTo((char*)signatureBuffer, signatureLengthInBits, true);

            /*SignatureArbitraryLengthIntegerInputStream signatureIntegerInputStream;
            if(allBitsToTrue) {
                readNBitsBigToNativeEndian16(&inputBitstream, 16, (char *) &actualSignatureNumber);
                changeSizeSignatures(
                        datasetMasterIndexTable->uAccessUnitsIndex.signatures[uAccessUnit_i],
                        actualSignatureNumber
                );

                initSignatureInputStream(&signatureIntegerInputStream,
                                         &inputBitstream, (uint8_t) signatureLengthInBits, 3, 0);
            }else {
                initSignatureInputStreamAndFirstInteger(&signatureIntegerInputStream,
                                                        &inputBitstream, (uint8_t) signatureLengthInBits, 3, 0,
            signatureBuffer);
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
            freeSignatureInputStream(&signatureIntegerInputStream);*/
        }

        forwardUntilAligned(&inputBitstream);
        if (!isBlockHeaderFlagSet(datasetHeader)) {
            for (uint32_t descriptorUnalignedAccessUnit_i = 0;
                 descriptorUnalignedAccessUnit_i < numberDescriptorsUnmapped; descriptorUnalignedAccessUnit_i++) {
                uint64_t offset_buffer;
                if (isByteOffsetUint64(getDatasetHeader(datasetContainer))) {
                    readNBitsBigToNativeEndian64(&inputBitstream, 64, &offset_buffer);
                } else {
                    uint32_t offset_buffer32;
                    readNBitsBigToNativeEndian32(&inputBitstream, 32, &offset_buffer32);
                    offset_buffer = offset_buffer32;
                }
                setUnalignedOffset(datasetMasterIndexTable, uAccessUnit_i, descriptorUnalignedAccessUnit_i,
                                   offset_buffer);
            }
        }
    }
    free(signatureBuffer);
}

DatasetMasterIndexTable* parseDatasetMasterIndexTable(DatasetContainer* datasetContainer, FILE* inputFile) {
    DatasetMasterIndexTable* datasetMasterIndexTable = initDatasetMasterIndexTable(datasetContainer);
    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not get file position");
        freeDatasetMasterIndexTable(datasetMasterIndexTable);
        return NULL;
    }
    datasetMasterIndexTable->seekPosition = (size_t)seekPosition;

    parseDatasetMasterIndexTableAlignedAUs(datasetMasterIndexTable, inputFile);
    parseDatasetMasterIndexTableUnalignedAUs(datasetMasterIndexTable, inputFile);
    return datasetMasterIndexTable;
}

size_t getDatasetMasterIndexTableSeekPosition(DatasetMasterIndexTable* datasetMasterIndexTable) {
    return datasetMasterIndexTable->seekPosition;
}

bool getAccessUnitByteOffsetFromSequenceClass(AUs_index* aus_index, uint32_t auId, uint64_t* byteOffset) {
    if (auId < aus_index->number_AUs) {
        *byteOffset = aus_index->au_indexings[auId].AU_byte_offset;
        return true;
    } else {
        return false;
    }
}

bool getAccessUnitByteOffsetFromSequence(Sequence_indexation* sequence_index, uint16_t classId, uint32_t auId,
                                         uint64_t* byteOffset) {
    if (classId < sequence_index->number_of_classes) {
        return getAccessUnitByteOffsetFromSequenceClass(sequence_index->classIndex + classId, auId, byteOffset);
    } else {
        return false;
    }
}

bool getAccessUnitByteOffset(DatasetMasterIndexTable* datasetMasterIndexTable, uint16_t sequenceId, uint16_t classId,
                             uint32_t auId, uint64_t* byteOffset) {
    if (sequenceId < datasetMasterIndexTable->sequence_indexes.number_sequences) {
        return getAccessUnitByteOffsetFromSequence(
            datasetMasterIndexTable->sequence_indexes.sequence_indexes + sequenceId, classId, auId, byteOffset);
    } else {
        return false;
    }
}

bool getBlockByteOffsetFromSequenceClassAu(AU_indexing* au_indexing, uint32_t descriptorId, uint64_t* byteOffset) {
    if (descriptorId < au_indexing->block_byte_offset.descriptors_number) {
        *byteOffset = au_indexing->block_byte_offset.block_byte_offset[descriptorId];
        return true;
    } else {
        return false;
    }
}

bool getBlockByteOffsetFromSequenceClass(AUs_index* aus_index, uint32_t auId, uint32_t descriptorId,
                                         uint64_t* byteOffset) {
    if (auId < aus_index->number_AUs) {
        return getBlockByteOffsetFromSequenceClassAu(aus_index->au_indexings + auId, descriptorId, byteOffset);
    } else {
        return false;
    }
}

bool getBlockByteOffsetFromSequence(Sequence_indexation* sequenceIndex, ClassTypeEnum classId, uint32_t auId,
                                    uint32_t descriptorId, uint64_t* byteOffset) {
    if (classId < sequenceIndex->number_of_classes) {
        return getBlockByteOffsetFromSequenceClass(sequenceIndex->classIndex + classId, auId, descriptorId, byteOffset);
    } else {
        return false;
    }
}

bool getBlockByteOffset(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, ClassTypeEnum classId,
                        uint32_t auId, uint32_t descriptorId, uint64_t* byteOffset) {
    uint16_t sequenceIndex;
    getSequenceIndex(getDatasetHeader(datasetMasterIndexTable->datasetContainer), sequenceId, &sequenceIndex);
    if (sequenceIndex < datasetMasterIndexTable->sequence_indexes.number_sequences) {
        return getBlockByteOffsetFromSequence(
            datasetMasterIndexTable->sequence_indexes.sequence_indexes + sequenceIndex, classId, auId, descriptorId,
            byteOffset);
    } else {
        return false;
    }
}

bool getNextBlockByteOffset(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, uint16_t classId,
                            uint32_t auId, uint32_t descriptorId, uint64_t* nextByteOffset) {
    uint64_t blockStart;
    if (!getBlockByteOffset(datasetMasterIndexTable, sequenceId, classId, auId, descriptorId, &blockStart)) {
        return NULL;
    }
    Tree* byteOffsetsTree = NULL;
    if (classId < datasetMasterIndexTable->offsetsPerClass->numberClasses) {
        if (descriptorId < datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[classId].numberStreams) {
            byteOffsetsTree =
                datasetMasterIndexTable->offsetsPerClass->offsetsPerClass[classId].offsetsPerStream[descriptorId];
        }
    }
    if (byteOffsetsTree == NULL) {
        return NULL;
    }
    TreeNode* currentBlockOffsetTreeNode = findNode(byteOffsetsTree, blockStart);
    if (currentBlockOffsetTreeNode == NULL) {
        return NULL;
    }
    TreeNode* nextBlockOffsetTreeNode = getNext(currentBlockOffsetTreeNode);
    if (nextBlockOffsetTreeNode == NULL) {
        return NULL;
    }
    *nextByteOffset = nextBlockOffsetTreeNode->value;
    return true;
}
