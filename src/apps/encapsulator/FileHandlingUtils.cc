/**
 * @file
 * @copyright This file is part of Genie. See LICENSE for more details.
 */

#include "FileHandlingUtils.h"
#include "genie/core/parameter/parameter_set.h"
#include "genie/format//mpegg_p1/dataset_group.h"
#include "genie/format/mgb/data-unit-factory.h"
#include "genie/format/mpegg_p1/dataset.h"
#include "genie/format/mpegg_p1/mpegg_file.h"

// DatasetHeader *
// initDatasetHeaderNoMIT(DatasetGroupId datasetGroupId, DatasetId datasetId, char *version, bool multipleAlignmentFlag,
//                       bool byteOffsetSizeFlag, bool posOffsetIsUint40, bool nonOverlappingAURange,
//                       bool blockHeaderFlag, uint16_t sequencesCount, ReferenceId referenceId, uint8_t datasetType,
//                       uint8_t numClasses, uint8_t alphabetId, uint32_t numUClusters, uint8_t uSignatureSize,
//                       uint8_t uSignatureLength, bool uSignatureConstantLength, uint32_t numberUAccessUnits,
//                       uint32_t multipleSignatureBase) {
//    DatasetHeader* datasetHeader = (DatasetHeader*)malloc(sizeof(DatasetHeader));
//
//    datasetHeader->datasetGroupId = datasetGroupId;
//    datasetHeader->datasetId = datasetId;
//    memcpy(datasetHeader->version, version, 4);
//    datasetHeader->multipleAlignmentFlag = multipleAlignmentFlag;
//    datasetHeader->byteOffsetSizeFlag = byteOffsetSizeFlag;
//    datasetHeader->posOffsetIsUint40 = posOffsetIsUint40;
//    datasetHeader->nonOverlappingAURange_flag = nonOverlappingAURange;
//    datasetHeader->blockHeaderFlag = blockHeaderFlag;
//
//    datasetHeader->mitFlag = false;
//    datasetHeader->classContiguosModeFlag = false;
//    datasetHeader->orderedBlocksFlag = false;
//
//    datasetHeader->sequencesCount = sequencesCount;
//    datasetHeader->referenceId = referenceId;
//    datasetHeader->seqIds = (SequenceID*)calloc(datasetHeader->sequencesCount, sizeof(SequenceID));
//    datasetHeader->seqBlocks = (uint32_t*)calloc(datasetHeader->sequencesCount, sizeof(uint32_t));
//    datasetHeader->datasetType=datasetType;
//    datasetHeader->numClasses=numClasses;
//    datasetHeader->classId = (ClassType*)calloc(numClasses, sizeof(ClassType*));
//    datasetHeader->numDescriptors = (uint8_t*)calloc(numClasses, sizeof(uint8_t));
//    datasetHeader->descriptorId = (uint8_t**)calloc(numClasses, sizeof(uint8_t*));
//
//    datasetHeader->alphabetId = alphabetId;
//    datasetHeader->numUClusters = numUClusters;
//    datasetHeader->uSignatureConstantLength = uSignatureConstantLength;
//    datasetHeader->uSignatureSize = uSignatureSize;
//    datasetHeader->uSignatureLength = uSignatureLength;
//    datasetHeader->numberUAccessUnits = numberUAccessUnits;
//    datasetHeader->multipleSignatureBase = multipleSignatureBase;
//    datasetHeader->thresholds = (uint32_t*)calloc(datasetHeader->sequencesCount, sizeof(uint32_t));
//    datasetHeader->hasSeek = false;
//    datasetHeader->seekPosition = 0;
//    return datasetHeader;
//}

// Ref_information readRefInformation(char* refInfoFilePath){
//    FILE* refInfoInputFile = fopen(refInfoFilePath, "r");
//    Ref_information ref_information;
//    void* returnedValue = fgets(ref_information.refUri, 1024, refInfoInputFile);
//    if(returnedValue){
//        char *c = strchr(ref_information.refUri, '\n');
//        if (c)
//            *c = 0;
//    }else{
//        printf("> errno:%d feof:%d ferror:%d retval:%p s[0]:%d\n\n",
//               errno, feof(refInfoInputFile), ferror(refInfoInputFile), returnedValue, ref_information.refUri[0]);
//    }
//    fscanf(refInfoInputFile, "%i\n", &(ref_information.numberSequences));
//    ref_information.sequenceName = (char**)calloc(ref_information.numberSequences, sizeof(char*));
//    for(int i=0; i<ref_information.numberSequences; i++){
//        ref_information.sequenceName[i] = (char*)calloc(1024, sizeof(char));
//        if (fgets(ref_information.sequenceName[i], 1024, refInfoInputFile))
//        {
//            char *c = strchr(ref_information.sequenceName[i], '\n');
//            if (c)
//                *c = 0;
//        }
//    }
//    if (fgets(ref_information.refName, 1024, refInfoInputFile))
//    {
//        char *c = strchr(ref_information.refName, '\n');
//        if (c)
//            *c = 0;
//    }
//    return ref_information;
//}

