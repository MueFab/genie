//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include <string.h>
#include "../../Boxes.h"
#include "../../FastaReader.h"
#include "../../utils.h"

bool writeDatasetsGroupReferenceGenomeContent(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome,
                                              FILE* outputFile);

DatasetsGroupReferenceGenome* initDatasetsGroupReferenceGenomeExternal(
    uint8_t dataset_group_id, char* ref_uri, uint8_t referenceType, DatasetGroupId externalDatasetGroupId,
    DatasetId externalDatasetId, uint8_t checksumAlg, Vector* checksums, ReferenceId referenceId, char* referenceName,
    ReferenceMajorVersion majorVersion, ReferenceMinorVersion minorVersion,
    ReferencePatchVersion referencePathVersion) {
    DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome =
        (DatasetsGroupReferenceGenome*)malloc(sizeof(DatasetsGroupReferenceGenome));
    datasetsGroupReferenceGenome->dataset_group_id = dataset_group_id;

    datasetsGroupReferenceGenome->isExternal = true;
    size_t ref_uri_length = strlen(ref_uri);
    datasetsGroupReferenceGenome->externalReference.ref_uri = (char*)malloc(ref_uri_length + 1);
    strcpy(datasetsGroupReferenceGenome->externalReference.ref_uri, ref_uri);
    datasetsGroupReferenceGenome->externalReference.ref_uri[ref_uri_length] = 0;
    datasetsGroupReferenceGenome->externalReference.reference_type = referenceType;
    datasetsGroupReferenceGenome->externalReference.externalDatasetGroupId = externalDatasetGroupId;
    datasetsGroupReferenceGenome->externalReference.externalDatasetId = externalDatasetId;
    datasetsGroupReferenceGenome->externalReference.checksumAlgorithm = checksumAlg;
    datasetsGroupReferenceGenome->externalReference.checksums = checksums;

    datasetsGroupReferenceGenome->referenceId = referenceId;
    size_t ref_name_length = strlen(referenceName);
    datasetsGroupReferenceGenome->referenceName = (char*)malloc(ref_name_length + 1);
    strcpy(datasetsGroupReferenceGenome->referenceName, referenceName);

    datasetsGroupReferenceGenome->referenceMajorVersion = majorVersion;
    datasetsGroupReferenceGenome->referenceMinorVersion = minorVersion;
    datasetsGroupReferenceGenome->referencePatchVersion = referencePathVersion;
    datasetsGroupReferenceGenome->hasSeek = false;
    datasetsGroupReferenceGenome->seekPosition = 0;

    datasetsGroupReferenceGenome->sequences = initVector();
    return datasetsGroupReferenceGenome;
}

DatasetsGroupReferenceGenome* initDatasetsGroupReferenceGenome(uint8_t dataset_group_id,
                                                               DatasetGroupId internalDatasetGroupId,
                                                               DatasetId internalDatasetId, ReferenceId referenceId,
                                                               char* referenceName, ReferenceMajorVersion majorVersion,
                                                               ReferenceMinorVersion minorVersion,
                                                               ReferencePatchVersion referencePathVersion) {
    DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome =
        (DatasetsGroupReferenceGenome*)malloc(sizeof(DatasetsGroupReferenceGenome));

    datasetsGroupReferenceGenome->dataset_group_id = dataset_group_id;

    datasetsGroupReferenceGenome->isExternal = false;
    datasetsGroupReferenceGenome->externalReference.ref_uri = NULL;
    datasetsGroupReferenceGenome->internalRefDatasetGroupId = internalDatasetGroupId;
    datasetsGroupReferenceGenome->internalRefDatasetId = internalDatasetId;
    datasetsGroupReferenceGenome->referenceId = referenceId;

    size_t ref_name_length = strlen(referenceName);
    datasetsGroupReferenceGenome->referenceName = (char*)malloc(ref_name_length + 1);
    strcpy(datasetsGroupReferenceGenome->referenceName, referenceName);
    datasetsGroupReferenceGenome->referenceMajorVersion = majorVersion;
    datasetsGroupReferenceGenome->referenceMinorVersion = minorVersion;
    datasetsGroupReferenceGenome->referencePatchVersion = referencePathVersion;
    datasetsGroupReferenceGenome->hasSeek = false;
    datasetsGroupReferenceGenome->seekPosition = 0;

    datasetsGroupReferenceGenome->sequences = initVector();
    return datasetsGroupReferenceGenome;
}

