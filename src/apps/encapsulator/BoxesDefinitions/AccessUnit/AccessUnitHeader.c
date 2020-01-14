//
// Created by gencom on 4/12/17.
//

#include "../../Boxes.h"
#include "../../DataStructures/BitStreams/InputBitstream.h"
#include "../../DataStructures/BitStreams/OutputBitstream.h"
#include "../../DataStructures/Signatures/SignatureSizeComputation.h"
#include "../../DataStructures/Signatures/Signatures.h"
#include "../../DataStructures/Signatures/SignaturesInputStream.h"
#include "../../DataStructures/Signatures/SignaturesOutputStream.h"
#include "../../utils.h"

bool writeUnalignedHeaderInformation(OutputBitstream* outputBitstream, AccessUnitHeader* accessUnitHeader);
AccessUnitHeader* initAccessUnitHeader() {
    AccessUnitHeader* accessUnitHeader = malloc(sizeof(AccessUnitHeader));
    if (accessUnitHeader != NULL) {
        accessUnitHeader->datasetContainer = NULL;
        accessUnitHeader->access_unit_ID = 0;
        accessUnitHeader->num_blocks = 0;
        accessUnitHeader->parameter_set_ID = 0;
        accessUnitHeader->au_type.classType = 0;
        accessUnitHeader->reads_count = 0;
        accessUnitHeader->mm_threshold = 0;
        accessUnitHeader->mm_count = 0;
        accessUnitHeader->seekPosition = -1;

        // if dataset_type == 2
        accessUnitHeader->ref_sequence_id.sequenceID = 0;
        accessUnitHeader->ref_start_position = 0;
        accessUnitHeader->ref_end_position = 0;

        // if MIT_flag == 0
        // if AU_type != CLASS_U || dataset_type == 2
        accessUnitHeader->sequence_ID.sequenceID = 0;
        accessUnitHeader->AU_start_position = 0;
        accessUnitHeader->AU_end_position = 0;
        // if multiple_alignment_flag
        accessUnitHeader->extended_AU_start_position = 0;
        accessUnitHeader->extended_AU_end_position = 0;
        // else
        accessUnitHeader->signatures = NULL;
    }
    return accessUnitHeader;
}

AccessUnitHeader* initAccessUnitHeaderWithValues(DatasetContainer* datasetContainer, uint32_t access_unit_ID,
                                                 uint8_t num_blocks, uint8_t parameter_set_ID, ClassType au_type,
                                                 uint32_t reads_count, uint16_t mm_threshold, uint32_t mm_count) {
    AccessUnitHeader* accessUnitHeader = malloc(sizeof(AccessUnitHeader));
    if (accessUnitHeader != NULL) {
        accessUnitHeader->datasetContainer = datasetContainer;
        accessUnitHeader->access_unit_ID = access_unit_ID;
        accessUnitHeader->num_blocks = num_blocks;
        accessUnitHeader->parameter_set_ID = parameter_set_ID;
        accessUnitHeader->au_type.classType = au_type.classType;
        accessUnitHeader->reads_count = reads_count;
        accessUnitHeader->mm_threshold = mm_threshold;
        accessUnitHeader->mm_count = mm_count;

        // if dataset_type == 2
        accessUnitHeader->ref_sequence_id.sequenceID = 0;
        accessUnitHeader->ref_start_position = 0;
        accessUnitHeader->ref_end_position = 0;

        // if MIT_flag == 0
        // if AU_type != CLASS_U || dataset_type == 2
        accessUnitHeader->sequence_ID.sequenceID = 0;
        accessUnitHeader->AU_start_position = 0;
        accessUnitHeader->AU_end_position = 0;
        // if multiple_alignment_flag
        accessUnitHeader->extended_AU_start_position = 0;
        accessUnitHeader->extended_AU_end_position = 0;
        // else
        accessUnitHeader->signatures = NULL;
    }
    return accessUnitHeader;
}

void freeAccessUnitHeader(AccessUnitHeader* accessUnitHeader) { free(accessUnitHeader); }

