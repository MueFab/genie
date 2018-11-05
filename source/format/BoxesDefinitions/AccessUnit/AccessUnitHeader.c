//
// Created by gencom on 4/12/17.
//

#include "DataStructures/Signatures/SignatureIntegerOutputStream.h"
#include "DataStructures/Signatures/SignatureIntegerOutputStream.h"
#include "DataStructures/Signatures/Signatures.h"
#include "DataStructures/Signatures/SignatureIntegerInputStream.h"
#include "DataStructures/Signatures/SignatureSizeComputation.h"
#include "../../Boxes.h"
#include "../../utils.h"
#include "../../DataStructures/BitStreams/InputBitstream.h"
#include "../../DataStructures/BitStreams/OutputBitstream.h"

AccessUnitHeader* initAccessUnitHeader(){
    AccessUnitHeader* accessUnitHeader = malloc(sizeof(AccessUnitHeader));
    if (accessUnitHeader != NULL){
        accessUnitHeader->datasetContainer = NULL;
        accessUnitHeader->access_unit_ID = 0;
        accessUnitHeader->num_blocks = 0;
        accessUnitHeader->parameter_set_ID = 0;
        accessUnitHeader->au_type = 0;
        accessUnitHeader->reads_count = 0;
        accessUnitHeader->mm_threshold = 0;
        accessUnitHeader->mm_count = 0;
        accessUnitHeader->seekPosition = -1;

        //if dataset_type == 2
            accessUnitHeader->ref_sequence_id = 0;
            accessUnitHeader->ref_start_position = 0;

        //if MIT_flag == 0
            //if AU_type != U_TYPE_AU || dataset_type == 2
                accessUnitHeader->sequence_ID = 0;
                accessUnitHeader->AU_start_position = 0;
                accessUnitHeader->AU_end_position = 0;
                //if multiple_alignment_flag
                    accessUnitHeader->extended_AU_start_position = 0;
                    accessUnitHeader->extended_AU_end_position = 0;
            //else
                accessUnitHeader->signatures = NULL;
    }
    return accessUnitHeader;
}

AccessUnitHeader* initAccessUnitHeaderWithValues(
    DatasetContainer* datasetContainer,
    uint32_t access_unit_ID,
    uint8_t num_blocks,
    uint16_t parameter_set_ID,
    uint8_t au_type,
    uint32_t reads_count,
    uint16_t mm_threshold,
    uint32_t mm_count
){
    AccessUnitHeader* accessUnitHeader = malloc(sizeof(AccessUnitHeader));
    if (accessUnitHeader != NULL){
        accessUnitHeader->datasetContainer = datasetContainer;
        accessUnitHeader->access_unit_ID = access_unit_ID;
        accessUnitHeader->num_blocks = num_blocks;
        accessUnitHeader->parameter_set_ID = parameter_set_ID;
        accessUnitHeader->au_type = au_type;
        accessUnitHeader->reads_count = reads_count;
        accessUnitHeader->mm_threshold = mm_threshold;
        accessUnitHeader->mm_count = mm_count;

        //if dataset_type == 2
            accessUnitHeader->ref_sequence_id = 0;
            accessUnitHeader->ref_start_position = 0;

        //if MIT_flag == 0
            //if AU_type != U_TYPE_AU || dataset_type == 2
                accessUnitHeader->sequence_ID = 0;
                accessUnitHeader->AU_start_position = 0;
                accessUnitHeader->AU_end_position = 0;
            //if multiple_alignment_flag
                accessUnitHeader->extended_AU_start_position = 0;
                accessUnitHeader->extended_AU_end_position = 0;
        //else
            accessUnitHeader->signatures = NULL;
    }
    return accessUnitHeader;
}

void freeAccessUnitHeader(AccessUnitHeader* accessUnitHeader){
    free(accessUnitHeader);
}

