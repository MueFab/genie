//
// Created by gencom on 27/07/17.
//

#include <string.h>
#include <stdlib.h>
#include <Boxes.h>
#include "../../Boxes.h"
#include "../../utils.h"


DatasetsGroupReferenceGenome *initDatasetsGroupReferenceGenomeExternal(char *ref_uri, DatasetGroupId externalDatasetGroupId,
                                                                       DatasetId externalDatasetId, uint8_t checksumAlg, Vector* checksums,
                                                                       ReferenceId referenceId, char *referenceName,
                                                                       ReferenceMajorVersion majorVersion, ReferenceMinorVersion minorVersion,
                                                                       ReferencePatchVersion referencePathVersion) {
    DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome =
            (DatasetsGroupReferenceGenome*)malloc(sizeof(DatasetsGroupReferenceGenome));
    datasetsGroupReferenceGenome->isExternal = true;
    size_t ref_uri_length = strlen(ref_uri);
    datasetsGroupReferenceGenome->externalReference.ref_uri = (char*)malloc(ref_uri_length+1);
    strcpy(datasetsGroupReferenceGenome->externalReference.ref_uri,ref_uri);
    datasetsGroupReferenceGenome->externalReference.ref_uri[ref_uri_length]=0;
    datasetsGroupReferenceGenome->externalReference.externalDatasetGroupId=externalDatasetGroupId;
    datasetsGroupReferenceGenome->externalReference.externalDatasetId= externalDatasetId;
    datasetsGroupReferenceGenome->externalReference.checksumAlgorithm = checksumAlg;
    datasetsGroupReferenceGenome->externalReference.checksums = checksums;

    datasetsGroupReferenceGenome->referenceId = referenceId;
    datasetsGroupReferenceGenome->referenceName = referenceName;
    datasetsGroupReferenceGenome->referenceMajorVersion = majorVersion;
    datasetsGroupReferenceGenome->referenceMinorVersion = minorVersion;
    datasetsGroupReferenceGenome->referencePatchVersion = referencePathVersion;
    datasetsGroupReferenceGenome->seekPosition = -1;

    datasetsGroupReferenceGenome->sequences = initVector();
    return datasetsGroupReferenceGenome;
}

DatasetsGroupReferenceGenome *
initDatasetsGroupReferenceGenome(DatasetGroupId internalDatasetGroupId, DatasetId internalDatasetId,
                                 ReferenceId referenceId, char *referenceName, ReferenceMajorVersion majorVersion,
                                 ReferenceMinorVersion minorVersion, ReferencePatchVersion referencePathVersion) {
    DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome =
            (DatasetsGroupReferenceGenome*)malloc(sizeof(DatasetsGroupReferenceGenome));
    datasetsGroupReferenceGenome->isExternal = false;
    datasetsGroupReferenceGenome->externalReference.ref_uri = NULL;
    datasetsGroupReferenceGenome->internalRefDatasetGroupId = internalDatasetGroupId;
    datasetsGroupReferenceGenome->internalRefDatasetId = internalDatasetId;
    datasetsGroupReferenceGenome->referenceId = referenceId;

    datasetsGroupReferenceGenome->referenceName = referenceName;
    datasetsGroupReferenceGenome->referenceMajorVersion = majorVersion;
    datasetsGroupReferenceGenome->referenceMinorVersion = minorVersion;
    datasetsGroupReferenceGenome->referencePatchVersion = referencePathVersion;
    datasetsGroupReferenceGenome->seekPosition = -1;

    datasetsGroupReferenceGenome->sequences = initVector();
    return datasetsGroupReferenceGenome;
}

void freeDatasetsGroupReferenceGenome(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome){
    uint32_t seq_count = (uint32_t) getSize(datasetsGroupReferenceGenome->sequences);

    if(datasetsGroupReferenceGenome->isExternal){
        free(datasetsGroupReferenceGenome->externalReference.ref_uri);
        for(size_t seq_i = 0; seq_i < seq_count; seq_i++){
            free(getValue(datasetsGroupReferenceGenome->externalReference.checksums, seq_i));
        }
        freeVector(datasetsGroupReferenceGenome->externalReference.checksums);
    }


    for(size_t seq_i = 0; seq_i < seq_count; seq_i++){
        char* sequence_name = getValue(datasetsGroupReferenceGenome->sequences, seq_i);
        free(sequence_name);
    }

    freeVector(datasetsGroupReferenceGenome->sequences);
    free(datasetsGroupReferenceGenome->referenceName);

    free(datasetsGroupReferenceGenome);
}