uint64_t getAccessUnitHeaderContentSize(AccessUnitHeader* accessUnitHeader) {
    uint8_t posSize = (uint8_t)(isPosOffsetUint40(getDatasetHeader(accessUnitHeader->datasetContainer)) ? 40 : 32);

    uint64_t accessUnitInBits = 0;
    accessUnitInBits += 32;
    accessUnitInBits += 8;
    accessUnitInBits += 8;
    accessUnitInBits += 4;
    accessUnitInBits += 32;
    if (accessUnitHeader->au_type.classType == CLASS_TYPE_CLASS_N ||
        accessUnitHeader->au_type.classType == CLASS_TYPE_CLASS_M) {
        accessUnitInBits += 16;
        accessUnitInBits += 32;
    }

    if (getDatasetType(getDatasetHeader(accessUnitHeader->datasetContainer)) == 2) {
        accessUnitInBits += 16;  // ref_sequence_id
        accessUnitInBits += posSize;
        accessUnitInBits += posSize;
    }

    if (!isMITFlagSet(getDatasetHeader(accessUnitHeader->datasetContainer))) {
        if (accessUnitHeader->au_type.classType != CLASS_TYPE_CLASS_U
            /*|| getDatasetType(getDatasetHeader(accessUnitHeader->datasetContainer))*/
        ) {
            accessUnitInBits += 16;       // sequence_ID
            accessUnitInBits += posSize;  // AU_start_position
            accessUnitInBits += posSize;  // AU_end_position

            if (isMultipleAlignment(getDatasetHeader(accessUnitHeader->datasetContainer))) {
                accessUnitInBits += posSize;  // extended_au_start_position
                accessUnitInBits += posSize;  // extended_au_end_position
            }
        } else {
            uint32_t multipleSignatureBase =
                getMultipleSignatureBase(getDatasetHeader(accessUnitHeader->datasetContainer));
            // todo correct this with entire support for all parameters
            if (multipleSignatureBase != 0 && accessUnitHeader->signatures != NULL) {
                size_t actualSignatureNumber = accessUnitHeader->signatures->allocated_signatures;

                uint64_t signatureSizeInBits = getUSignatureSize(getDatasetHeader(accessUnitHeader->datasetContainer));
                uint64_t signatureLengthInSymbols =
                    getSignatureLength(getDatasetHeader(accessUnitHeader->datasetContainer));

                if (actualSignatureNumber != multipleSignatureBase) {
                    accessUnitInBits += /*signal that actual number differs*/
                        signatureSizeInBits + /*the actual number*/ 16;
                }

                SignatureSizeComputation signatureSizeComputation;

                // todo find out bits per symbol based on alphabet
                uint8_t bitsPerSymbol = 3;
                initSignatureSizeComputation(&signatureSizeComputation, signatureLengthInSymbols,
                                             (uint8_t)signatureSizeInBits, bitsPerSymbol);
                for (uint16_t signature_i = 0; signature_i < actualSignatureNumber; signature_i++) {
                    simulateSignature(&signatureSizeComputation,
                                      getSignature(accessUnitHeader->signatures, signature_i));
                }

                accessUnitInBits += getTotalSizeInBits(&signatureSizeComputation);
            }
        }
    }

    return bitsToBytes(accessUnitInBits);
}

uint64_t getAccessUnitHeaderSize(AccessUnitHeader* accessUnitHeader) {
    return BOX_HEADER_SIZE + getAccessUnitHeaderContentSize(accessUnitHeader);
}