uint64_t getAccessUnitHeaderContentSize(AccessUnitHeader *accessUnitHeader){
    uint8_t posSize = (uint8_t) (isPosOffsetUint40(getDatasetHeader(accessUnitHeader->datasetContainer))?40:32);

    uint64_t accessUnitInBits = 0;
    accessUnitInBits += 32;
    accessUnitInBits += 8;
    accessUnitInBits += 12;
    accessUnitInBits += 4;
    accessUnitInBits += 32;
    if (accessUnitHeader->au_type == N_TYPE_AU || accessUnitHeader->au_type == M_TYPE_AU ){
        accessUnitInBits += 16;
        accessUnitInBits += 32;
    }

    if(getDatasetType(getDatasetHeader(accessUnitHeader->datasetContainer)) == 2){
        accessUnitInBits += 16; //ref_sequence_id
        accessUnitInBits += posSize;
    }

    if(!isMITFlagSet(getDatasetHeader(accessUnitHeader->datasetContainer))){
        if(
            accessUnitHeader->au_type != U_TYPE_AU
            || getDatasetType(getDatasetHeader(accessUnitHeader->datasetContainer))
        ){
            accessUnitInBits += 16; //sequence_ID
            accessUnitInBits += posSize; //AU_start_position
            accessUnitInBits += posSize; //AU_end_position

            if(isMultipleAlignment(getDatasetHeader(accessUnitHeader->datasetContainer))){
                accessUnitInBits += posSize; //extended_au_start_position
                accessUnitInBits += posSize; //extended_au_end_position
            }
        }else{
            size_t actualSignatureNumber = accessUnitHeader->signatures->allocated_signatures;
            uint32_t multipleSignatureBase = getMultipleSignatureBase(getDatasetHeader(accessUnitHeader->datasetContainer));
            uint64_t signatureSizeInBits = getUSignatureSize(getDatasetHeader(accessUnitHeader->datasetContainer));
            int signatureLengthInSymbols = getSignatureLength(getDatasetHeader(accessUnitHeader->datasetContainer));

            if (actualSignatureNumber != multipleSignatureBase) {
                accessUnitInBits += /*signal that actual number differs*/ signatureSizeInBits + /*the actual number*/16;
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
                        getSignature(accessUnitHeader->signatures, signature_i)
                );
            }

            accessUnitInBits += getTotalSizeInBits(&signatureSizeComputation);
        }
    }

    return bitsToBytes(accessUnitInBits);
}

uint64_t getAccessUnitHeaderSize(AccessUnitHeader *accessUnitHeader){
    return BOX_HEADER_SIZE + getAccessUnitHeaderContentSize(accessUnitHeader);
}