bool writeDatasetsGroupReferenceGenomeContent(
        DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome,
        FILE *outputFile
){
    bool referenceIdSuccessfulWrite = writeUint8(datasetsGroupReferenceGenome->referenceId, outputFile);

    size_t referenceNameSize = strlen(datasetsGroupReferenceGenome->referenceName);
    bool referenceNameWriteSuccessfulWrite =
            fwrite(
                    datasetsGroupReferenceGenome->referenceName,
                    sizeof(char),referenceNameSize+1,
                    outputFile
            )==referenceNameSize+1;

    bool referenceMajorVersionSuccessfulWrite =
            writeBigEndian16ToFile(datasetsGroupReferenceGenome->referenceMajorVersion, outputFile);
    bool referenceMinorVersionSuccessfulWrite =
            writeBigEndian16ToFile(datasetsGroupReferenceGenome->referenceMinorVersion, outputFile);
    bool referencePatchVersionSuccessfulWrite =
            writeBigEndian16ToFile(datasetsGroupReferenceGenome->referencePatchVersion, outputFile);

    if (!referenceIdSuccessfulWrite ||
        !referenceNameWriteSuccessfulWrite ||
        !referenceMajorVersionSuccessfulWrite ||
        !referenceMinorVersionSuccessfulWrite ||
        !referencePatchVersionSuccessfulWrite){
        fprintf(stderr,"Error printing reference information.\n");
        return false;
    }

    uint16_t seq_count = (uint16_t) getSize(datasetsGroupReferenceGenome->sequences);
    bool seqCountSuccessfulWrite = writeBigEndian16ToFile(seq_count, outputFile);
    if(!seqCountSuccessfulWrite){
        fprintf(stderr,"Error printing seq count.\n");
    }

    for(size_t seq_i = 0; seq_i < seq_count; seq_i++){
        char* sequence_name = getValue(datasetsGroupReferenceGenome->sequences, seq_i);
        size_t sequence_name_length = strlen(sequence_name);
        bool sequence_name_write_success =
                fwrite(sequence_name, sizeof(char), sequence_name_length+1, outputFile) == (sequence_name_length+1);
        if (!sequence_name_write_success){
            fprintf(stderr,"Error writing sequence name.\n");
            return false;
        }
    }




    uint8_t isExternalFlagValue = datasetsGroupReferenceGenome->isExternal ? (uint8_t)1:(uint8_t)0;
    if (!writeUint8(isExternalFlagValue,outputFile)){
        fprintf(stderr,"Error external uri.\n");
        return false;
    }

    if(datasetsGroupReferenceGenome->isExternal){
        size_t refUriLength = strlen(datasetsGroupReferenceGenome->externalReference.ref_uri)+1;
        size_t refUriWriteLength =
                fwrite(datasetsGroupReferenceGenome->externalReference.ref_uri,sizeof(char),refUriLength,outputFile);
        if(refUriWriteLength!=refUriLength){
            fprintf(stderr,"Error external uri.\n");
            return false;
        }
        bool externalDatasetGroupIdSuccessfulWrite =
                writeUint8(datasetsGroupReferenceGenome->externalReference.externalDatasetGroupId,
                                          outputFile);
        bool externalDatasetIdSuccessfulWrite =
                writeBigEndian16ToFile(datasetsGroupReferenceGenome->externalReference.externalDatasetId, outputFile);

        bool checksumAlgorithmSuccessfulWrite =
                writeUint8(datasetsGroupReferenceGenome->externalReference.checksumAlgorithm, outputFile);
        size_t checksumSize = datasetsGroupReferenceGenome->externalReference.checksumAlgorithm? 32:16;
        for(size_t seq_i = 0; seq_i < seq_count; seq_i++) {
            //todo add error check
            size_t externalChecksumWriteSize =
                    fwrite(
                            getValue(datasetsGroupReferenceGenome->externalReference.checksums,seq_i),
                            sizeof(char),
                            checksumSize,
                            outputFile
                    );
        }
        if(!externalDatasetGroupIdSuccessfulWrite ||
                !externalDatasetIdSuccessfulWrite ||
                !checksumAlgorithmSuccessfulWrite){
            fprintf(stderr,"Error printing external dataset group id, dataset id or checksum.\n");
            return false;
        }
    }else{
        bool datasetGroupIdSuccessfulWrite = writeUint8(datasetsGroupReferenceGenome->internalRefDatasetGroupId, outputFile)==1;
        bool datasetIdSuccessfulWrite =
                writeBigEndian16ToFile(datasetsGroupReferenceGenome->internalRefDatasetId, outputFile);
        if(!datasetGroupIdSuccessfulWrite || !datasetIdSuccessfulWrite){
            fprintf(stderr,"Error printing internal dataset group or dataset reference.\n");
            return false;
        }
    }



    return true;
}