bool writeUnalignedHeaderInformation(OutputBitstream* outputBitstream, AccessUnitHeader* accessUnitHeader) {
    uint32_t multipleSignatureBase = getMultipleSignatureBase(getDatasetHeader(accessUnitHeader->datasetContainer));
    uint64_t signatureSize = getUSignatureSize(getDatasetHeader(accessUnitHeader->datasetContainer));
    uint32_t signatureLength = getSignatureLength(getDatasetHeader(accessUnitHeader->datasetContainer));
    // todo update this value
    uint8_t signatureBitsPerSymbol = 3;

    // todo correct with support of all parameters
    if (multipleSignatureBase != 0 && accessUnitHeader->signatures != NULL) {
        if (accessUnitHeader->signatures->number_signatures != multipleSignatureBase) {
            for (uint64_t signatureBit = 0; signatureBit < signatureSize; signatureBit++) {
                writeBit(outputBitstream, 1);
            }
            writeNBitsShiftAndConvertToBigEndian16(outputBitstream, 16,
                                                   (uint16_t)accessUnitHeader->signatures->number_signatures);
        }
        SignaturesOutputStream signatureIntegerOutputStream;

        /*initSignaturesOutputStream(
                &signatureIntegerOutputStream,
                outputBitstream,
                signatureLength,
                (uint8_t) signatureSize,
                signatureBitsPerSymbol
        );
        for (uint16_t signature_i = 0; signature_i < accessUnitHeader->signatures->number_signatures; signature_i++) {
            writeSignature(
                    &signatureIntegerOutputStream,
                    accessUnitHeader->signatures[signature_i].signature
            );
        }
        writeBufferSignature(&signatureIntegerOutputStream);
        writeBuffer(outputBitstream);*/
    }

    return true;
}

