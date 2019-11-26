#include "../../Boxes.h"
#include "../../utils.h"

DatasetsGroupReferenceMetadata* initDatasetsGroupReferenceMetadata() {
    DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata =
        (DatasetsGroupReferenceMetadata*)malloc(sizeof(DatasetsGroupReferenceMetadata));
    datasetsGroupReferenceMetadata->seekPosition = -1;
    return datasetsGroupReferenceMetadata;
}

void freeDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata) {
    freeByteArray(datasetsGroupReferenceMetadata->metadata);
    free(datasetsGroupReferenceMetadata);
}

bool copyContentDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata,
                                               char* content, uint64_t contentSize) {
    datasetsGroupReferenceMetadata->metadata = initByteArrayWithSize((uint64_t)contentSize);
    return copyBytesSource(datasetsGroupReferenceMetadata->metadata, (Byte*)content, contentSize);
}

bool defineContentDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata,
                                                 char* filename) {
    FILE* metadataInput = fopen(filename, "rb");
    if (metadataInput != NULL) {
        int errorSeekingEnd = fseek(metadataInput, 0, SEEK_END);
        long int size = ftell(metadataInput);
        int errorSeekingStart = fseek(metadataInput, 0, SEEK_SET);
        if (errorSeekingEnd != 0 || size <= 0L || errorSeekingStart != 0) {
            fprintf(stderr, "Error while determining reference metadata size.\n");
            return false;
        }
        datasetsGroupReferenceMetadata->metadata = initByteArrayWithSize((uint64_t)size);
        Byte* buffer = (Byte*)malloc((uint64_t)size);
        if (buffer == NULL) {
            fprintf(stderr, "Dataset group reference metadata memory buffer could not be created.\n");
            return false;
        }
        size_t metadataReadSize = fread(buffer, 1, (uint64_t)size, metadataInput);
        if (metadataReadSize != (uint64_t)size) {
            fprintf(stderr, "Dataset group reference metadata input could not be read.\n");
            free(buffer);
            return false;
        }
        copyBytesSource(datasetsGroupReferenceMetadata->metadata, buffer, (uint64_t)size);
        free(buffer);
        return true;
    }
    fprintf(stderr, "Dataset group reference metadata input could not be opened.\n");
    return false;
}

bool writeDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata,
                                         FILE* outputFile) {
    bool typeWriteSuccessful = fwrite(datasetsGroupReferenceMetadataName, sizeof(char), 4, outputFile) == 4;
    uint64_t datasetsGroupReferenceMetadataSize = getSizeDatasetsGroupReferenceMetadata(datasetsGroupReferenceMetadata);
    uint64_t datasetsGroupReferenceMetadataSizeBigEndian = nativeToBigEndian64(datasetsGroupReferenceMetadataSize);
    bool sizeWriteSuccessful =
        fwrite(&datasetsGroupReferenceMetadataSizeBigEndian, sizeof(uint64_t), 1, outputFile) == 1;
    if (!typeWriteSuccessful || !sizeWriteSuccessful) {
        return false;
    }
    return writeContentDatasetsGroupReferenceMetadata(datasetsGroupReferenceMetadata, outputFile);
}

bool writeContentDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata,
                                                FILE* outputFile) {
    if (datasetsGroupReferenceMetadata->metadata != NULL) {
        return writeByteArray(datasetsGroupReferenceMetadata->metadata, outputFile);
    }
    fprintf(stderr, "Dataset group metadata input data not set.\n");
    return false;
}

DatasetsGroupReferenceMetadata* parseDatasetsGroupReferenceMetadata(uint64_t boxContentSize, FILE* inputFile) {
    Byte* metadataBuffer = (Byte*)malloc((boxContentSize + 1) * sizeof(Byte));
    if (metadataBuffer == NULL) {
        fprintf(stderr, "Error while reserving memory for metadata buffer.\n");
        return NULL;
    }
    long seekPosition = ftell(inputFile);
    size_t metadataBufferReadSize = fread(metadataBuffer, sizeof(Byte), boxContentSize, inputFile);
    if (metadataBufferReadSize != boxContentSize) {
        fprintf(stderr, "Could not read requested number of bytes.\n");
        free(metadataBuffer);
        return NULL;
    }
    metadataBuffer[boxContentSize] = '\0';
    DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata = initDatasetsGroupReferenceMetadata();
    datasetsGroupReferenceMetadata->seekPosition = seekPosition;
    copyContentDatasetsGroupReferenceMetadata(datasetsGroupReferenceMetadata, (char*)metadataBuffer, boxContentSize);
    free(metadataBuffer);
    return datasetsGroupReferenceMetadata;
}

uint64_t getSizeDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata) {
    return BOX_HEADER_SIZE + getSizeContentDatasetsGroupReferenceMetadata(datasetsGroupReferenceMetadata);
}

uint64_t getSizeContentDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata) {
    return getSizeByteArray(datasetsGroupReferenceMetadata->metadata);
}

long getDatasetGroupReferenceMetadataSeekPosition(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata) {
    return datasetsGroupReferenceMetadata->seekPosition;
}