bool writeDatasetsGroupReferenceGenome(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome, FILE *outputFile){
    uint64_t datasetsGroupReferencesSize = getDatasetsGroupReferenceGenomeSize(datasetsGroupReferenceGenome);
    if (!writeBoxHeader(outputFile, datasetsGroupReferencesName, datasetsGroupReferencesSize)){
        fprintf(stderr, "Error writing type or size of datasets group reference genome.\n");
        return false;
    }
    return writeDatasetsGroupReferenceGenomeContent(datasetsGroupReferenceGenome, outputFile);
}


uint64_t getDatasetsGroupReferenceGenomeContentSize(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome){
    uint64_t boxContentSize = 0;

    boxContentSize += sizeof(datasetsGroupReferenceGenome->referenceId);
    boxContentSize += strlen(datasetsGroupReferenceGenome->referenceName)+1;
    boxContentSize += sizeof(datasetsGroupReferenceGenome->referenceMajorVersion);
    boxContentSize += sizeof(datasetsGroupReferenceGenome->referenceMinorVersion);
    boxContentSize += sizeof(datasetsGroupReferenceGenome->referencePatchVersion);

    uint16_t seq_count = (uint16_t) getSize(datasetsGroupReferenceGenome->sequences);
    boxContentSize += sizeof(seq_count);

    for(uint32_t seq_i=0; seq_i<seq_count; seq_i++){
        boxContentSize += strlen(getValue(datasetsGroupReferenceGenome->sequences, seq_i))+1;
    }

    boxContentSize += sizeof(uint8_t); //For flag
    if (datasetsGroupReferenceGenome->isExternal){
        boxContentSize += strlen(datasetsGroupReferenceGenome->externalReference.ref_uri)+1;
        boxContentSize += sizeof(datasetsGroupReferenceGenome->externalReference.externalDatasetGroupId);
        boxContentSize += sizeof(datasetsGroupReferenceGenome->externalReference.externalDatasetId);
        boxContentSize += sizeof(datasetsGroupReferenceGenome->externalReference.checksumAlgorithm);
        boxContentSize += (datasetsGroupReferenceGenome->externalReference.checksumAlgorithm ? 32:16)*seq_count;
    }else{
        boxContentSize += sizeof(datasetsGroupReferenceGenome->internalRefDatasetGroupId);
        boxContentSize += sizeof(datasetsGroupReferenceGenome->internalRefDatasetId);
    }
    return boxContentSize;
}

uint64_t getDatasetsGroupReferenceGenomeSize(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome){
    return BOX_HEADER_SIZE+getDatasetsGroupReferenceGenomeContentSize(datasetsGroupReferenceGenome);
}