bool writeAccessUnitHeaderContent(FILE* outputFile, AccessUnitHeader* accessUnitHeader) {
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);

    bool accessUnitIdSuccessfulWrite = writeBigEndian32ToBitstream(&outputBitstream, accessUnitHeader->access_unit_ID);
    bool numBlocksSuccessfulWrite = writeToBitstream(&outputBitstream, accessUnitHeader->num_blocks);
    bool parameterSetIDSuccessfulWrite =
        writeNBitsShift(&outputBitstream, 8, (char*)&accessUnitHeader->parameter_set_ID);
    bool mpegg_rec::MpeggRecord::ClassTypeSuccessfulWrite =
        writeNBitsShift(&outputBitstream, 4, (const char*)&(accessUnitHeader->au_type));
    bool readsCountSuccessfulWrite = writeBigEndian32ToBitstream(&outputBitstream, accessUnitHeader->reads_count);

    if (!accessUnitIdSuccessfulWrite || !numBlocksSuccessfulWrite || !parameterSetIDSuccessfulWrite ||
        !mpegg_rec::MpeggRecord::ClassTypeSuccessfulWrite || !readsCountSuccessfulWrite) {
        fprintf(stderr, "Error writing access unit header.\n");
        return false;
    }

    if (accessUnitHeader->au_type.classType == CLASS_TYPE_CLASS_N ||
        accessUnitHeader->au_type.classType == CLASS_TYPE_CLASS_M) {
        bool mmThresholdSuccessfulWrite = writeBigEndian16ToBitstream(&outputBitstream, accessUnitHeader->mm_threshold);
        bool mmCountSuccessfulWrite = writeBigEndian32ToBitstream(&outputBitstream, accessUnitHeader->mm_count);
        if (!mmThresholdSuccessfulWrite || !mmCountSuccessfulWrite) {
            fprintf(stderr, "Error writing access unit header.\n");
            return false;
        }
    }

    if (getDatasetType(getDatasetHeader(accessUnitHeader->datasetContainer)) == 2) {
        bool ref_sequence_idSuccessfulWrite =
            writeBigEndian16ToBitstream(&outputBitstream, accessUnitHeader->ref_sequence_id.sequenceID);
        bool ref_start_positionSuccessfulWrite;
        bool ref_end_positionSuccessfulWrite;
        if (isPosOffsetUint40(getDatasetHeader(accessUnitHeader->datasetContainer))) {
            ref_start_positionSuccessfulWrite =
                writeNBitsShiftAndConvertToBigEndian64(&outputBitstream, 40, accessUnitHeader->ref_start_position);
            ref_end_positionSuccessfulWrite =
                writeNBitsShiftAndConvertToBigEndian64(&outputBitstream, 40, accessUnitHeader->ref_end_position);
        } else {
            ref_start_positionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                &outputBitstream, 32, (uint32_t)accessUnitHeader->ref_start_position);
            ref_end_positionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                &outputBitstream, 32, (uint32_t)accessUnitHeader->ref_end_position);
        }
        if (!ref_sequence_idSuccessfulWrite || !ref_start_positionSuccessfulWrite || !ref_end_positionSuccessfulWrite) {
            fprintf(stderr, "Error writing access unit header.\n");
            return false;
        }
    }
    if (!isMITFlagSet(getDatasetHeader(accessUnitHeader->datasetContainer))) {
        if (accessUnitHeader->au_type.classType != CLASS_TYPE_CLASS_U) {
            bool sequenceIdSuccessfulWrite =
                writeNBitsShiftAndConvertToBigEndian16(&outputBitstream, 16, accessUnitHeader->sequence_ID.sequenceID);
            bool auStartPositionSuccessfulWrite;
            bool auEndPositionSuccessfulWrite;
            if (isPosOffsetUint40(getDatasetHeader(accessUnitHeader->datasetContainer))) {
                auStartPositionSuccessfulWrite =
                    writeNBitsShiftAndConvertToBigEndian64(&outputBitstream, 40, accessUnitHeader->AU_start_position);
                auEndPositionSuccessfulWrite =
                    writeNBitsShiftAndConvertToBigEndian64(&outputBitstream, 40, accessUnitHeader->AU_end_position);
            } else {
                auStartPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                    &outputBitstream, 32, (uint32_t)accessUnitHeader->AU_start_position);
                auEndPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                    &outputBitstream, 32, (uint32_t)accessUnitHeader->AU_end_position);
            }
            if (!sequenceIdSuccessfulWrite || !auStartPositionSuccessfulWrite || !auEndPositionSuccessfulWrite) {
                fprintf(stderr, "Error writing access unit header position information.\n");
                return false;
            }

            if (isMultipleAlignment(getDatasetHeader(accessUnitHeader->datasetContainer))) {
                bool extendedAuStartPositionSuccessfulWrite;
                bool extendedAuEndPositionSuccessfulWrite;
                if (isPosOffsetUint40(getDatasetHeader(accessUnitHeader->datasetContainer))) {
                    extendedAuStartPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64(
                        &outputBitstream, 40, accessUnitHeader->AU_start_position);
                    extendedAuEndPositionSuccessfulWrite =
                        writeNBitsShiftAndConvertToBigEndian64(&outputBitstream, 40, accessUnitHeader->AU_end_position);
                } else {
                    extendedAuStartPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                        &outputBitstream, 32, (uint32_t)accessUnitHeader->extended_AU_start_position);
                    extendedAuEndPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                        &outputBitstream, 32, (uint32_t)accessUnitHeader->extended_AU_end_position);
                }
                if (!extendedAuStartPositionSuccessfulWrite || !extendedAuEndPositionSuccessfulWrite) {
                    fprintf(stderr, "Error writing access unit header position information.\n");
                    return false;
                }
            }
        } else {
            writeUnalignedHeaderInformation(&outputBitstream, accessUnitHeader);
        }
    }

    // todo add mit information
    if (!writeBuffer(&outputBitstream)) {
        fprintf(stderr, "Error writing access unit header bitstream buffer.\n");
        return false;
    }
    return true;
}

bool writeAccessUnitHeader(FILE* outputFile, AccessUnitHeader* accessUnitHeader) {
    uint64_t headerSize = getAccessUnitHeaderSize(accessUnitHeader);
    if (!writeBoxHeader(outputFile, accessUnitHeaderName, headerSize)) {
        fprintf(stderr, "Error writing access unit header's box header.\n");
        return false;
    };
    return writeAccessUnitHeaderContent(outputFile, accessUnitHeader);
}