void freeDatasetsGroupReferenceGenome(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome) {
    uint32_t seq_count = (uint32_t)getSize(datasetsGroupReferenceGenome->sequences);

    if (datasetsGroupReferenceGenome->isExternal) {
        free(datasetsGroupReferenceGenome->externalReference.ref_uri);
        for (size_t seq_i = 0; seq_i < seq_count; seq_i++) {
            free(getValue(datasetsGroupReferenceGenome->externalReference.checksums, seq_i));
        }
        freeVector(datasetsGroupReferenceGenome->externalReference.checksums);
    }

    for (size_t seq_i = 0; seq_i < seq_count; seq_i++) {
        char* sequence_name = getValue(datasetsGroupReferenceGenome->sequences, seq_i);
        free(sequence_name);
    }

    freeVector(datasetsGroupReferenceGenome->sequences);
    free(datasetsGroupReferenceGenome->referenceName);

    free(datasetsGroupReferenceGenome);
}

bool writeDatasetsGroupReferenceGenomeContent(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome,
                                              FILE* outputFile) {
    bool datasetGroupIdSuccessfulWrite = utils_write(datasetsGroupReferenceGenome->dataset_group_id, outputFile);
    bool referenceIdSuccessfulWrite = utils_write(datasetsGroupReferenceGenome->referenceId, outputFile);

    size_t referenceNameSize = strlen(datasetsGroupReferenceGenome->referenceName);
    bool referenceNameWriteSuccessfulWrite = fwrite(datasetsGroupReferenceGenome->referenceName, sizeof(char),
                                                    referenceNameSize + 1, outputFile) == referenceNameSize + 1;

    bool referenceMajorVersionSuccessfulWrite =
        writeBigEndian16ToFile(datasetsGroupReferenceGenome->referenceMajorVersion, outputFile);
    bool referenceMinorVersionSuccessfulWrite =
        writeBigEndian16ToFile(datasetsGroupReferenceGenome->referenceMinorVersion, outputFile);
    bool referencePatchVersionSuccessfulWrite =
        writeBigEndian16ToFile(datasetsGroupReferenceGenome->referencePatchVersion, outputFile);

    if (!datasetGroupIdSuccessfulWrite || !referenceIdSuccessfulWrite || !referenceNameWriteSuccessfulWrite ||
        !referenceMajorVersionSuccessfulWrite || !referenceMinorVersionSuccessfulWrite ||
        !referencePatchVersionSuccessfulWrite) {
        fprintf(stderr, "Error printing reference information.\n");
        return false;
    }

    uint16_t seq_count = (uint16_t)getSize(datasetsGroupReferenceGenome->sequences);
    bool seqCountSuccessfulWrite = writeBigEndian16ToFile(seq_count, outputFile);
    if (!seqCountSuccessfulWrite) {
        fprintf(stderr, "Error printing seq count.\n");
    }

    for (size_t seq_i = 0; seq_i < seq_count; seq_i++) {
        char* sequence_name = getValue(datasetsGroupReferenceGenome->sequences, seq_i);
        size_t sequence_name_length = strlen(sequence_name);
        bool sequence_name_write_success =
            fwrite(sequence_name, sizeof(char), sequence_name_length + 1, outputFile) == (sequence_name_length + 1);
        if (!sequence_name_write_success) {
            fprintf(stderr, "Error writing sequence name.\n");
            return false;
        }
    }

    uint8_t isExternalFlagValue = datasetsGroupReferenceGenome->isExternal ? (uint8_t)1 : (uint8_t)0;
    if (!utils_write(isExternalFlagValue, outputFile)) {
        fprintf(stderr, "Error external uri.\n");
        return false;
    }

    if (datasetsGroupReferenceGenome->isExternal) {
        size_t refUriLength = strlen(datasetsGroupReferenceGenome->externalReference.ref_uri) + 1;
        size_t refUriWriteLength =
            fwrite(datasetsGroupReferenceGenome->externalReference.ref_uri, sizeof(char), refUriLength, outputFile);
        if (refUriWriteLength != refUriLength) {
            fprintf(stderr, "Error external uri.\n");
            return false;
        }

        bool checksumAlgorithmSuccessfulWrite =
            utils_write(datasetsGroupReferenceGenome->externalReference.checksumAlgorithm, outputFile);
        if (!checksumAlgorithmSuccessfulWrite) {
            fprintf(stderr, "Error printing external dataset group id, dataset id or checksum.\n");
            return false;
        }

        bool externalReferenceTypeSuccessfulWrite =
            utils_write(datasetsGroupReferenceGenome->externalReference.reference_type, outputFile);
        if (!externalReferenceTypeSuccessfulWrite) {
            fprintf(stderr, "Error writing reference type.\n");
            return false;
        }

        if (datasetsGroupReferenceGenome->externalReference.reference_type == REFERENCE_TYPE_MPEGG_REF) {
            bool externalDatasetGroupIdSuccessfulWrite =
                utils_write(datasetsGroupReferenceGenome->externalReference.externalDatasetGroupId, outputFile);
            bool externalDatasetIdSuccessfulWrite =
                writeBigEndian16ToFile(datasetsGroupReferenceGenome->externalReference.externalDatasetId, outputFile);
            if (!externalDatasetGroupIdSuccessfulWrite || !externalDatasetIdSuccessfulWrite) {
                fprintf(stderr, "Error writing external ids of reference.");
                return false;
            }
        }

        size_t checksumSize = datasetsGroupReferenceGenome->externalReference.checksumAlgorithm ? 32 : 16;
        if (datasetsGroupReferenceGenome->externalReference.reference_type == REFERENCE_TYPE_MPEGG_REF) {
            size_t externalChecksumWriteSize =
                fwrite(getValue(datasetsGroupReferenceGenome->externalReference.checksums, 0), sizeof(char),
                       checksumSize, outputFile);
        }

        size_t numberChecksumsToWrite = 1;
        if (datasetsGroupReferenceGenome->externalReference.reference_type != REFERENCE_TYPE_MPEGG_REF) {
            numberChecksumsToWrite = seq_count;
        }

        for (size_t seq_i = 0; seq_i < numberChecksumsToWrite; seq_i++) {
            size_t externalChecksumWriteSize =
                fwrite(getValue(datasetsGroupReferenceGenome->externalReference.checksums, seq_i), sizeof(char),
                       checksumSize, outputFile);
        }

    } else {
        bool internalRefDatasetGroupIdSuccessfulWrite =
            utils_write(datasetsGroupReferenceGenome->internalRefDatasetGroupId, outputFile) == 1;
        bool internalRefDatasetIdSuccessfulWrite =
            writeBigEndian16ToFile(datasetsGroupReferenceGenome->internalRefDatasetId, outputFile);
        if (!internalRefDatasetGroupIdSuccessfulWrite || !internalRefDatasetIdSuccessfulWrite) {
            fprintf(stderr, "Error printing internal dataset group or dataset reference.\n");
            return false;
        }
    }

    return true;
}