DatasetsGroupReferenceGenome *parseDatasetsGroupReferenceGenome(FILE *inputFile) {
    long seekPosition = ftell(inputFile);

    ReferenceId referenceIdBuffer = 0;
    char* referenceNameBuffer = NULL;
    size_t referenceNameBufferSize = 0;
    uint16_t referenceMajorVersion = 0;
    uint16_t referenceMinorVersion = 0;
    uint16_t referencePatchVersion = 0;

    bool referenceIdSuccessfulRead = readUint8(&referenceIdBuffer,inputFile);
    bool referenceNameSuccessfulRead = getdelim(&referenceNameBuffer,&referenceNameBufferSize,'\0',inputFile)!=-1;

    if (!referenceIdSuccessfulRead || !referenceNameSuccessfulRead) {
        fprintf(stderr, "Error reading reference Id or reference Name in reference genome.\n");
        free(referenceNameBuffer);
        return NULL;
    }

    bool referenceMajorVersionSuccessfulRead = readBigEndian16FromFile(&referenceMajorVersion, inputFile);
    bool referenceMinorVersionSuccessfulRead = readBigEndian16FromFile(&referenceMinorVersion, inputFile);
    bool referencePatchVersionSuccessfulRead = readBigEndian16FromFile(&referencePatchVersion, inputFile);

    if(
        !referenceMajorVersionSuccessfulRead ||
        !referenceMinorVersionSuccessfulRead ||
        !referencePatchVersionSuccessfulRead
    ){
        fprintf(stderr, "Error reading versions.\n");
        free(referenceNameBuffer);
        return NULL;
    }

    uint16_t seqCountBuffer = 0;
    if(!readBigEndian16FromFile(&seqCountBuffer, inputFile)) {
        fprintf(stderr,"Error reading sequence count.\n");
        free(referenceNameBuffer);
        return NULL;
    }

    Vector* sequenceNames = initVector();
    if(!resizeVector(sequenceNames, seqCountBuffer)){
        fprintf(stderr,"Error reallocating sequence names buffer.\n");
        free(referenceNameBuffer);
        freeVector(sequenceNames);
        return NULL;
    }


    bool errorSequenceNames = false;
    for(uint32_t seq_i=0; seq_i<seqCountBuffer; seq_i++) {
        char* sequenceNameBuffer = NULL;
        size_t sequenceBufferSize = 0;
        bool sequenceNameSuccessfulRead = getdelim(&sequenceNameBuffer,&sequenceBufferSize,'\0',inputFile)!=-1;

        if(!sequenceNameSuccessfulRead) {
            errorSequenceNames = true;
            break;
        }
        setValue(sequenceNames, seq_i, sequenceNameBuffer);
    }

    if (errorSequenceNames) {
        fprintf(stderr, "Error reading sequence names.\n");
        for(uint32_t seq_i=0; seq_i<seqCountBuffer; seq_i++) {
            void* value = getValue(sequenceNames, seq_i);
            free(value);
        }
        freeVector(sequenceNames);
        free(referenceNameBuffer);
        return NULL;
    }

    uint8_t isExternalFlagValue;
    if(!readUint8(&isExternalFlagValue, inputFile)){
        fprintf(stderr,"Error reading reference genome external flag.\n");
        for(uint32_t seq_i=0; seq_i<seqCountBuffer; seq_i++) {
            void* value = getValue(sequenceNames, seq_i);
            free(value);
        }
        freeVector(sequenceNames);
        free(referenceNameBuffer);
        return NULL;
    };


    if(isExternalFlagValue==1){
        char* refUriBuffer = NULL;
        size_t refUriBufferSize = 0;
        if (getdelim(&refUriBuffer,&refUriBufferSize,'\0',inputFile)==-1) {
            for(uint32_t seq_i=0; seq_i<seqCountBuffer; seq_i++) {
                void* value = getValue(sequenceNames, seq_i);
                free(value);
            }
            freeVector(sequenceNames);
            free(referenceNameBuffer);
            free(refUriBuffer);
            fprintf(stderr,"Error reading reference uri.\n");
            return NULL;
        };
        DatasetGroupId datasetGroupId;
        bool datasetGroupIdSuccessfulRead = readUint8(&datasetGroupId, inputFile);
        DatasetId datasetId;
        bool datasetIdSuccessfulRead = readBigEndian16FromFile(&datasetId, inputFile);
        uint8_t checksumAlgorithmBuffer;
        bool checksumAlgorithmSuccessfulRead = readUint8(&checksumAlgorithmBuffer, inputFile);

        Vector* checksums = initVector();
        for(uint32_t seq_i=0; seq_i<seqCountBuffer; seq_i++) {
            //todo add error handling
            size_t checksumSize = checksumAlgorithmBuffer ? 32:16;
            uint8_t * checksumBuffer = (uint8_t *)calloc(sizeof(uint8_t),checksumSize);
            bool checksumSuccessfulRead = readChars((char*)checksumBuffer,(uint32_t) checksumSize,inputFile);
            setValue(checksums, seq_i, checksumBuffer);
        }

        if (!datasetGroupIdSuccessfulRead ||
                !datasetIdSuccessfulRead ||
                !checksumAlgorithmSuccessfulRead
        ){
            for(uint32_t seq_i=0; seq_i<seqCountBuffer; seq_i++) {
                void* value = getValue(sequenceNames, seq_i);
                free(value);
            }
            freeVector(sequenceNames);
            free(referenceNameBuffer);
            free(refUriBuffer);
            fprintf(stderr,"Error reading reference uri.\n");
            return NULL;
        }


        DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome = initDatasetsGroupReferenceGenomeExternal(
                refUriBuffer, datasetGroupId, datasetId, checksumAlgorithmBuffer, checksums, referenceIdBuffer, referenceNameBuffer,
                referenceMajorVersion, referenceMinorVersion, referencePatchVersion);
        setSequenceNamesVector(datasetsGroupReferenceGenome, sequenceNames);
        freeVector(sequenceNames);
        free(refUriBuffer);
        datasetsGroupReferenceGenome->seekPosition = seekPosition;
        return datasetsGroupReferenceGenome;
    }

    DatasetGroupId internalDatasetGroupId;
    DatasetId internalDatasetId;
    bool internalDatasetGroupIdSuccessfulRead = readUint8(&internalDatasetGroupId, inputFile);
    bool internalDatasetIdSuccessfulRead = readBigEndian16FromFile(&internalDatasetId, inputFile);
    if (!internalDatasetGroupIdSuccessfulRead || !internalDatasetIdSuccessfulRead) {
        fprintf(stderr,"Error reading internal reference.\n");
        return NULL;
    }

    DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome =
            initDatasetsGroupReferenceGenome(internalDatasetGroupId, internalDatasetId, referenceIdBuffer,
                                             referenceNameBuffer, referenceMajorVersion, referenceMinorVersion,
                                             referencePatchVersion);
    for(uint32_t seq_i=0; seq_i<seqCountBuffer; seq_i++) {
        char* sequenceNameBuffer = getValue(sequenceNames, seq_i);
        setSequenceName(datasetsGroupReferenceGenome, seq_i, sequenceNameBuffer);
    }
    freeVector(sequenceNames);
    datasetsGroupReferenceGenome->seekPosition = seekPosition;
    return datasetsGroupReferenceGenome;
}

long getDatasetGroupReferenceGenomeSeekPosition(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome){
    return datasetsGroupReferenceGenome->seekPosition;
}

bool resizeSequences(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome, size_t newSize){
    return resizeVector(datasetsGroupReferenceGenome->sequences, newSize);
}

bool setSequenceName(DatasetsGroupReferenceGenome *datasetsGroupReferenceGenome, size_t sequence_i, char *name) {
    setValue(datasetsGroupReferenceGenome->sequences, sequence_i, name);
}

bool setSequenceNamesVector(DatasetsGroupReferenceGenome *datasetsGroupReferenceGenome, Vector *namesToCopy) {
    if(datasetsGroupReferenceGenome->sequences != NULL){
        uint32_t seq_count = (uint32_t) getSize(datasetsGroupReferenceGenome->sequences);

        for(size_t seq_i = 0; seq_i < seq_count; seq_i++){
            char* sequence_name = getValue(datasetsGroupReferenceGenome->sequences, seq_i);
            free(sequence_name);
        }

        freeVector(datasetsGroupReferenceGenome->sequences);
    }
    datasetsGroupReferenceGenome->sequences = shallowCopy(namesToCopy);
    return datasetsGroupReferenceGenome!=NULL;
}