AccessUnitHeader* parseAccessUnitHeader(FILE* inputFile, DatasetContainer* datasetContainer) {
    long seekPosition = ftell(inputFile);
    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    uint32_t accessUnitIdBuffer;
    uint8_t numBlocksBuffer;
    uint8_t parameterSetIdBuffer;
    uint8_t mpegg_rec::MpeggRecord::ClassTypeBuffer;
    uint32_t readsCountBuffer;
    uint16_t mmThresholdBuffer = 0;
    uint32_t mmCountBuffer = 0;

    bool accessUnitIdSuccessfulRead = readNBitsBigToNativeEndian32(&inputBitstream, 32, &accessUnitIdBuffer);
    bool numBlocksSuccessfulRead = readBytes(&inputBitstream, 1, (char*)&numBlocksBuffer);
    bool parameterSetIdSuccessfulRead = readNBitsShift(&inputBitstream, 8, (char*)&parameterSetIdBuffer);
    bool mpegg_rec::MpeggRecord::ClassTypeSuccessfulRead =
        readNBitsShift(&inputBitstream, 4, (char*)&mpegg_rec::MpeggRecord::ClassTypeBuffer);
    bool readsCountBufferSuccessfulRead = readNBitsBigToNativeEndian32(&inputBitstream, 32, &readsCountBuffer);
    if (!accessUnitIdSuccessfulRead || !numBlocksSuccessfulRead || !parameterSetIdSuccessfulRead ||
        !mpegg_rec::MpeggRecord::ClassTypeSuccessfulRead || !readsCountBufferSuccessfulRead) {
        fprintf(stderr, "Error parsing access unit header.\n");
        return false;
    }

    if (mpegg_rec::MpeggRecord::ClassTypeBuffer == CLASS_TYPE_CLASS_N ||
        mpegg_rec::MpeggRecord::ClassTypeBuffer == CLASS_TYPE_CLASS_M) {
        bool mmThresholdSuccessfulRead = readNBitsBigToNativeEndian16(&inputBitstream, 16, &mmThresholdBuffer);
        bool mmCountSuccessfulRead = readNBitsBigToNativeEndian32(&inputBitstream, 32, &mmCountBuffer);
        if (!mmThresholdSuccessfulRead || !mmCountSuccessfulRead) {
            fprintf(stderr, "Error parsing access unit header.\n");
            return false;
        }
    }

    // if dataset_type == 2
    SequenceID ref_sequence_id;
    ref_sequence_id.sequenceID = 0;
    uint64_t ref_start_position = 0;
    uint64_t ref_end_position = 0;

    // if MIT_flag == 0
    // if AU_type != CLASS_U || dataset_type == 2
    SequenceID sequence_ID;
    sequence_ID.sequenceID = 0;
    uint64_t AU_start_position = 0;
    uint64_t AU_end_position = 0;
    // if multiple_alignment_flag
    uint64_t extended_AU_start_position = 0;
    uint64_t extended_AU_end_position = 0;
    // else
    Signatures* signatures = NULL;

    if (getDatasetType(getDatasetHeader(datasetContainer)) == 2) {
        bool ref_sequence_id_SuccessfulRead =
            readNBitsBigToNativeEndian16(&inputBitstream, 16, &(ref_sequence_id.sequenceID));
        bool ref_start_position_SuccessfulRead;
        bool ref_end_position_SuccessfulRead;
        if (isPosOffsetUint40(getDatasetHeader(datasetContainer))) {
            ref_start_position_SuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 40, &ref_start_position);
            ref_end_position_SuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 40, &ref_end_position);
        } else {
            ref_start_position_SuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 32, &ref_start_position);
            ref_end_position_SuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 32, &ref_end_position);
        }
    }
    if (!isMITFlagSet(getDatasetHeader(datasetContainer))) {
        if (mpegg_rec::MpeggRecord::ClassTypeBuffer != CLASS_TYPE_CLASS_U
            /*|| getDatasetType(getDatasetHeader(datasetContainer)) == 2*/
        ) {
            bool sequence_ID_successfulRead =
                readNBitsBigToNativeEndian16(&inputBitstream, 16, &(sequence_ID.sequenceID));

            bool auStartPositionSuccessfulRead;
            bool auEndPositionSuccessfulRead;

            if (isPosOffsetUint40(getDatasetHeader(datasetContainer))) {
                auStartPositionSuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 40, &AU_start_position);
                auEndPositionSuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 40, &AU_end_position);
            } else {
                auStartPositionSuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 32, &AU_start_position);
                auEndPositionSuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 32, &AU_end_position);
            }

            if (!auStartPositionSuccessfulRead || !auEndPositionSuccessfulRead) {
                fprintf(stderr, "Error reading access unit header.\n");
                return false;
            }

            if (isMultipleAlignment(getDatasetHeader(datasetContainer))) {
                bool extendedAuStartPositionSuccessfulRead;
                bool extendedAuEndPositionSuccessfulRead;

                if (isPosOffsetUint40(getDatasetHeader(datasetContainer))) {
                    extendedAuStartPositionSuccessfulRead =
                        readNBitsBigToNativeEndian64(&inputBitstream, 40, &extended_AU_start_position);
                    extendedAuEndPositionSuccessfulRead =
                        readNBitsBigToNativeEndian64(&inputBitstream, 40, &extended_AU_end_position);
                } else {
                    extendedAuStartPositionSuccessfulRead =
                        readNBitsBigToNativeEndian64(&inputBitstream, 32, &extended_AU_start_position);
                    extendedAuEndPositionSuccessfulRead =
                        readNBitsBigToNativeEndian64(&inputBitstream, 32, &extended_AU_end_position);
                }

                if (!extendedAuStartPositionSuccessfulRead || !extendedAuEndPositionSuccessfulRead) {
                    fprintf(stderr, "Error reading access unit header.\n");
                    return false;
                }
            }
        } else {
            uint32_t multipleSignatureBase = getMultipleSignatureBase(getDatasetHeader(datasetContainer));
            uint32_t signatureLengthInBits = getSignatureLength(getDatasetHeader(datasetContainer));
            uint64_t bytesNeeded = bitsToBytes(signatureLengthInBits);
            // uint8_t* signatureBuffer = (uint8_t*) malloc(bytesNeeded*sizeof(uint8_t));

            if (multipleSignatureBase != 0) {
                /*uint16_t actualSignatureNumber = (uint16_t) multipleSignatureBase;
                readNBitsShift(&inputBitstream,signatureLengthInBits,(char*)signatureBuffer);
                bool allBitsToTrue = firstNBitsSetTo((char*)signatureBuffer, signatureLengthInBits, true);


                SignatureArbitraryLengthIntegerInputStream signatureIntegerInputStream;
                if(allBitsToTrue) {
                    readNBitsBigToNativeEndian16(&inputBitstream, 16, (char *) &actualSignatureNumber);
                    changeSizeSignatures(
                            signatures,
                            actualSignatureNumber
                    );

                    initSignatureInputStream(&signatureIntegerInputStream,
                                             &inputBitstream, (uint8_t) signatureLengthInBits, 3, 0);
                }else {
                    initSignatureInputStreamAndFirstInteger(&signatureIntegerInputStream,
                                                            &inputBitstream, (uint8_t) signatureLengthInBits, 3, 0,
                signatureBuffer);
                }

                signatures = initSignatures(actualSignatureNumber);

                for(uint16_t signature_i=0; signature_i<actualSignatureNumber; signature_i++) {
                    Signature signature = getSignatureFromInputStream(&signatureIntegerInputStream);
                    *getSignature(
                            signatures,
                            signature_i
                    ) = signature;
                    if(signature_i != actualSignatureNumber-1){
                        getNewInteger(&signatureIntegerInputStream);
                    }
                }
                freeSignatureInputStream(&signatureIntegerInputStream);*/
            }
        }
    }

    AccessUnitHeader* accessUnitHeader = initAccessUnitHeader();
    accessUnitHeader->access_unit_ID = accessUnitIdBuffer;
    accessUnitHeader->num_blocks = numBlocksBuffer;
    accessUnitHeader->parameter_set_ID = parameterSetIdBuffer;
    accessUnitHeader->au_type.classType = mpegg_rec::MpeggRecord::ClassTypeBuffer;
    accessUnitHeader->reads_count = readsCountBuffer;
    accessUnitHeader->mm_threshold = mmThresholdBuffer;
    accessUnitHeader->mm_count = mmCountBuffer;
    accessUnitHeader->seekPosition = seekPosition;
    accessUnitHeader->datasetContainer = datasetContainer;
    accessUnitHeader->ref_sequence_id = ref_sequence_id;
    accessUnitHeader->ref_start_position = ref_start_position;
    accessUnitHeader->ref_end_position = ref_end_position;
    accessUnitHeader->sequence_ID = sequence_ID;
    accessUnitHeader->AU_start_position = AU_start_position;
    accessUnitHeader->AU_end_position = AU_end_position;
    accessUnitHeader->extended_AU_start_position = extended_AU_start_position;
    accessUnitHeader->extended_AU_end_position = extended_AU_end_position;
    accessUnitHeader->signatures = signatures;

    return accessUnitHeader;
}