bool writeUnalignedHeaderInformation(OutputBitstream* outputBitstream, AccessUnitHeader* accessUnitHeader){
    uint32_t multipleSignatureBase = getMultipleSignatureBase(getDatasetHeader(accessUnitHeader->datasetContainer));
    uint64_t signatureSize = getUSignatureSize(getDatasetHeader(accessUnitHeader->datasetContainer));
    uint32_t signatureLength = getSignatureLength(getDatasetHeader(accessUnitHeader->datasetContainer));
    //todo update this value
    uint8_t signatureBitsPerSymbol = 3;


    if (multipleSignatureBase != 0) {
        if (accessUnitHeader->signatures->number_signatures != multipleSignatureBase) {
            for (uint64_t signatureBit = 0; signatureBit < signatureSize; signatureBit++) {
                writeBit(outputBitstream, 1);
            }
            writeNBitsShiftAndConvertToBigEndian16(
                    outputBitstream, 16, (uint16_t) accessUnitHeader->signatures->number_signatures);
        }
        SignatureIntegerOutputStream signatureIntegerOutputStream;


        initSignatureOutputStream(
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
        writeBuffer(outputBitstream);
    }

    return true;
}

bool writeAccessUnitHeaderContent(FILE* outputFile, AccessUnitHeader* accessUnitHeader){
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);

    bool accessUnitIdSuccessfulWrite =
            writeBigEndian32ToBitstream(&outputBitstream, accessUnitHeader->access_unit_ID);
    bool numBlocksSuccessfulWrite = writeToBitstream(&outputBitstream, accessUnitHeader->num_blocks);
    bool parameterSetIDSuccessfulWrite =
            writeNBitsShiftAndConvertToBigEndian16(&outputBitstream, 12, accessUnitHeader->parameter_set_ID);
    bool auTypeSuccessfulWrite = writeNBitsShift(&outputBitstream, 4, (const char *) &(accessUnitHeader->au_type));
    bool readsCountSuccessfulWrite = writeBigEndian32ToBitstream(&outputBitstream, accessUnitHeader->reads_count);

    if(
        !accessUnitIdSuccessfulWrite ||
        !numBlocksSuccessfulWrite ||
        !parameterSetIDSuccessfulWrite ||
        !auTypeSuccessfulWrite ||
        !readsCountSuccessfulWrite
    ){
        fprintf(stderr, "Error writing access unit header.\n");
        return false;
    }

    if(accessUnitHeader->au_type == N_TYPE_AU || accessUnitHeader->au_type == M_TYPE_AU){
        bool mmThresholdSuccessfulWrite =
                writeBigEndian16ToBitstream(&outputBitstream, accessUnitHeader->mm_threshold);
        bool mmCountSuccessfulWrite =
                writeBigEndian32ToBitstream(&outputBitstream, accessUnitHeader->mm_count);
        if(
            !mmThresholdSuccessfulWrite ||
            !mmCountSuccessfulWrite
        ){
            fprintf(stderr, "Error writing access unit header.\n");
            return false;
        }
    }

    if(getDatasetType(getDatasetHeader(accessUnitHeader->datasetContainer))==2){
        bool ref_sequence_idSuccessfulWrite = writeBigEndian16ToBitstream(
                &outputBitstream,
                accessUnitHeader->ref_sequence_id
        );
        bool ref_start_positionSuccessfulWrite;
        if(isPosOffsetUint40(getDatasetHeader(accessUnitHeader->datasetContainer))){
            ref_start_positionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64(
                    &outputBitstream, 40,
                    accessUnitHeader->ref_start_position
            );
        }else{
            ref_start_positionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                    &outputBitstream, 32,
                    (uint32_t)accessUnitHeader->ref_start_position
            );
        }
        bool ref_end_positionSuccessfulWrite;
        if(isPosOffsetUint40(getDatasetHeader(accessUnitHeader->datasetContainer))){
            ref_end_positionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64(
                    &outputBitstream, 40,
                    accessUnitHeader->ref_end_position
            );
        }else{
            ref_end_positionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                    &outputBitstream, 32,
                    (uint32_t)accessUnitHeader->ref_end_position
            );
        }
        if(
                !ref_sequence_idSuccessfulWrite ||
                !ref_start_positionSuccessfulWrite ||
                !ref_end_positionSuccessfulWrite
        ){
            fprintf(stderr, "Error writing access unit header.\n");
            return false;
        }
    }
    if(!isMITFlagSet(getDatasetHeader(accessUnitHeader->datasetContainer))){
        if(accessUnitHeader->au_type != U_TYPE_AU
           || getDatasetType(getDatasetHeader(accessUnitHeader->datasetContainer))==2
        ) {
            bool sequenceIdSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian16(
                    &outputBitstream, 16,
                    accessUnitHeader->sequence_ID
            );
            bool auStartPositionSuccessfulWrite;
            bool auEndPositionSuccessfulWrite;
            if (isPosOffsetUint40(getDatasetHeader(accessUnitHeader->datasetContainer))) {
                auStartPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64(
                        &outputBitstream, 40,
                        accessUnitHeader->AU_start_position
                );
                auEndPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64(
                        &outputBitstream, 40,
                        accessUnitHeader->AU_end_position
                );
            } else {
                auStartPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                        &outputBitstream, 32,
                        (uint32_t) accessUnitHeader->AU_start_position
                );
                auEndPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                        &outputBitstream, 32,
                        (uint32_t) accessUnitHeader->AU_end_position
                );
            }
            if (
                !sequenceIdSuccessfulWrite
                || !auStartPositionSuccessfulWrite
                || !auEndPositionSuccessfulWrite
            ) {
                fprintf(stderr, "Error writing access unit header position information.\n");
                return false;
            }

            if (isMultipleAlignment(getDatasetHeader(accessUnitHeader->datasetContainer))) {
                bool extendedAuStartPositionSuccessfulWrite;
                bool extendedAuEndPositionSuccessfulWrite;
                if (isPosOffsetUint40(getDatasetHeader(accessUnitHeader->datasetContainer))) {
                    extendedAuStartPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64(
                            &outputBitstream, 40,
                            accessUnitHeader->AU_start_position
                    );
                    extendedAuEndPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64(
                            &outputBitstream, 40,
                            accessUnitHeader->AU_end_position
                    );
                } else {
                    extendedAuStartPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                            &outputBitstream, 32,
                            (uint32_t) accessUnitHeader->extended_AU_start_position
                    );
                    extendedAuEndPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                            &outputBitstream, 32,
                            (uint32_t) accessUnitHeader->extended_AU_end_position
                    );
                }
                if (
                        !extendedAuStartPositionSuccessfulWrite
                        || !extendedAuEndPositionSuccessfulWrite
                        ) {
                    fprintf(stderr, "Error writing access unit header position information.\n");
                    return false;
                }
            }
        } else {
            writeUnalignedHeaderInformation(&outputBitstream, accessUnitHeader);
        }
    }

    //todo add mit information
    if (!writeBuffer(&outputBitstream)){
        fprintf(stderr, "Error writing access unit header bitstream buffer.\n");
        return false;
    }
    return true;
}