int createMPEGGFileNoMITFromByteStream(const std::string& fileName, const std::string& outputFileName) {
    fprintf(stdout, "Bytestream file: %s\n", fileName.c_str());

    // Ref_information ref_information = readRefInformation(refInfoPath);

    std::ifstream inputFilestream;
    inputFilestream.open(fileName, std::ios::binary);
    if (!inputFilestream.good()) {
        fprintf(stderr, "File could not be opened!\n");
        return -1;
    }

    genie::util::BitReader inputFileBitReader(inputFilestream);

    /*uint32_t value;
    char buffer[256];
    fprintf(stdout, "%s\n", buffer);
    inputFileBitReader.readNBits(32, buffer);
    fprintf(stdout, "%s\n", buffer);

    inputFileBitReader.readNBits(8, &value);
    fprintf(stdout, "%u\n", value);*/

    /*    uint32_t value;
        for (int j = 0; j < 6; ++j) {
            fprintf(stdout, "Byte %i:", j);

            for (int i = 0; i < 1; i++) {
                inputFileBitReader.readNBits(8, &value);
                fprintf(stdout, "%u \n", value);
            }
        }*/

    std::vector<genie::format::mgb::AccessUnit> accessUnits;

    auto dataUnitFactory = genie::format::mgb::DataUnitFactory();
    while (true) {
        auto au = dataUnitFactory.read(inputFileBitReader);
        if (!au) {
            break;
        }
        accessUnits.push_back(std::move(au.get()));
    }

    for (auto const& au : accessUnits) {
        fprintf(stdout, "\ndata_unit_type:%u\n", (uint8_t)au.getDataUnitType());
    }

    // ---------------------------------------------------------------------------------------------------------------------

    // Dataset consists of:
    // - DatasetHeader
    // - DatasetParameterSet
    // - AccessUnit[]

    std::vector<genie::format::mpegg_p1::Dataset> datasets;
    datasets.emplace_back(dataUnitFactory, accessUnits, 22);
    // std::cout << datasets.front().getDatasetParameterSetDatasetID() << std::endl;

    // DatasetGroup consists of:
    // - DatasetGroupHeader
    // - Dataset[]

    std::vector<genie::format::mpegg_p1::DatasetGroup> datasetGroups;
    datasetGroups.emplace_back(&datasets);

    fprintf(stdout, "%u\n", datasetGroups.front().getDatasets().front().getDatasetHeader().getDatasetId());
    fprintf(stdout, "%u\n", datasetGroups.front().getDatasets().front().getDatasetHeader().getDatasetGroupId());
    fprintf(stdout, "%u\n", datasetGroups.front().getDatasets().front().getDatasetParameterSetDatasetID());
    fprintf(stdout, "%u\n", datasetGroups.front().getDatasets().front().getDatasetParameterSetDatasetGroupID());

    // MpeggFile consists of:
    // - FileHeader
    // - DatasetGroup[]

    genie::format::mpegg_p1::MpeggFile mpeggFile(&datasetGroups);
    fprintf(stdout, "%u\n",
            mpeggFile.getDatasetGroups().front().getDatasets().front().getDatasetHeader().getDatasetGroupId());

    // FIXME: try to open file at function start to avoid decompressing without writing?!?
    std::string outputFileNameNew(outputFileName);
    if (outputFileNameNew.empty()) {
        fprintf(stdout, "empty outputFileName!!!\n");
        outputFileNameNew = "";  // FIXME: add default path/filename and remove else clause?!?
    } else {
        fprintf(stdout, "Writing to file %s...\n", outputFileNameNew.c_str());

        std::filebuf fb;
        fb.open(outputFileNameNew, std::ios::out);
        if (!fb.is_open()) {
            fprintf(stdout, "File is not open!!!\n");
        }
        std::ostream os(&fb);
        genie::util::BitWriter outputFileBitReader(&os);

        mpeggFile.writeToFile(outputFileBitReader);

        fb.close();
        fprintf(stdout, "Writing to file %s done!!!\n", outputFileNameNew.c_str());
    }
    
    return 0;
}