bool writeDatasetsGroupReferenceGenome(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome, FILE* outputFile) {
    uint64_t datasetsGroupReferencesSize = getDatasetsGroupReferenceGenomeSize(datasetsGroupReferenceGenome);
    if (!writeBoxHeader(outputFile, datasetsGroupReferencesName, datasetsGroupReferencesSize)) {
        fprintf(stderr, "Error writing type or size of datasets group reference genome.\n");
        return false;
    }
    return writeDatasetsGroupReferenceGenomeContent(datasetsGroupReferenceGenome, outputFile);
}

uint64_t getDatasetsGroupReferenceGenomeContentSize(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome) {
    uint64_t boxContentSize = 0;

    boxContentSize += sizeof(datasetsGroupReferenceGenome->dataset_group_id);
    boxContentSize += sizeof(datasetsGroupReferenceGenome->referenceId);
    boxContentSize += strlen(datasetsGroupReferenceGenome->referenceName) + 1;
    boxContentSize += sizeof(datasetsGroupReferenceGenome->referenceMajorVersion);
    boxContentSize += sizeof(datasetsGroupReferenceGenome->referenceMinorVersion);
    boxContentSize += sizeof(datasetsGroupReferenceGenome->referencePatchVersion);

    uint16_t seq_count = (uint16_t)getSize(datasetsGroupReferenceGenome->sequences);
    boxContentSize += sizeof(seq_count);

    for (uint32_t seq_i = 0; seq_i < seq_count; seq_i++) {
        boxContentSize += strlen(getValue(datasetsGroupReferenceGenome->sequences, seq_i)) + 1;
    }

    boxContentSize += sizeof(uint8_t);  // for reserved + external_ref_flag
    if (datasetsGroupReferenceGenome->isExternal) {
        boxContentSize += strlen(datasetsGroupReferenceGenome->externalReference.ref_uri) + 1;
        boxContentSize += sizeof(datasetsGroupReferenceGenome->externalReference.reference_type);
        boxContentSize += sizeof(datasetsGroupReferenceGenome->externalReference.checksumAlgorithm);
        if (datasetsGroupReferenceGenome->externalReference.reference_type == REFERENCE_TYPE_MPEGG_REF) {
            boxContentSize += sizeof(datasetsGroupReferenceGenome->externalReference.externalDatasetGroupId);
            boxContentSize += sizeof(datasetsGroupReferenceGenome->externalReference.externalDatasetId);
        }
        if (datasetsGroupReferenceGenome->externalReference.reference_type != REFERENCE_TYPE_MPEGG_REF) {
            boxContentSize +=
                (uint64_t)((datasetsGroupReferenceGenome->externalReference.checksumAlgorithm ? 32 : 16) * seq_count);
        } else {
            boxContentSize += (uint64_t)(datasetsGroupReferenceGenome->externalReference.checksumAlgorithm ? 32 : 16);
        }
    } else {
        boxContentSize += sizeof(datasetsGroupReferenceGenome->internalRefDatasetGroupId);
        boxContentSize += sizeof(datasetsGroupReferenceGenome->internalRefDatasetId);
    }
    return boxContentSize;
}