bool writeAccessUnitHeader(FILE* outputFile, AccessUnitHeader* accessUnitHeader){
    uint64_t headerSize = getAccessUnitHeaderSize(accessUnitHeader);
    if (!writeBoxHeader(outputFile, accessUnitHeaderName, headerSize)){
        fprintf(stderr, "Error writing access unit header's box header.\n");
        return false;
    };
    return writeAccessUnitHeaderContent(outputFile, accessUnitHeader);
}

AccessUnitHeader* parseAccessUnitHeader(FILE* inputFile, DatasetContainer* datasetContainer){

    long seekPosition = ftell(inputFile);
    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    uint32_t accessUnitIdBuffer;
    uint8_t numBlocksBuffer;
    uint16_t parameterSetIdBuffer;
    uint8_t auTypeBuffer;
    uint32_t readsCountBuffer;
    uint16_t mmThresholdBuffer = 0;
    uint32_t mmCountBuffer = 0;

    bool accessUnitIdSuccessfulRead =
            readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char *) &accessUnitIdBuffer);
    bool numBlocksSuccessfulRead = readBytes(&inputBitstream,1,(char*)&numBlocksBuffer);
    bool parameterSetIdSuccessfulRead =
            readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 12, (char *) &parameterSetIdBuffer);
    bool auTypeSuccessfulRead = readNBitsShift(&inputBitstream, 4, (char*)&auTypeBuffer);
    bool readsCountBufferSuccessfulRead =
            readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char *) &readsCountBuffer);
    if (
        !accessUnitIdSuccessfulRead ||
        !numBlocksSuccessfulRead ||
        !parameterSetIdSuccessfulRead ||
        !auTypeSuccessfulRead ||
        !readsCountBufferSuccessfulRead
    ){
        fprintf(stderr,"Error parsing access unit header.\n");
        return false;
    }

    if(auTypeBuffer == N_TYPE_AU || auTypeBuffer == M_TYPE_AU) {
        bool mmThresholdSuccessfulRead =
                readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 16, (char *) &mmThresholdBuffer);
        bool mmCountSuccessfulRead =
                readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char *) &mmCountBuffer);
        if (!mmThresholdSuccessfulRead || !mmCountSuccessfulRead){
            fprintf(stderr,"Error parsing access unit header.\n");
            return false;
        }
    }

    //if dataset_type == 2
        uint16_t  ref_sequence_id = 0;
        uint64_t ref_start_position = 0;
        uint64_t ref_end_position = 0;

    //if MIT_flag == 0
        //if AU_type != U_TYPE_AU || dataset_type == 2
            uint16_t sequence_ID = 0;
            uint64_t AU_start_position = 0;
            uint64_t AU_end_position = 0;
        //if multiple_alignment_flag
            uint64_t extended_AU_start_position = 0;
            uint64_t extended_AU_end_position = 0;
    //else
        Signatures* signatures = NULL;

    if(getDatasetType(getDatasetHeader(datasetContainer)) == 2){
        bool ref_sequence_id_SuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian16(
                &inputBitstream, 16,
                (char*)&ref_sequence_id
        );
        if(isPosOffsetUint40(getDatasetHeader(datasetContainer))) {
            bool ref_start_position_SuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian64(
                    &inputBitstream, 40,
                    (char *) &ref_start_position
            );
            bool ref_end_position_SuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian64(
                    &inputBitstream, 40,
                    (char *) &ref_end_position
            );
        }else{
            bool ref_start_position_SuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian64(
                    &inputBitstream, 32,
                    (char *) &ref_start_position
            );
            bool ref_end_position_SuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian64(
                    &inputBitstream, 32,
                    (char *) &ref_end_position
            );
        }
    }
    if(!isMITFlagSet(getDatasetHeader(datasetContainer))){
        if(
                auTypeBuffer != U_TYPE_AU
                || getDatasetType(getDatasetHeader(datasetContainer)) == 2
        ){
            bool sequence_ID_successfulRead = readNBitsShiftAndConvertBigToNativeEndian16(
                    &inputBitstream, 16,
                    (char*)&sequence_ID
            );


            bool auStartPositionSuccessfulRead;
            bool auEndPositionSuccessfulRead;

            if(isPosOffsetUint40(getDatasetHeader(datasetContainer))){
                auStartPositionSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian64(
                    &inputBitstream, 40,
                    (char*) &AU_start_position
                );
                auEndPositionSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian64(
                        &inputBitstream, 40,
                        (char*) &AU_end_position
                );
            }else{
                auStartPositionSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian32(
                        &inputBitstream, 32,
                        (char*) &AU_start_position
                );
                auEndPositionSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian32(
                        &inputBitstream, 32,
                        (char*) &AU_end_position
                );
            }

            if(
                !auStartPositionSuccessfulRead
                || !auEndPositionSuccessfulRead
            ){
                fprintf(stderr, "Error reading access unit header.\n");
                return false;
            }


            if(isMultipleAlignment(getDatasetHeader(datasetContainer))){
                bool extendedAuStartPositionSuccessfulRead;
                bool extendedAuEndPositionSuccessfulRead;

                if(isPosOffsetUint40(getDatasetHeader(datasetContainer))){
                    extendedAuStartPositionSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian64(
                            &inputBitstream, 40,
                            (char*) &extended_AU_start_position
                    );
                    extendedAuEndPositionSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian64(
                            &inputBitstream, 40,
                            (char*) &extended_AU_end_position
                    );
                }else{
                    extendedAuStartPositionSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian32(
                            &inputBitstream, 32,
                            (char*) &extended_AU_start_position
                    );
                    extendedAuEndPositionSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian32(
                            &inputBitstream, 32,
                            (char*) &extended_AU_end_position
                    );
                }

                if(
                    !extendedAuStartPositionSuccessfulRead
                    || !extendedAuEndPositionSuccessfulRead
                ){
                    fprintf(stderr, "Error reading access unit header.\n");
                    return false;
                }
            }
        }else{

            uint32_t multipleSignatureBase = getMultipleSignatureBase(getDatasetHeader(datasetContainer));
            uint32_t signatureLengthInBits = getSignatureLength(getDatasetHeader(datasetContainer));
            uint64_t bytesNeeded =  bitsToBytes(signatureLengthInBits);
            uint8_t* signatureBuffer = (uint8_t*) malloc(bytesNeeded*sizeof(uint8_t));

            if (multipleSignatureBase!=0){
                uint16_t actualSignatureNumber = (uint16_t) multipleSignatureBase;
                readNBitsShift(&inputBitstream,signatureLengthInBits,(char*)signatureBuffer);
                bool allBitsToTrue = firstNBitsSetTo((char*)signatureBuffer, signatureLengthInBits, true);


                SignatureArbitraryLengthIntegerInputStream signatureIntegerInputStream;
                if(allBitsToTrue) {
                    readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 16, (char *) &actualSignatureNumber);
                    changeSizeSignatures(
                            signatures,
                            actualSignatureNumber
                    );

                    initSignatureInputStream(&signatureIntegerInputStream,
                                             &inputBitstream, (uint8_t) signatureLengthInBits, 3, 0);
                }else {
                    initSignatureInputStreamAndFirstInteger(&signatureIntegerInputStream,
                                                            &inputBitstream, (uint8_t) signatureLengthInBits, 3, 0, signatureBuffer);
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
                freeSignatureInputStream(&signatureIntegerInputStream);
            }
        }
    }

    AccessUnitHeader* accessUnitHeader = initAccessUnitHeader();
    accessUnitHeader->access_unit_ID = accessUnitIdBuffer;
    accessUnitHeader->num_blocks = numBlocksBuffer;
    accessUnitHeader->parameter_set_ID =parameterSetIdBuffer;
    accessUnitHeader->au_type =auTypeBuffer;
    accessUnitHeader->reads_count =readsCountBuffer;
    accessUnitHeader->mm_threshold =mmThresholdBuffer;
    accessUnitHeader->mm_count =mmCountBuffer;
    accessUnitHeader->seekPosition = seekPosition;
    accessUnitHeader->datasetContainer = datasetContainer;
    accessUnitHeader->ref_sequence_id = ref_sequence_id;
    accessUnitHeader->ref_start_position = ref_start_position;
    accessUnitHeader->sequence_ID = sequence_ID;
    accessUnitHeader->AU_start_position = AU_start_position;
    accessUnitHeader->AU_end_position = AU_end_position;
    accessUnitHeader->extended_AU_start_position = extended_AU_start_position;
    accessUnitHeader->extended_AU_end_position = extended_AU_end_position;
    accessUnitHeader->signatures = signatures;

    return accessUnitHeader;
}