long getAccessUnitHeaderSeekPosition(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->seekPosition; }

uint8_t getNumBlocks(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->num_blocks; }

uint32_t getAccessUnitId(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->access_unit_ID; }

uint16_t getParametersSetId(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->parameter_set_ID; }

uint32_t getReadsCount(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->reads_count; }

SequenceID getReferenceSequence(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->ref_sequence_id; }
uint64_t getReferenceStart(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->ref_start_position; }
uint64_t getReferenceEnd(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->ref_end_position; }

uint16_t getMMThreshold(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->mm_threshold; }

uint32_t getMMCount(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->mm_count; }

ClassType getAccessUnitType(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->au_type; }
SequenceID getAccessUnitSequenceID(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->sequence_ID; }
uint64_t getAccessUnitStart(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->AU_start_position; }
uint64_t getAccessUnitEnd(AccessUnitHeader* accessUnitHeader) { return accessUnitHeader->AU_end_position; }
uint64_t getExtendedAccessUnitStart(AccessUnitHeader* accessUnitHeader) {
    return accessUnitHeader->extended_AU_start_position;
}
uint64_t getExtendedAccessUnitEnd(AccessUnitHeader* accessUnitHeader) {
    return accessUnitHeader->extended_AU_end_position;
}
int setAccessUnitHeaderSequence_ID(AccessUnitHeader* accessUnitHeader, SequenceID sequenceId) {
    if (accessUnitHeader == NULL) return -1;
    accessUnitHeader->sequence_ID = sequenceId;
    return 0;
}
int setAccessUnitHeaderAuStartPosition(AccessUnitHeader* accessUnitHeader, uint64_t auStartPosition) {
    if (accessUnitHeader == NULL) return -1;
    accessUnitHeader->AU_start_position = auStartPosition;
    return 0;
}
int setAccessUnitHeaderAuEndPosition(AccessUnitHeader* accessUnitHeader, uint64_t auEndPosition) {
    if (accessUnitHeader == NULL) return -1;
    accessUnitHeader->AU_end_position = auEndPosition;
    return 0;
}
int setAccessUnitHeaderAuExtendedStartPosition(AccessUnitHeader* accessUnitHeader, uint64_t auExtendedStartPosition) {
    if (accessUnitHeader == NULL) return -1;
    accessUnitHeader->extended_AU_start_position = auExtendedStartPosition;
    return 0;
}
int setAccessUnitHeaderAuExtendedEndPosition(AccessUnitHeader* accessUnitHeader, uint64_t auExtendedEndPosition) {
    if (accessUnitHeader == NULL) return -1;
    accessUnitHeader->extended_AU_end_position = auExtendedEndPosition;
    return 0;
}

int setAccessUnitHeaderReferenceSequence_ID(AccessUnitHeader* accessUnitHeader, SequenceID referenceSequenceId) {
    if (accessUnitHeader == NULL) return -1;
    accessUnitHeader->ref_sequence_id = referenceSequenceId;
    return 0;
}

int setAccessUnitHeaderReferenceStartPosition(AccessUnitHeader* accessUnitHeader, uint64_t referenceStartPosition) {
    if (accessUnitHeader == NULL) return -1;
    accessUnitHeader->ref_start_position = referenceStartPosition;
    return 0;
}

int setAccessUnitHeaderReferenceEndPosition(AccessUnitHeader* accessUnitHeader, uint64_t referenceEndPosition) {
    if (accessUnitHeader == NULL) return -1;
    accessUnitHeader->ref_end_position = referenceEndPosition;
    return 0;
}
