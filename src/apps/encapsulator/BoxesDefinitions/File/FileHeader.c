//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include <string.h>
#include "../../Boxes.h"
#include "../../utils.h"

FileHeader* initFileHeader(char* minorVersion) {
    FileHeader* fileHeader = (FileHeader*)calloc(1, sizeof(FileHeader));
    strncpy(fileHeader->majorBrand, "MPEG-G", MAJOR_BRAND_SIZE);
    strncpy(fileHeader->minorVersion, minorVersion, MINOR_VERSION_SIZE);
    fileHeader->compatibleBrands = initVector();
    return fileHeader;
}

void freeFileHeader(FileHeader* fileHeader) {
    for (uint8_t i = 0; i < getSize(fileHeader->compatibleBrands); i++) {
        free(getValue(fileHeader->compatibleBrands, i));
    }
    freeVector(fileHeader->compatibleBrands);
    free(fileHeader);
}

void addCompatibleBrandToFileHeader(FileHeader* fileHeader, char* brand) {
    char* newCompatibleBrand = (char*)malloc(5 * sizeof(char));
    if (newCompatibleBrand != NULL) {
        strncpy(newCompatibleBrand, brand, BRAND_SIZE);
        pushBack(fileHeader->compatibleBrands, newCompatibleBrand);
    }
}

bool writeFileHeader(FileHeader* fileHeader, FILE* outputFile) {
    int boxNameSuccessfulWrite = fwrite(fileHeaderBoxName, 4, 1, outputFile) == 1;
    uint64_t boxSizeBigEndian = nativeToBigEndian64(12 + getSizeFileHeader(fileHeader));
    int fileHeaderSuccessful = fwrite(&boxSizeBigEndian, sizeof(boxSizeBigEndian), 1, outputFile) == 1;
    if (!boxNameSuccessfulWrite || !fileHeaderSuccessful || !writeFileHeaderContent(fileHeader, outputFile)) {
        fprintf(stderr, "Error writing file header.\n");
        return false;
    }
    return true;
}

bool writeFileHeaderContent(FileHeader* fileHeader, FILE* outputFile) {
    size_t majorBrandWrittenSize = fwrite(fileHeader->majorBrand, sizeof(char), MAJOR_BRAND_SIZE, outputFile);
    size_t minorVersionWrittenSize = fwrite(fileHeader->minorVersion, sizeof(char), MINOR_VERSION_SIZE, outputFile);
    if (majorBrandWrittenSize != MAJOR_BRAND_SIZE || minorVersionWrittenSize != MINOR_VERSION_SIZE) {
        fprintf(stderr, "Failed to write either major brand or minor version in file header.\n");
        return false;
    }
    for (uint8_t i = 0; i < getSize(fileHeader->compatibleBrands); i++) {
        size_t compatibleBrandWrittenSize =
            fwrite(getValue(fileHeader->compatibleBrands, i), sizeof(char), BRAND_SIZE, outputFile);
        if (compatibleBrandWrittenSize != BRAND_SIZE) {
            fprintf(stderr, "Failed to write compatible brand of index %u.\n", i);
            return false;
        }
    }

    return true;
}

FileHeader* parseFileHeader(uint64_t boxContentSize, FILE* inputFile) {
    if (boxContentSize < (MAJOR_BRAND_SIZE * sizeof(char) + sizeof(((FileHeader*)0)->minorVersion))) {
        fprintf(stderr, "File header's content size too small.\n");
        return NULL;
    }
    char majorBrand[MAJOR_BRAND_SIZE];
    char minorVersion[MINOR_VERSION_SIZE];
    size_t majorBrandReadSize = fread(majorBrand, sizeof(char), MAJOR_BRAND_SIZE, inputFile);
    if (majorBrandReadSize != MAJOR_BRAND_SIZE) {
        fprintf(stderr, "Error while reading major brand.\n");
        return NULL;
    }
    size_t minorVersionReadSize = fread(minorVersion, sizeof(char), MINOR_VERSION_SIZE, inputFile);
    if (minorVersionReadSize != MINOR_VERSION_SIZE) {
        fprintf(stderr, "Error while reading minor version.\n");
        return NULL;
    }
    FileHeader* fileHeader = initFileHeader(minorVersion);
    uint64_t remainingContentSize = boxContentSize - MAJOR_BRAND_SIZE - sizeof(((FileHeader*)0)->minorVersion);
    if (remainingContentSize % 4 != 0) {
        fprintf(stderr, "Content size inconsistent with file header type.\n");
        freeFileHeader(fileHeader);
        return NULL;
    }
    while (remainingContentSize > 0) {
        char compatibleBrand[BRAND_SIZE + 1];
        size_t compatibleBrandReadSize = fread(compatibleBrand, sizeof(char), BRAND_SIZE, inputFile);
        if (compatibleBrandReadSize != BRAND_SIZE) {
            fprintf(stderr, "Error while reading compatible brands.\n");
            freeFileHeader(fileHeader);
            return NULL;
        }
        compatibleBrand[BRAND_SIZE] = '\0';
        addCompatibleBrandToFileHeader(fileHeader, compatibleBrand);
        remainingContentSize -= BRAND_SIZE;
    }
    return fileHeader;
}

uint64_t getSizeFileHeader(FileHeader* fileHeader) {
    uint64_t sizeMajorBrand = MAJOR_BRAND_SIZE * sizeof(char);
    uint64_t sizeMinorVersion = MINOR_VERSION_SIZE * sizeof(char);
    uint64_t sizeCompatibleBrands = getSize(fileHeader->compatibleBrands) * BRAND_SIZE * sizeof(char);
    return sizeMajorBrand + sizeMinorVersion + sizeCompatibleBrands;
}