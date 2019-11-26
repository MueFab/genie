/**
 * @file
 * @copyright This file is part of Genie. See LICENSE for more details.
 */

#include "FileHandlingUtils.h"
#include <format/part2/parameter_set.h>

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

int createMPEGGFileNoMITFromByteStream(const char* fileName, char* outputFileName) {
    fprintf(stdout, "Bytestream file: %s\n", fileName);

    // Ref_information ref_information = readRefInformation(refInfoPath);

    (void)outputFileName;  // silence compiler warning

    std::ifstream inputFilestream;
    inputFilestream.open(fileName, std::ios::binary);

    util::BitReader inputFileBitReader(&inputFilestream);

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

    std::vector<std::unique_ptr<format::DataUnit>> dataUnits;

    while (inputFileBitReader.isGood()) {
        dataUnits.push_back(format::DataUnit::createFromBitReader(&inputFileBitReader));
    }
    for (auto const& dataUnit : dataUnits) {
        fprintf(stdout, "\ndata_unit_type:%u\n", (uint8_t)dataUnit->getDataUnitType());
        fprintf(stdout, "data_unit_size:%u\n", dataUnit->getDataUnitSize());
        fprintf(stdout, "Vector Size:%lu\n", dataUnit->rawData.size());
        fprintf(stdout, "First byte:%u -> Last byte:%u\n", dataUnit->rawData.front(), dataUnit->rawData.back());
    }

    // while (true) {
    //     auto dataUnit = DataUnitFactory.createFromBitReader(bitReader);
    //     if (dataUnit.getDataUnitType() == DataUnit::DataUnitType::ACCESS_UNIT) {
    //         // Copy to MPEG-G file
    //     }
    // }

    //
    //    FILE* inputFile = fopen(fileName, "rb");
    //    if(inputFile == nullptr){
    //        fprintf(stderr, "Could not open file.\n");
    //        return -1;
    //    }
    //
    //    DataUnits* dataUnits = nullptr;
    //
    //    if(parseDataUnits(inputFile, &dataUnits, (char*) fileName)){
    //        fprintf(stderr, "DataUnits could not be read.\n");
    //        return -1;
    //    }
    //
    //    Vector* datasetParameters = initVector();
    //    if(datasetParameters == nullptr){
    //        freeDataUnits(dataUnits);
    //        return -1;
    //    }
    //
    //    Vector* parameters;
    //    if(getDataUnitsParameters(dataUnits, &parameters) != 0){
    //        freeDataUnits(dataUnits);
    //        return -1;
    //    }
    //    size_t parametersCount = getSize(parameters);
    //    bool isAReferenceFile = false;
    //    for(size_t parameters_i = 0; parameters_i < parametersCount; parameters_i++){
    //        DataUnitParametersSet* parametersSet = (DataUnitParametersSet*) getValue(parameters, parameters_i);
    //
    //        pushBack(datasetParameters, initDatasetParametersWithParameters(
    //                0,0,parametersSet->parent_parameter_setId, parametersSet->parameter_setId,
    //
    //                // encoding_parameters
    //                /*parametersSet->dataset_type,
    //                parametersSet->alphabet_ID,
    //                parametersSet->read_length,
    //                parametersSet->number_of_template_segments_minus1,
    //                parametersSet->max_au_data_unit_size,
    //                parametersSet->pos_40_bits,
    //                parametersSet->qv_depth,
    //                parametersSet->as_depth,*/ //old code
    //                parametersSet->encodingParameters
    //
    //                //initByteArrayCopying(parametersSet->data)  TODO: needed?
    //        ));
    //        if(parametersSet->encodingParameters->dataset_type == 2){
    //            isAReferenceFile = true;
    //        }
    //    }
    //
    //    Vector* dataUnitsAccessUnits;
    //    if(getDataUnitsAccessUnits(dataUnits, &dataUnitsAccessUnits) != 0 || dataUnitsAccessUnits == nullptr){
    //        fprintf(stderr, "Error getting the data units access units.\n");
    //        freeDataUnits(dataUnits);
    //    }

    //
    //    size_t numberDataUnitsAccessUnits = getSize(dataUnitsAccessUnits);
    //    uint32_t numBlocksPerClass[7]; //cell for 0 will remain empty
    //    for(uint8_t class_i = 0; class_i<7; class_i++){
    //        numBlocksPerClass[class_i] = 0;
    //    }
    //    for(size_t access_unit_i = 0; access_unit_i < numberDataUnitsAccessUnits; access_unit_i++) {
    //        DataUnitAccessUnit* dataUnitAccessUnit = (DataUnitAccessUnit*)getValue(dataUnitsAccessUnits,
    //        access_unit_i); numBlocksPerClass[dataUnitAccessUnit->AU_type.classType] += 1;
    //    }
    //    uint32_t numBlocksInSequence = 0;
    //    for(uint8_t class_i = 0; class_i<6; class_i++){
    //        if(numBlocksInSequence < numBlocksPerClass[class_i]){
    //            numBlocksInSequence = numBlocksPerClass[class_i];
    //        }
    //    }
    //

    //    MPEGGFile* mpeggFile = initFile();
    //    if(mpeggFile == nullptr){
    //        fprintf(stderr, "Error creating mpeggFile.\n");
    //        freeDataUnits(dataUnits);
    //        return -1;
    //    }
    //    FileHeader* fileHeader = initFileHeader("1900");
    //    char* compatibleBrand = "\0\0\0\0";
    //    addCompatibleBrandToFileHeader(fileHeader, compatibleBrand);
    //    setFileHeaderToFile(mpeggFile, fileHeader);
    //
    //    DatasetsGroupContainer* datasetsGroupContainer = initDatasetsGroupContainer();
    //    if(datasetsGroupContainer == nullptr){
    //        fprintf(stderr, "Error creating datasetsGroupContainer.\n");
    //        freeFile(mpeggFile);
    //        freeDataUnits(dataUnits);
    //        return -1;
    //    }
    //    addDatasetsGroupToFile(mpeggFile, datasetsGroupContainer);
    //    DatasetsGroupHeader* datasetsGroupHeader = initDatasetsGroupHeader(0,0);
    //    setDatasetsGroupHeader(datasetsGroupContainer, datasetsGroupHeader);

    //
    //    char *ref_uri = ref_information.refUri;
    //    DatasetGroupId externalDatasetGroupId = 255;
    //    DatasetId externalDatasetId = 0;
    //    uint8_t checksumAlg = 0;
    //
    //    Vector* checksums = initVector();
    //    for(int i=0; i<ref_information.numberSequences; i++) {
    //        pushBack(checksums, calloc(512 / 8, sizeof(uint8_t)));
    //    }
    //
    //    ReferenceId referenceId = 0;
    //    if(isAReferenceFile){
    //        referenceId = 1;
    //    }
    //    char *referenceName = ref_information.refName;
    //    ReferenceMajorVersion majorVersion = 0;
    //    ReferenceMinorVersion minorVersion = 0;
    //    ReferencePatchVersion referencePathVersion = 0;
    //
    //    DatasetsGroupReferenceGenome* referenceGenome = initDatasetsGroupReferenceGenomeExternal(
    //            0,
    //            ref_uri,
    //            REFERENCE_TYPE_FASTA_REF,
    //            externalDatasetGroupId,
    //            externalDatasetId,
    //            checksumAlg,
    //            checksums,
    //            referenceId,
    //            referenceName,
    //            majorVersion,
    //            minorVersion,
    //            referencePathVersion
    //    );
    //    for(size_t i=0; i<ref_information.numberSequences; i++) {
    //        setSequenceName(referenceGenome, i, ref_information.sequenceName[i]);
    //    }
    //
    //
    //    Vector* referenceGenomes = initVector();
    //    pushBack(referenceGenomes, referenceGenome);
    //    setDatasetsGroupReferenceGenomes(datasetsGroupContainer, referenceGenomes);
    //

    //    DatasetContainer* datasetContainer = initDatasetContainer();
    //    if(datasetContainer == nullptr){
    //        fprintf(stderr, "Error creating datasetContainer.\n");
    //        freeFile(mpeggFile);
    //        freeDataUnits(dataUnits);
    //        return -1;
    //    }
    //
    //    DatasetGroupId datasetGroupId = 0;
    //    DatasetId datasetId = 0;
    //    char* version = "1900";
    //    bool unmapped_indexing_flag = false;
    //    bool byteOffsetSizeFlag = false;
    //    bool posOffsetIsUint40 = false;
    //    bool nonOverlappingAURange = true;
    //    bool blockHeaderFlag = true;
    //    uint16_t sequencesCount = 1;
    //    if(isAReferenceFile){
    //        sequencesCount = 0;
    //    }
    //    uint8_t datasetType = 1;
    //    if(isAReferenceFile){
    //        datasetType = 2;
    //    }
    //    uint8_t numClasses = 4;
    //    uint8_t alphabetId = 1;
    //    uint32_t numUClusters = 0;
    //    uint8_t uSignatureSize = 16;
    //    uint8_t uSignatureLength = 32;
    //    uint32_t numberUAccessUnits = numBlocksPerClass[CLASS_TYPE_CLASS_U];
    //    bool multipleAlignmentFlag = false;
    //    uint32_t multipleSignatureBase = 0;
    //
    //    DatasetHeader* datasetHeader = initDatasetHeaderNoMIT(
    //            datasetGroupId, datasetId, version, multipleAlignmentFlag, byteOffsetSizeFlag, posOffsetIsUint40,
    //            nonOverlappingAURange, blockHeaderFlag, sequencesCount, referenceId, datasetType, numClasses,
    //            alphabetId, numUClusters, uSignatureSize, uSignatureLength, /*uSignatureConstantLength,*/
    //            numberUAccessUnits, multipleSignatureBase
    //    );
    //    setClassContiguousModeFlag(datasetHeader, true);
    //    if(!isAReferenceFile) {
    //        setBlocksInSequence(datasetHeader, 0, (uint32_t) numBlocksInSequence);
    //        setThresholdForSequence(datasetHeader, 0, 100000);
    //    }
    //    setDatasetHeader(datasetContainer, datasetHeader);
    //    addDatasetsContainer(datasetsGroupContainer, datasetContainer);
    //    setDatasetParameters(datasetContainer, datasetParameters);
    //

    //    size_t numberAccessUnits = getSize(dataUnitsAccessUnits);
    //
    //    for(size_t access_unit_i = 0; access_unit_i < numberAccessUnits; access_unit_i++) {
    //        DataUnitAccessUnit *dataUnitAccessUnit = (DataUnitAccessUnit*)getValue(dataUnitsAccessUnits,
    //        access_unit_i);
    //
    //        uint32_t accessUnitId;
    //        uint8_t numBlocks;
    //        uint8_t parameterSetId;
    //        ClassType auType;
    //        uint32_t readsCount;
    //        uint16_t mmThreshold;
    //        uint32_t mmCount;
    //        SequenceID sequenceId;
    //        uint64_t auStartPosition;
    //        uint64_t auEndPosition;
    //        SequenceID referenceSequenceID;
    //        uint64_t referenceStart;
    //        uint64_t referenceEnd;
    //        uint64_t extendedAuStartPosition;
    //        uint64_t extendedAuEndPosition;
    //
    //        if(
    //                getDataUnitAccessUnitId(dataUnitAccessUnit, &accessUnitId) != 0 ||
    //                getDataUnitNumBlocks(dataUnitAccessUnit, &numBlocks) != 0 ||
    //                getDataUnitParameterSetId(dataUnitAccessUnit, &parameterSetId) != 0 ||
    //                getDataUnitAUType(dataUnitAccessUnit, &auType) != 0 ||
    //                getDataUnitReadsCount(dataUnitAccessUnit, &readsCount) != 0 ||
    //                getDataUnitMMThreshold(dataUnitAccessUnit, &mmThreshold) != 0 ||
    //                getDataUnitMMCount(dataUnitAccessUnit, &mmCount) != 0 ||
    //                getDataUnitSequenceId(dataUnitAccessUnit, &sequenceId) != 0 ||
    //                getDataUnitAuStartPosition(dataUnitAccessUnit, &auStartPosition) != 0 ||
    //                getDataUnitAuEndPosition(dataUnitAccessUnit, &auEndPosition) != 0 ||
    //                getDataUnitReferenceSequenceId(dataUnitAccessUnit, &referenceSequenceID) != 0 ||
    //                getDataUnitReferenceStartPosition(dataUnitAccessUnit, &referenceStart) != 0 ||
    //                getDataUnitReferenceEndPosition(dataUnitAccessUnit, &referenceEnd) != 0 ||
    //                getDataUnitExtendedAuStartPosition(dataUnitAccessUnit, &extendedAuStartPosition) != 0 ||
    //                getDataUnitExtendedAuEndPosition(dataUnitAccessUnit, &extendedAuEndPosition) != 0
    //                ){
    //            freeFile(mpeggFile);
    //            freeDataUnits(dataUnits);
    //            return -1;
    //        }
    //        AccessUnitContainer *accessUnitContainer = initAccessUnitContainer(datasetContainer);
    //
    //        AccessUnitHeader* accessUnitHeader = initAccessUnitHeaderWithValues(
    //                datasetContainer,
    //                accessUnitId,
    //                numBlocks,
    //                parameterSetId,
    //                auType,
    //                readsCount,
    //                mmThreshold,
    //                mmCount
    //        );
    //        setAccessUnitHeaderSequence_ID(accessUnitHeader, sequenceId);
    //        setAccessUnitHeaderAuStartPosition(accessUnitHeader, auStartPosition);
    //        setAccessUnitHeaderAuEndPosition(accessUnitHeader, auEndPosition);
    //        setAccessUnitHeaderAuExtendedStartPosition(accessUnitHeader, extendedAuStartPosition);
    //        setAccessUnitHeaderAuExtendedEndPosition(accessUnitHeader, extendedAuEndPosition);
    //        setAccessUnitHeaderReferenceSequence_ID(accessUnitHeader, referenceSequenceID);
    //        setAccessUnitHeaderReferenceStartPosition(accessUnitHeader, referenceStart);
    //        setAccessUnitHeaderReferenceEndPosition(accessUnitHeader, referenceEnd);
    //        setAccessUnitContainerHeader(accessUnitContainer, accessUnitHeader);
    //
    //
    //        for(uint8_t block_i=0; block_i<numBlocks; block_i++){
    //            Block* block;
    //            getDataUnitBlock(dataUnitAccessUnit, block_i, &block);
    //            Block* blockToAdd = initBlock(datasetContainer, cloneFromFile(block->payload));
    //            BlockHeader* blockHeader = initBlockHeader(
    //                    datasetContainer,
    //                    block->blockHeader->descriptorId,
    //                    block->blockHeader->payloadSize
    //            );
    //            setBlockHeader(blockToAdd, blockHeader);
    //            addBlock(accessUnitContainer, blockToAdd);
    //        }
    //
    //        addAccessUnitToDataset(datasetContainer, accessUnitContainer);
    //    }
    //
    //    FILE* outputFile = fopen(outputFileName, "wb");
    //    writeFile(mpeggFile, outputFile);
    //
    //    freeDataUnits(dataUnits);
    //    freeFile(mpeggFile);

    return 0;
}