uint64_t getDatasetsGroupReferenceGenomeSize(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome) {
    return BOX_HEADER_SIZE + getDatasetsGroupReferenceGenomeContentSize(datasetsGroupReferenceGenome);
}

DatasetsGroupReferenceGenome* parseDatasetsGroupReferenceGenome(FILE* inputFile) {
    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not get file position.\n");
        return NULL;
    }

    uint8_t dataset_group_id = 0;
    ReferenceId referenceIdBuffer = 0;
    char* referenceNameBuffer = NULL;
    size_t referenceNameBufferSize = 0;
    uint16_t referenceMajorVersion = 0;
    uint16_t referenceMinorVersion = 0;
    uint16_t referencePatchVersion = 0;

    bool datasetGroupIdSuccessfulRead = utils_read(&dataset_group_id, inputFile);
    bool referenceIdSuccessfulRead = utils_read(&referenceIdBuffer, inputFile);
    bool referenceNameSuccessfulRead = getdelim(&referenceNameBuffer, &referenceNameBufferSize, '\0', inputFile) != -1;

    if (!datasetGroupIdSuccessfulRead || !referenceIdSuccessfulRead || !referenceNameSuccessfulRead) {
        fprintf(stderr, "Error reading dataset group id, reference Id or reference Name in reference genome.\n");
        free(referenceNameBuffer);
        return NULL;
    }

    bool referenceMajorVersionSuccessfulRead = readBigEndian16FromFile(&referenceMajorVersion, inputFile);
    bool referenceMinorVersionSuccessfulRead = readBigEndian16FromFile(&referenceMinorVersion, inputFile);
    bool referencePatchVersionSuccessfulRead = readBigEndian16FromFile(&referencePatchVersion, inputFile);

    if (!referenceMajorVersionSuccessfulRead || !referenceMinorVersionSuccessfulRead ||
        !referencePatchVersionSuccessfulRead) {
        fprintf(stderr, "Error reading versions.\n");
        free(referenceNameBuffer);
        return NULL;
    }

    uint16_t seqCountBuffer = 0;
    if (!readBigEndian16FromFile(&seqCountBuffer, inputFile)) {
        fprintf(stderr, "Error reading sequence count.\n");
        free(referenceNameBuffer);
        return NULL;
    }

    Vector* sequenceNames = initVector();
    if (!resizeVector(sequenceNames, seqCountBuffer)) {
        fprintf(stderr, "Error reallocating sequence names buffer.\n");
        free(referenceNameBuffer);
        freeVector(sequenceNames);
        return NULL;
    }

    bool errorSequenceNames = false;
    for (uint32_t seq_i = 0; seq_i < seqCountBuffer; seq_i++) {
        char* sequenceNameBuffer = NULL;
        size_t sequenceBufferSize = 0;
        bool sequenceNameSuccessfulRead = getdelim(&sequenceNameBuffer, &sequenceBufferSize, '\0', inputFile) != -1;

        if (!sequenceNameSuccessfulRead) {
            errorSequenceNames = true;
            break;
        }
        setValue(sequenceNames, seq_i, sequenceNameBuffer);
    }

    if (errorSequenceNames) {
        fprintf(stderr, "Error reading sequence names.\n");
        for (uint32_t seq_i = 0; seq_i < seqCountBuffer; seq_i++) {
            void* value = getValue(sequenceNames, seq_i);
            free(value);
        }
        freeVector(sequenceNames);
        free(referenceNameBuffer);
        return NULL;
    }

    uint8_t isExternalFlagValue;
    if (!utils_read(&isExternalFlagValue, inputFile)) {
        fprintf(stderr, "Error reading reference genome external flag.\n");
        for (uint32_t seq_i = 0; seq_i < seqCountBuffer; seq_i++) {
            void* value = getValue(sequenceNames, seq_i);
            free(value);
        }
        freeVector(sequenceNames);
        free(referenceNameBuffer);
        return NULL;
    };

    if (isExternalFlagValue == 1) {
        char* refUriBuffer = NULL;
        size_t refUriBufferSize = 0;
        if (getdelim(&refUriBuffer, &refUriBufferSize, '\0', inputFile) == -1) {
            for (uint32_t seq_i = 0; seq_i < seqCountBuffer; seq_i++) {
                void* value = getValue(sequenceNames, seq_i);
                free(value);
            }
            freeVector(sequenceNames);
            free(referenceNameBuffer);
            free(refUriBuffer);
            fprintf(stderr, "Error reading reference uri.\n");
            return NULL;
        };

        uint8_t checksum_alg;
        if (!utils_read(&checksum_alg, inputFile)) {
            freeVector(sequenceNames);
            free(referenceNameBuffer);
            free(refUriBuffer);
            fprintf(stderr, "Error reading checksum algorithm.\n");
            return NULL;
        }

        uint8_t reference_type;
        if (!utils_read(&reference_type, inputFile)) {
            for (uint32_t seq_i = 0; seq_i < seqCountBuffer; seq_i++) {
                void* value = getValue(sequenceNames, seq_i);
                free(value);
            }
            freeVector(sequenceNames);
            free(referenceNameBuffer);
            free(refUriBuffer);
            fprintf(stderr, "Error reading reference type.\n");
            return NULL;
        }

        DatasetGroupId datasetGroupId = 0;
        DatasetId datasetId = 0;

        if (reference_type == REFERENCE_TYPE_MPEGG_REF) {
            bool externalDatasetGroupIdSuccessfulRead = utils_read(&datasetGroupId, inputFile);
            bool externalDatasetIdSuccessfulRead = readBigEndian16FromFile(&datasetId, inputFile);

            if (!externalDatasetGroupIdSuccessfulRead || !externalDatasetIdSuccessfulRead) {
                for (uint32_t seq_i = 0; seq_i < seqCountBuffer; seq_i++) {
                    void* value = getValue(sequenceNames, seq_i);
                    free(value);
                }
                freeVector(sequenceNames);
                free(referenceNameBuffer);
                free(refUriBuffer);
                fprintf(stderr, "Error reading dataset group id or dataset id.\n");
                return NULL;
            }
        }

        size_t numberSequencesWithChecksum = 1;
        if (reference_type != REFERENCE_TYPE_MPEGG_REF) {
            numberSequencesWithChecksum = seqCountBuffer;
        }

        Vector* checksums = initVector();

        for (uint32_t seq_i = 0; seq_i < numberSequencesWithChecksum; seq_i++) {
            // todo add error handling
            size_t checksumSize = checksum_alg ? 32 : 16;
            uint8_t* checksumBuffer = (uint8_t*)calloc(sizeof(uint8_t), checksumSize);
            bool checksumSuccessfulRead = readChars((char*)checksumBuffer, (uint32_t)checksumSize, inputFile);
            setValue(checksums, seq_i, checksumBuffer);
        }

        DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome = initDatasetsGroupReferenceGenomeExternal(
            dataset_group_id, refUriBuffer, reference_type, datasetGroupId, datasetId, checksum_alg, checksums,
            referenceIdBuffer, referenceNameBuffer, referenceMajorVersion, referenceMinorVersion,
            referencePatchVersion);
        setSequenceNamesVector(datasetsGroupReferenceGenome, sequenceNames);
        freeVector(sequenceNames);
        free(refUriBuffer);
        free(referenceNameBuffer);
        datasetsGroupReferenceGenome->hasSeek = true;
        datasetsGroupReferenceGenome->seekPosition = (size_t)seekPosition;
        return datasetsGroupReferenceGenome;
    }

    DatasetGroupId internalDatasetGroupId;
    DatasetId internalDatasetId;
    bool internalDatasetGroupIdSuccessfulRead = utils_read(&internalDatasetGroupId, inputFile);
    bool internalDatasetIdSuccessfulRead = readBigEndian16FromFile(&internalDatasetId, inputFile);
    if (!internalDatasetGroupIdSuccessfulRead || !internalDatasetIdSuccessfulRead) {
        fprintf(stderr, "Error reading internal reference.\n");
        return NULL;
    }

    DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome = initDatasetsGroupReferenceGenome(
        dataset_group_id, internalDatasetGroupId, internalDatasetId, referenceIdBuffer, referenceNameBuffer,
        referenceMajorVersion, referenceMinorVersion, referencePatchVersion);
    for (uint32_t seq_i = 0; seq_i < seqCountBuffer; seq_i++) {
        char* sequenceNameBuffer = getValue(sequenceNames, seq_i);
        setSequenceName(datasetsGroupReferenceGenome, seq_i, sequenceNameBuffer);
        free(sequenceNameBuffer);
    }
    freeVector(sequenceNames);
    free(referenceNameBuffer);
    datasetsGroupReferenceGenome->hasSeek = true;
    datasetsGroupReferenceGenome->seekPosition = (size_t)seekPosition;
    return datasetsGroupReferenceGenome;
}