long getAccessUnitHeaderSeekPosition(AccessUnitHeader* accessUnitHeader){
    return accessUnitHeader->seekPosition;
}

uint8_t getNumBlocks(AccessUnitHeader* accessUnitHeader){
    return accessUnitHeader->num_blocks;
}

uint32_t getAccessUnitId(AccessUnitHeader* accessUnitHeader){
    return accessUnitHeader->access_unit_ID;
}

uint16_t getParametersSetId(AccessUnitHeader* accessUnitHeader){
    return accessUnitHeader->parameter_set_ID;
}

uint32_t getReadsCount(AccessUnitHeader* accessUnitHeader){
    return accessUnitHeader->reads_count;
}

uint16_t getMMThreshold(AccessUnitHeader* accessUnitHeader){
    return accessUnitHeader->mm_threshold;
}

uint32_t getMMCount(AccessUnitHeader* accessUnitHeader){
    return accessUnitHeader->mm_count;
}

void setReferenceSequenceId(AccessUnitHeader* accessUnitHeader, uint16_t sequenceId){
    accessUnitHeader->ref_sequence_id = sequenceId;
}

void setReferenceStartPosition(AccessUnitHeader* accessUnitHeader, uint64_t startPosition){
    accessUnitHeader->ref_start_position = startPosition;
}

void setReferenceEndPosition(AccessUnitHeader* accessUnitHeader, uint64_t endPosition){
    accessUnitHeader->ref_end_position = endPosition;
}
