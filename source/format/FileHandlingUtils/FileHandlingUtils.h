

#ifndef MPEGG_REFERENCE_SW_FILEHANDLINGUTILS_H
#define MPEGG_REFERENCE_SW_FILEHANDLINGUTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <errno.h>

typedef struct{
    char refUri[1024];
    uint8_t numberSequences;
    char** sequenceName;
    char refName[1024];
} Ref_information;

typedef struct{
    uint64_t start;
    uint64_t end;
} ClassRequest;

typedef struct{
    SequenceID sequenceID;
    ClassRequest classRequest[6];
} SequenceRequest;

typedef struct{
    DatasetId datasetId;
    uint16_t numSequences;
    SequenceRequest* sequenceRequest;
} DatasetRequest;

typedef struct{
    DatasetGroupId datasetGroupId;
    uint16_t numDatasetsRequests;
    DatasetRequest* datasetRequests;
} DatasetGroupRequest;

DataUnits* extractRequestedInformationFromMPEGGFile(char* fileName, DatasetGroupRequest request);
void extractRequestedInformationFromDataset(DatasetGroupId datasetGroupId, DatasetContainer *datasetContainer,
                                            DatasetRequest request, DataUnits *dataUnits);
void extractRequestedReferenceInformationFromDataset(DatasetGroupId datasetGroupId, DatasetContainer *datasetContainer,
                                                     DatasetRequest request, DataUnits *dataUnits);

Ref_information readRefInformation(char* refInfoFilePath);
int readAndClone(char* filename, char* outputFileName);
int createMPEG_GFileFromWriteByteStreams(char *inputFilename, char *outputFilename);
int createMPEGGFileNoMITFromByteStream(char *fileName, char* refInfoPath, char* outputFileName);
int createMPEGGFileMIT_AUC_FromByteStream(char *fileName, char* outputFileName);
int createMPEGGFile_DSC_FromByteStream(char *fileName, char* outputFileName);
int readAndWriteByteStreams(char *filename, char* outputFilename);


#endif //MPEGG_REFERENCE_SW_FILEHANDLINGUTILS_H