size_t getDatasetGroupReferenceGenomeSeekPosition(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome) {
    return datasetsGroupReferenceGenome->seekPosition;
}

bool resizeSequences(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome, size_t newSize) {
    return resizeVector(datasetsGroupReferenceGenome->sequences, newSize);
}

bool setSequenceName(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome, size_t sequence_i, char* name) {
    size_t sequenceNameLength = strlen(name);
    char* copyNameBuffer = malloc((sequenceNameLength + 1) * sizeof(char));
    if (copyNameBuffer == NULL) {
        return false;
    }
    strncpy(copyNameBuffer, name, sequenceNameLength + 1);
    return setValue(datasetsGroupReferenceGenome->sequences, sequence_i, copyNameBuffer);
}

bool setSequenceNamesVector(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome, Vector* namesToCopy) {
    if (datasetsGroupReferenceGenome->sequences != NULL) {
        uint32_t seq_count = (uint32_t)getSize(datasetsGroupReferenceGenome->sequences);

        for (size_t seq_i = 0; seq_i < seq_count; seq_i++) {
            char* sequence_name = getValue(datasetsGroupReferenceGenome->sequences, seq_i);
            free(sequence_name);
        }

        freeVector(datasetsGroupReferenceGenome->sequences);
    }
    datasetsGroupReferenceGenome->sequences = shallowCopy(namesToCopy);
    return datasetsGroupReferenceGenome != NULL;
}
