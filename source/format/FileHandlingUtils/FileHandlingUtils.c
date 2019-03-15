#include <Boxes.h>
#include <DataUnits/DataUnits.h>
#include <rbtree.h>
#include <AccessUnitTreeNode.h>
#include <DescriptorTreeNode.h>
#include "FileHandlingUtils.h"

int writeAccessUnitsFromMIT_seq_au_class(DatasetContainer* datasetContainer, FILE* outputFile);
int writeAccessUnitsFromMIT_seq_class_au(DatasetContainer *datasetContainer, FILE *outputFile);
int writeAccessUnitsNotFromMIT(DatasetContainer* datasetContainer, FILE* outputFile);
bool dataUnitAccessUnitIntersects(DataUnitAccessUnit* dataUnitAccessUnit, DatasetRequest datasetRequest);
void addDataUnitAccessUnitAndDependenciesIfRequired(DataUnitAccessUnit *dataUnitAccessUnit, DataUnits *dataUnits,
                                                    DatasetRequest datasetRequest, DatasetGroupId datatasetGroupId,
                                                    DatasetContainer *datasetContainer);
uint8_t addParametersInformation(
        DataUnits* dataUnits,
        DatasetContainer* datasetContainer,
        uint8_t parametersId,
        DatasetGroupId datasetGroupId
);

int extractAccessUnitsFromMIT(
        DatasetGroupId datasetGroupId,
        DatasetContainer* datasetContainer,
        DatasetRequest datasetRequest,
        DataUnits* dataUnits
);
int extractAccessUnitsNotFromMIT(
        DatasetGroupId datasetGroupId,
        DatasetContainer* datasetContainer,
        DatasetRequest datasetRequest,
        DataUnits* dataUnits
);


Ref_information readRefInformation(char* refInfoFilePath){
    FILE* refInfoInputFile = fopen(refInfoFilePath, "r");
    Ref_information ref_information;
    void* returnedValue = fgets(ref_information.refUri, 1024, refInfoInputFile);
    if(returnedValue){
        char *c = strchr(ref_information.refUri, '\n');
        if (c)
            *c = 0;
    }else{
        printf("> errno:%d feof:%d ferror:%d retval:%p s[0]:%d\n\n",
               errno, feof(refInfoInputFile), ferror(refInfoInputFile), returnedValue, ref_information.refUri[0]);
    }
    fscanf(refInfoInputFile, "%hhu\n", &(ref_information.numberSequences));
    ref_information.sequenceName = (char**)calloc(ref_information.numberSequences, sizeof(char*));
    for(int i=0; i<ref_information.numberSequences; i++){
        ref_information.sequenceName[i] = (char*)calloc(1024, sizeof(char));
        if (fgets(ref_information.sequenceName[i], 1024, refInfoInputFile))
        {
            char *c = strchr(ref_information.sequenceName[i], '\n');
            if (c)
                *c = 0;
        }
    }
    if (fgets(ref_information.refName, 1024, refInfoInputFile))
    {
        char *c = strchr(ref_information.refName, '\n');
        if (c)
            *c = 0;
    }
    return ref_information;
}

int readAndClone(char* filename, char* outputFileName){
    FILE* inputFile = fopen(filename,"rb");
    if(inputFile == NULL){
        fprintf(stderr,"File not found.\n");
        return -1;
    }
    MPEGGFile* file = parseFile(inputFile, filename);
    fclose(inputFile);

    if (file == NULL){
        fprintf(stderr, "file could not be read.\n");
        return -1;
    }else{
        
        FILE* outputFile = fopen(outputFileName,"wb");
        if(outputFile == NULL){
            fprintf(stderr,"Output file could not be opened.\n");
        }else{
            bool writeSuccessful = writeFile(file, outputFile);
            if (!writeSuccessful){
                fprintf(stderr, "Error while writing.\n");
            }
        }
        
        freeFile(file);
    }
    return 0;
}

int writeAccessUnitsFromMIT_seq_au_class(DatasetContainer* datasetContainer, FILE* outputFile){
    uint16_t num_sequences = getSequencesCount(getDatasetHeader(datasetContainer));
    for (uint16_t sequence_i = 0; sequence_i < num_sequences; sequence_i++) {
        uint32_t numAUs = getBlocksInSequence(getDatasetHeader(datasetContainer), sequence_i);
        for (uint32_t au_i = 0; au_i < numAUs; au_i++) {
            uint8_t numClasses = getClassesCount(getDatasetHeader(datasetContainer));
            for (uint8_t class_i = 0; class_i < numClasses; class_i++) {
                SequenceID sequenceID = getSequenceId(datasetContainer->datasetHeader, sequence_i);
                DataUnitAccessUnit *dataUnitAccessUnit = getDataUnitAccessUnit(
                        datasetContainer,
                        sequenceID,
                        class_i,
                        au_i
                );
                if (dataUnitAccessUnit == NULL) continue;

                uint8_t datasetType = getDatasetType(getDatasetHeader(datasetContainer));
                bool pos40Bits = isPosOffsetUint40(getDatasetHeader(datasetContainer));
                bool writeSuccessful = writeDataUnitAccessUnit(dataUnitAccessUnit, false, datasetType, pos40Bits, outputFile);
                if (!writeSuccessful) {
                    fprintf(stderr, "Error while writing.\n");
                    return -1;
                }

                freeDataUnitAccessUnit(dataUnitAccessUnit);
            }
        }
    }
    return 0;
}

int writeAccessUnitsFromMIT_seq_class_au(DatasetContainer *datasetContainer, FILE *outputFile){
    uint16_t num_sequences = getSequencesCount(getDatasetHeader(datasetContainer));
    for (uint16_t sequence_i = 0; sequence_i < num_sequences; sequence_i++) {
        uint8_t numClasses = getClassesCount(getDatasetHeader(datasetContainer));
        for (uint8_t class_i = 0; class_i < numClasses; class_i++) {
            uint32_t numAUs = getBlocksInSequence(getDatasetHeader(datasetContainer), sequence_i);
            for (uint32_t au_i = 0; au_i < numAUs; au_i++) {
                SequenceID sequenceID;
                sequenceID.sequenceID = sequence_i;
                DataUnitAccessUnit *dataUnitAccessUnit = getDataUnitAccessUnit(
                        datasetContainer,
                        sequenceID,
                        class_i,
                        au_i
                );
                if (dataUnitAccessUnit == NULL) continue;

                uint8_t datasetType = getDatasetType(getDatasetHeader(datasetContainer));
                bool pos40Bits = isPosOffsetUint40(getDatasetHeader(datasetContainer));
                bool writeSuccessful = writeDataUnitAccessUnit(dataUnitAccessUnit, false, datasetType, pos40Bits, outputFile);
                if (!writeSuccessful) {
                    fprintf(stderr, "Error while writing.\n");
                    return -1;
                }

                freeDataUnitAccessUnit(dataUnitAccessUnit);
            }
        }
    }
    return 0;
}

int writeAccessUnitsNotFromMIT(DatasetContainer* datasetContainer, FILE* outputFile){
    Vector* accessUnits = getDataUnitAccessUnits(datasetContainer);
    size_t numberAccessUnits = getSize(accessUnits);
    for(size_t accessUnit_i = 0; accessUnit_i < numberAccessUnits; accessUnit_i++){
        AccessUnitContainer* accessUnitContainer = getValue(accessUnits, accessUnit_i);
        AccessUnitHeader* accessUnitHeader = getAccessUnitHeader(accessUnitContainer);
        DataUnitAccessUnit* dataUnitAccessUnit = initDataUnitAccessUnit(
                getAccessUnitId(accessUnitHeader),
                getNumBlocks(accessUnitHeader),
                getParametersSetId(accessUnitHeader),
                getAccessUnitType(accessUnitHeader),
                getReadsCount(accessUnitHeader),
                getMMThreshold(accessUnitHeader),
                getMMCount(accessUnitHeader),
                getReferenceSequence(accessUnitHeader),
                getReferenceStart(accessUnitHeader),
                getReferenceEnd(accessUnitHeader),
                getAccessUnitSequenceID(accessUnitHeader),
                getAccessUnitStart(accessUnitHeader),
                getAccessUnitEnd(accessUnitHeader),
                getExtendedAccessUnitStart(accessUnitHeader),
                getExtendedAccessUnitEnd(accessUnitHeader)
        );

        uint8_t numBlocks = getNumBlocks(accessUnitHeader);
        Vector* blocks = getBlocks(accessUnitContainer);
        for(uint8_t block_i = 0; block_i<numBlocks; block_i++){
            Block* block = getValue(blocks, block_i);
            BlockHeader* blockHeader = block->blockHeader;
            FromFile* data = cloneFromFile(block->payload);

            Block* newBlock = initBlockWithHeader(
                    blockHeader->descriptorId,
                    blockHeader->payloadSize,
                    data
            );
            DataUnitBlockHeader* dataUnitBlockHeader =
                    initDataUnitBlockHeader(
                            blockHeader->descriptorId,
                            blockHeader->payloadSize
                    );
            addBlockToDataUnitAccessUnit(dataUnitAccessUnit, newBlock, dataUnitBlockHeader);
        }

        uint8_t datasetType = getDatasetType(getDatasetHeader(datasetContainer));
        bool posIs40Bits = isPosOffsetUint40(getDatasetHeader(datasetContainer));

        writeDataUnitAccessUnit(dataUnitAccessUnit, false, datasetType, posIs40Bits, outputFile);
        freeDataUnitAccessUnit(dataUnitAccessUnit);
    }
    return 0;
}

int createMPEG_GFileFromWriteByteStreams(char *inputFilename, char *outputFilename){
    FILE* inputFile = fopen(inputFilename,"rb");
    if(inputFile == NULL){
        fprintf(stderr,"File not found.\n");
        return -1;
    }
    MPEGGFile* file = parseFile(inputFile, inputFilename);
    fclose(inputFile);

    if (file == NULL){
        fprintf(stderr, "file could not be read.\n");
        return -1;
    }else{
        FILE* outputFile = fopen(outputFilename,"wb");


        if(outputFile == NULL){
            fprintf(stderr,"Output file could not be opened.\n");
        }else{
            DatasetsGroupContainer* datasetsGroupContainer = getDatasetGroupContainerByIndex(file, 0);
            DatasetContainer* datasetContainer = getDatasetContainerByIndex(datasetsGroupContainer, 0);

            size_t numberParameters;
            getNumberParameters(datasetContainer, &numberParameters);

            for(size_t parameters_i = 0; parameters_i<numberParameters; parameters_i++) {
                DatasetParameters* datasetParameters;
                getDatasetParameters(datasetContainer, parameters_i, &datasetParameters);
                ParametersSet* dataUnitParameterSet = initParametersSet(
                        datasetParameters->parent_parameter_set_ID,
                        datasetParameters->parameter_set_ID,
                        datasetParameters->dataset_type,
                        datasetParameters->alphabet_ID,
                        datasetParameters->reads_length,
                        datasetParameters->number_of_template_segments_minus1,
                        datasetParameters->max_au_data_unit_size,
                        datasetParameters->pos_40_bits,
                        datasetParameters->qv_depth,
                        datasetParameters->as_depth,
                        datasetParameters->parameters
                );
                writeParametersSet(dataUnitParameterSet, outputFile);
                freeDataUnitsParametersSet(dataUnitParameterSet);
            }

            if(isMITFlagSet(getDatasetHeader(datasetContainer))) {
                writeAccessUnitsFromMIT_seq_au_class(datasetContainer, outputFile);
            }else{
                writeAccessUnitsNotFromMIT(datasetContainer, outputFile);
            }
        }

        free(outputFilename);
        freeFile(file);
    }
    return 0;
}

int createMPEGGFileNoMITFromByteStream(char *fileName, char* refInfoPath, char* outputFileName) {
    Ref_information ref_information = readRefInformation(refInfoPath);

    FILE* inputFile = fopen(fileName, "rb");
    if(inputFile == NULL){
        fprintf(stderr, "Could not open file.\n");
        return -1;
    }

    DataUnits* dataUnits = NULL;
    if(parseDataUnits(inputFile, &dataUnits, fileName)){
        fprintf(stderr, "DataUnits could not be read.\n");
        return -1;
    }

    Vector* datasetParameters = initVector();
    if(datasetParameters == NULL){
        freeDataUnits(dataUnits);
        return -1;
    }

    Vector* parameters;
    if(getDataUnitsParameters(dataUnits, &parameters) != 0){
        freeDataUnits(dataUnits);
        return -1;
    }
    size_t parametersCount = getSize(parameters);
    bool isAReferenceFile = false;
    for(size_t parameters_i = 0; parameters_i < parametersCount; parameters_i++){
        ParametersSet* parametersSet = getValue(parameters, parameters_i);
        pushBack(datasetParameters, initDatasetParametersWithParameters(
                0,0,parametersSet->parent_parameter_setId, parametersSet->parameter_setId,
                parametersSet->dataset_type,
                parametersSet->alphabet_ID,
                parametersSet->reads_length,
                parametersSet->number_of_template_segments_minus1,
                parametersSet->max_au_data_unit_size,
                parametersSet->pos_40_bits,
                parametersSet->qv_depth,
                parametersSet->as_depth,
                initByteArrayCopying(parametersSet->data)
        ));
        if(parametersSet->dataset_type == 2){
            isAReferenceFile = true;
        }
    }



    Vector* dataUnitsAcessUnits;
    if(getDataUnitsAccessUnits(dataUnits, &dataUnitsAcessUnits) != 0 || dataUnitsAcessUnits == NULL){
        fprintf(stderr, "Error getting the data units access units.\n");
        freeDataUnits(dataUnits);
    }

    size_t numberDataUnitsAccessUnits = getSize(dataUnitsAcessUnits);
    uint32_t numBlocksPerClass[7]; //cell for 0 will remain empty
    for(uint8_t class_i = 0; class_i<7; class_i++){
        numBlocksPerClass[class_i] = 0;
    }
    for(size_t access_unit_i = 0; access_unit_i < numberDataUnitsAccessUnits; access_unit_i++) {
        DataUnitAccessUnit *dataUnitAccessUnit = getValue(dataUnitsAcessUnits, access_unit_i);
        numBlocksPerClass[dataUnitAccessUnit->AU_type.classType] += 1;
    }
    uint32_t numBlocksInSequence = 0;
    for(uint8_t class_i = 0; class_i<6; class_i++){
        if(numBlocksInSequence < numBlocksPerClass[class_i]){
            numBlocksInSequence = numBlocksPerClass[class_i];
        }
    }

    MPEGGFile* mpeggFile = initFile();
    if(mpeggFile == NULL){
        fprintf(stderr, "Error creating mpeggFile.\n");
        freeDataUnits(dataUnits);
        return -1;
    }
    FileHeader* fileHeader = initFileHeader("1900");
    char* compatibleBrand = "\0\0\0\0";
    addCompatibleBrandToFileHeader(fileHeader, compatibleBrand);
    setFileHeaderToFile(mpeggFile, fileHeader);

    DatasetsGroupContainer* datasetsGroupContainer = initDatasetsGroupContainer();
    if(datasetsGroupContainer == NULL){
        fprintf(stderr, "Error creating datasetsGroupContainer.\n");
        freeFile(mpeggFile);
        freeDataUnits(dataUnits);
        return -1;
    }
    addDatasetsGroupToFile(mpeggFile, datasetsGroupContainer);
    DatasetsGroupHeader* datasetsGroupHeader = initDatasetsGroupHeader(0,0);
    setDatasetsGroupHeader(datasetsGroupContainer, datasetsGroupHeader);

    char *ref_uri = ref_information.refUri;
    DatasetGroupId externalDatasetGroupId = 255;
    DatasetId externalDatasetId = 0;
    uint8_t checksumAlg = 0;

    Vector* checksums = initVector();
    for(int i=0; i<ref_information.numberSequences; i++) {
        pushBack(checksums, calloc(512 / 8, sizeof(uint8_t)));
    }

    ReferenceId referenceId = 0;
    if(isAReferenceFile){
        referenceId = 1;
    }
    char *referenceName = ref_information.refName;
    ReferenceMajorVersion majorVersion = 0;
    ReferenceMinorVersion minorVersion = 0;
    ReferencePatchVersion referencePathVersion = 0;

    DatasetsGroupReferenceGenome* referenceGenome = initDatasetsGroupReferenceGenomeExternal(
            0,
            ref_uri,
            REFERENCE_TYPE_FASTA_REF,
            externalDatasetGroupId,
            externalDatasetId,
            checksumAlg,
            checksums,
            referenceId,
            referenceName,
            majorVersion,
            minorVersion,
            referencePathVersion
    );
    for(size_t i=0; i<ref_information.numberSequences; i++) {
        setSequenceName(referenceGenome, i, ref_information.sequenceName[i]);
    }


    Vector* referenceGenomes = initVector();
    pushBack(referenceGenomes, referenceGenome);
    setDatasetsGroupReferenceGenomes(datasetsGroupContainer, referenceGenomes);


    DatasetContainer* datasetContainer = initDatasetContainer();
    if(datasetContainer == NULL){
        fprintf(stderr, "Error creating datasetContainer.\n");
        freeFile(mpeggFile);
        freeDataUnits(dataUnits);
        return -1;
    }

    DatasetGroupId datasetGroupId = 0;
    DatasetId datasetId = 0;
    char* version = "1900";
    bool unmapped_indexing_flag = false;
    bool byteOffsetSizeFlag = false;
    bool posOffsetIsUint40 = false;
    bool nonOverlappingAURange = true;
    bool blockHeaderFlag = true;
    uint16_t sequencesCount = 1;
    if(isAReferenceFile){
        sequencesCount = 0;
    }
    uint8_t datasetType = 1;
    if(isAReferenceFile){
        datasetType = 2;
    }
    uint8_t numClasses = 4;
    uint8_t alphabetId = 1;
    uint32_t numUClusters = 0;
    uint8_t uSignatureSize = 16;
    uint8_t uSignatureLength = 32;
    uint32_t numberUAccessUnits = numBlocksPerClass[CLASS_TYPE_CLASS_U];
    bool multipleAlignmentFlag = false;
    uint32_t multipleSignatureBase = 0;

    DatasetHeader* datasetHeader = initDatasetHeaderNoMIT(
            datasetGroupId, datasetId, version, multipleAlignmentFlag, byteOffsetSizeFlag, posOffsetIsUint40,
            nonOverlappingAURange, blockHeaderFlag, sequencesCount, referenceId, datasetType, numClasses, alphabetId,
            numUClusters, uSignatureSize, uSignatureLength, numberUAccessUnits,
            multipleSignatureBase
    );
    setClassContiguousModeFlag(datasetHeader, true);
    if(!isAReferenceFile) {
        setBlocksInSequence(datasetHeader, 0, (uint32_t) numBlocksInSequence);
        setThresholdForSequence(datasetHeader, 0, 100000);
    }
    setDatasetHeader(datasetContainer, datasetHeader);
    addDatasetsContainer(datasetsGroupContainer, datasetContainer);
    setDatasetParameters(datasetContainer, datasetParameters);


    size_t numberAccessUnits = getSize(dataUnitsAcessUnits);

    for(size_t access_unit_i = 0; access_unit_i < numberAccessUnits; access_unit_i++) {
        DataUnitAccessUnit *dataUnitAccessUnit = getValue(dataUnitsAcessUnits, access_unit_i);

        uint32_t accessUnitId;
        uint8_t numBlocks;
        uint8_t parameterSetId;
        ClassType auType;
        uint32_t readsCount;
        uint16_t mmThreshold;
        uint32_t mmCount;
        SequenceID sequenceId;
        uint64_t auStartPosition;
        uint64_t auEndPosition;
        SequenceID referenceSequenceID;
        uint64_t referenceStart;
        uint64_t referenceEnd;
        uint64_t extendedAuStartPosition;
        uint64_t extendedAuEndPosition;

        if(
                getDataUnitAccessUnitId(dataUnitAccessUnit, &accessUnitId) != 0 ||
                getDataUnitNumBlocks(dataUnitAccessUnit, &numBlocks) != 0 ||
                getDataUnitParameterSetId(dataUnitAccessUnit, &parameterSetId) != 0 ||
                getDataUnitAUType(dataUnitAccessUnit, &auType) != 0 ||
                getDataUnitReadsCount(dataUnitAccessUnit, &readsCount) != 0 ||
                getDataUnitMMThreshold(dataUnitAccessUnit, &mmThreshold) != 0 ||
                getDataUnitMMCount(dataUnitAccessUnit, &mmCount) != 0 ||
                getDataUnitSequenceId(dataUnitAccessUnit, &sequenceId) != 0 ||
                getDataUnitAuStartPosition(dataUnitAccessUnit, &auStartPosition) != 0 ||
                getDataUnitAuEndPosition(dataUnitAccessUnit, &auEndPosition) != 0 ||
                getDataUnitReferenceSequenceId(dataUnitAccessUnit, &referenceSequenceID) != 0 ||
                getDataUnitReferenceStartPosition(dataUnitAccessUnit, &referenceStart) != 0 ||
                getDataUnitReferenceEndPosition(dataUnitAccessUnit, &referenceEnd) != 0 ||
                getDataUnitExtendedAuStartPosition(dataUnitAccessUnit, &extendedAuStartPosition) != 0 ||
                getDataUnitExtendedAuEndPosition(dataUnitAccessUnit, &extendedAuEndPosition) != 0
                ){
            freeFile(mpeggFile);
            freeDataUnits(dataUnits);
            return -1;
        }
        AccessUnitContainer *accessUnitContainer = initAccessUnitContainer(datasetContainer);

        AccessUnitHeader* accessUnitHeader = initAccessUnitHeaderWithValues(
                datasetContainer,
                accessUnitId,
                numBlocks,
                parameterSetId,
                auType,
                readsCount,
                mmThreshold,
                mmCount
        );
        setAccessUnitHeaderSequence_ID(accessUnitHeader, sequenceId);
        setAccessUnitHeaderAuStartPosition(accessUnitHeader, auStartPosition);
        setAccessUnitHeaderAuEndPosition(accessUnitHeader, auEndPosition);
        setAccessUnitHeaderAuExtendedStartPosition(accessUnitHeader, extendedAuStartPosition);
        setAccessUnitHeaderAuExtendedEndPosition(accessUnitHeader, extendedAuEndPosition);
        setAccessUnitHeaderReferenceSequence_ID(accessUnitHeader, referenceSequenceID);
        setAccessUnitHeaderReferenceStartPosition(accessUnitHeader, referenceStart);
        setAccessUnitHeaderReferenceEndPosition(accessUnitHeader, referenceEnd);
        setAccessUnitContainerHeader(accessUnitContainer, accessUnitHeader);


        for(uint8_t block_i=0; block_i<numBlocks; block_i++){
            Block* block;
            getDataUnitBlock(dataUnitAccessUnit, block_i, &block);
            Block* blockToAdd = initBlock(datasetContainer, cloneFromFile(block->payload));
            BlockHeader* blockHeader = initBlockHeader(
                    datasetContainer,
                    block->blockHeader->descriptorId,
                    false,
                    block->blockHeader->payloadSize
            );
            setBlockHeader(blockToAdd, blockHeader);
            addBlock(accessUnitContainer, blockToAdd);
        }

        addAccessUnitToDataset(datasetContainer, accessUnitContainer);
    }

    FILE* outputFile = fopen(outputFileName, "wb");
    writeFile(mpeggFile, outputFile);

    freeDataUnits(dataUnits);
    freeFile(mpeggFile);

    return 0;
}

int createMPEGGFileMIT_AUC_FromByteStream(char *fileName, char* outputFileName){
    FILE* inputFile = fopen(fileName, "rb");
    if(inputFile == NULL){
        fprintf(stderr, "Could not open file.\n");
        return -1;
    }

    DataUnits* dataUnits = NULL;
    if(parseDataUnits(inputFile, &dataUnits, fileName)){
        fprintf(stderr, "DataUnits could not be read.\n");
        return -1;
    }

    Vector* dataUnitsAcessUnits;
    if(getDataUnitsAccessUnits(dataUnits, &dataUnitsAcessUnits) != 0 || dataUnitsAcessUnits == NULL){
        fprintf(stderr, "Error getting the data units access units.\n");
        freeDataUnits(dataUnits);
    }

    size_t numberDataUnitsAccessUnits = getSize(dataUnitsAcessUnits);
    uint32_t numBlocksPerClass[7]; //cell for 0 will remain empty
    for(uint8_t class_i = 0; class_i<7; class_i++){
        numBlocksPerClass[class_i] = 0;
    }


    ldns_rbtree_t * treeAccessUnits = ldns_rbtree_create(&accessUnitTreeNodeCompareFunction_Seq_Au_Class);
    ldns_rbtree_t * treeClassesUsed = ldns_rbtree_create(&descriptorTreeNodeCompareFunction);
    for(size_t access_unit_i = 0; access_unit_i < numberDataUnitsAccessUnits; access_unit_i++) {
        DataUnitAccessUnit *dataUnitAccessUnit = getValue(dataUnitsAcessUnits, access_unit_i);
        numBlocksPerClass[dataUnitAccessUnit->AU_type.classType] += 1;

        ldns_rbnode_t* data_unit_new_node;
        initializeAccessUnitTreeNode(
                &data_unit_new_node,
                dataUnitAccessUnit->sequenceId,
                dataUnitAccessUnit->ref_start_position,
                dataUnitAccessUnit->AU_type.classType,
                dataUnitAccessUnit->accessUnitId,
                dataUnitAccessUnit
        );
        ldns_rbtree_insert(treeAccessUnits, data_unit_new_node);
    }
    uint32_t numBlocksInSequence = 0;
    for(uint8_t class_i = 0; class_i<6; class_i++){
        if(numBlocksInSequence < numBlocksPerClass[class_i]){
            numBlocksInSequence = numBlocksPerClass[class_i];
        }
    }

    MPEGGFile* mpeggFile = initFile();
    if(mpeggFile == NULL){
        fprintf(stderr, "Error creating mpeggFile.\n");
        freeDataUnits(dataUnits);
        return -1;
    }
    FileHeader* fileHeader = initFileHeader("1900");
    char* compatibleBrand = "\0\0\0\0";
    addCompatibleBrandToFileHeader(fileHeader, compatibleBrand);
    setFileHeaderToFile(mpeggFile, fileHeader);

    DatasetsGroupContainer* datasetsGroupContainer = initDatasetsGroupContainer();
    if(datasetsGroupContainer == NULL){
        fprintf(stderr, "Error creating datasetsGroupContainer.\n");
        freeFile(mpeggFile);
        freeDataUnits(dataUnits);
        return -1;
    }
    addDatasetsGroupToFile(mpeggFile, datasetsGroupContainer);
    DatasetsGroupHeader* datasetsGroupHeader = initDatasetsGroupHeader(0,0);
    setDatasetsGroupHeader(datasetsGroupContainer, datasetsGroupHeader);

    char *ref_uri = "/home/daniele/_.NAS/genome/data/human/illumina/hs37d5.fa";
    DatasetGroupId externalDatasetGroupId = 255;
    DatasetId externalDatasetId = 0;
    uint8_t checksumAlg = 0;

    Vector* checksums = initVector();
    pushBack(checksums, calloc(512/8, sizeof(uint8_t)));

    ReferenceId referenceId = 0;
    char *referenceName = "hs37d5";
    ReferenceMajorVersion majorVersion = 0;
    ReferenceMinorVersion minorVersion = 0;
    ReferencePatchVersion referencePathVersion = 0;

    DatasetsGroupReferenceGenome* referenceGenome = initDatasetsGroupReferenceGenomeExternal(
            0,
            ref_uri,
            REFERENCE_TYPE_FASTA_REF,
            externalDatasetGroupId,
            externalDatasetId,
            checksumAlg,
            checksums,
            referenceId,
            referenceName,
            majorVersion,
            minorVersion,
            referencePathVersion
    );
    setSequenceName(referenceGenome, 0, "1");


    Vector* referenceGenomes = initVector();
    pushBack(referenceGenomes, referenceGenome);
    setDatasetsGroupReferenceGenomes(datasetsGroupContainer, referenceGenomes);


    DatasetContainer* datasetContainer = initDatasetContainer();
    if(datasetContainer == NULL){
        fprintf(stderr, "Error creating datasetContainer.\n");
        freeFile(mpeggFile);
        freeDataUnits(dataUnits);
        return -1;
    }

    DatasetGroupId datasetGroupId = 0;
    DatasetId datasetId = 0;
    char* version = "1900";
    bool byteOffsetSizeFlag = false;
    bool posOffsetIsUint40 = false;
    bool nonOverlappingAURange = true;
    bool blockHeaderFlag = true;
    uint16_t sequencesCount = 1;
    uint8_t datasetType = 1;
    uint8_t numClasses = 4;
    uint8_t alphabetId = 1;
    uint32_t numUClusters = 0;
    bool uSignatureConstantLength = true;
    uint8_t uSignatureSize = 16;
    uint8_t uSignatureLength = 32;
    uint32_t numberUAccessUnits = 0;
    bool multipleAlignmentFlag = false;
    uint32_t multipleSignatureBase = 1;
    bool classContiguous = false;

    DatasetHeader* datasetHeader = initDatasetHeader_MIT_AUC(
            datasetGroupId, datasetId, version, multipleAlignmentFlag, byteOffsetSizeFlag, posOffsetIsUint40,
            nonOverlappingAURange, sequencesCount, referenceId, datasetType, numClasses, alphabetId,
            numUClusters, uSignatureConstantLength, uSignatureSize, uSignatureLength, numberUAccessUnits,
            multipleSignatureBase, classContiguous
    );

    setClassContiguousModeFlag(datasetHeader, true);
    setBlocksInSequence(datasetHeader, 0, (uint32_t)numBlocksInSequence);
    setThresholdForSequence(datasetHeader, 0, 100000);
    setDatasetHeader(datasetContainer, datasetHeader);
    addDatasetsContainer(datasetsGroupContainer, datasetContainer);

    uint8_t class_i = 0;
    for(uint8_t class_id = 1; class_id<7; class_id++) {
        ClassType classType;
        classType.classType = class_id;
        if (numBlocksPerClass[class_id] != 0) {
            setClassType(datasetHeader, class_i, classType);
            class_i++;
        }
    }

    Vector* parameters;
    if(getDataUnitsParameters(dataUnits, &parameters) != 0){
        freeFile(mpeggFile);
        freeDataUnits(dataUnits);
        return -1;
    }
    size_t parametersCount = getSize(parameters);

    Vector* datasetParameters = initVector();
    if(datasetParameters == NULL){
        freeFile(mpeggFile);
        freeDataUnits(dataUnits);
        return -1;
    }

    for(size_t parameters_i = 0; parameters_i < parametersCount; parameters_i++){
        ParametersSet* parametersSet = getValue(parameters, parameters_i);
        pushBack(datasetParameters, initDatasetParametersWithParameters(
                0,0,parametersSet->parent_parameter_setId, parametersSet->parameter_setId,
                parametersSet->dataset_type,
                parametersSet->alphabet_ID,
                parametersSet->reads_length,
                parametersSet->number_of_template_segments_minus1,
                parametersSet->max_au_data_unit_size,
                parametersSet->pos_40_bits,
                parametersSet->qv_depth,
                parametersSet->as_depth,
                initByteArrayCopying(parametersSet->data)
        ));
    }
    setDatasetParameters(datasetContainer, datasetParameters);

    DatasetMasterIndexTable* datasetMasterIndexTable = initDatasetMasterIndexTable(
            datasetContainer
    );
    setDatasetMasterIndexTable(datasetContainer, datasetMasterIndexTable);

    uint64_t auOffset = getSizeDatasetContainer(datasetContainer) - BOX_HEADER_SIZE;

    ldns_rbnode_t* currentNode = ldns_rbtree_first(treeAccessUnits);
    while (currentNode != NULL && currentNode != LDNS_RBTREE_NULL){
        DataUnitAccessUnitTreeNodeKey* key = (DataUnitAccessUnitTreeNodeKey*)currentNode->key;
        DataUnitAccessUnit* dataUnitAccessUnit =
                ((DataUnitAccessUnitTreeNodeData*)currentNode->data)->dataUnitAccessUnit;

        uint32_t accessUnitId;
        uint8_t numBlocks;
        uint8_t parameterSetId;
        ClassType auType;
        uint32_t readsCount;
        uint16_t mmThreshold;
        uint32_t mmCount;
        SequenceID sequenceId;
        uint64_t auStartPosition;
        uint64_t auEndPosition;
        uint64_t extendedAuStartPosition;
        uint64_t extendedAuEndPosition;

        if(
                getDataUnitAccessUnitId(dataUnitAccessUnit, &accessUnitId) != 0 ||
                getDataUnitNumBlocks(dataUnitAccessUnit, &numBlocks) != 0 ||
                getDataUnitParameterSetId(dataUnitAccessUnit, &parameterSetId) != 0 ||
                getDataUnitAUType(dataUnitAccessUnit, &auType) != 0 ||
                getDataUnitReadsCount(dataUnitAccessUnit, &readsCount) != 0 ||
                getDataUnitMMThreshold(dataUnitAccessUnit, &mmThreshold) != 0 ||
                getDataUnitMMCount(dataUnitAccessUnit, &mmCount) != 0 ||
                getDataUnitSequenceId(dataUnitAccessUnit, &sequenceId) != 0 ||
                getDataUnitAuStartPosition(dataUnitAccessUnit, &auStartPosition) != 0 ||
                getDataUnitAuEndPosition(dataUnitAccessUnit, &auEndPosition) != 0 ||
                getDataUnitExtendedAuStartPosition(dataUnitAccessUnit, &extendedAuStartPosition) != 0 ||
                getDataUnitExtendedAuEndPosition(dataUnitAccessUnit, &extendedAuEndPosition) != 0
                ){
            freeFile(mpeggFile);
            freeDataUnits(dataUnits);
            return -1;
        }
        AccessUnitContainer *accessUnitContainer = initAccessUnitContainer(datasetContainer);

        AccessUnitHeader* accessUnitHeader = initAccessUnitHeaderWithValues(
                datasetContainer,
                accessUnitId,
                numBlocks,
                parameterSetId,
                auType,
                readsCount,
                mmThreshold,
                mmCount
        );
        setAccessUnitHeaderSequence_ID(accessUnitHeader, sequenceId);
        setAccessUnitHeaderAuStartPosition(accessUnitHeader, auStartPosition);
        setAccessUnitHeaderAuEndPosition(accessUnitHeader, auEndPosition);
        setAccessUnitHeaderAuExtendedStartPosition(accessUnitHeader, extendedAuStartPosition);
        setAccessUnitHeaderAuExtendedEndPosition(accessUnitHeader, extendedAuEndPosition);
        setAccessUnitContainerHeader(accessUnitContainer, accessUnitHeader);

        setStartAndEnd(
                datasetMasterIndexTable,
                sequenceId,
                auType,
                key->auId,
                dataUnitAccessUnit->auStartPosition,
                dataUnitAccessUnit->auEndPosition
        );
        setAUOffset(datasetMasterIndexTable, sequenceId, auType, key->auId, auOffset);


        for(uint8_t block_i=0; block_i<numBlocks; block_i++){
            Block* block;
            getDataUnitBlock(dataUnitAccessUnit, block_i, &block);
            Block* blockToAdd = initBlock(datasetContainer, cloneFromFile(block->payload));
            BlockHeader* blockHeader = initBlockHeader(
                    datasetContainer,
                    block->blockHeader->descriptorId,
                    false,
                    block->blockHeader->payloadSize
            );
            setBlockHeader(blockToAdd, blockHeader);
            addBlock(accessUnitContainer, blockToAdd);
        }

        addAccessUnitToDataset(datasetContainer, accessUnitContainer);

        auOffset += getAccessUnitContainerSize(accessUnitContainer);

        currentNode =ldns_rbtree_next(currentNode);
    }

    FILE* outputFile = fopen(outputFileName, "wb");
    writeFile(mpeggFile, outputFile);

    freeDataUnits(dataUnits);
    freeFile(mpeggFile);

    return 0;
}

int createMPEGGFile_DSC_FromByteStream(char *fileName, char* outputFileName){
    FILE* inputFile = fopen(fileName, "rb");
    if(inputFile == NULL){
        fprintf(stderr, "Could not open file.\n");
        return -1;
    }

    DataUnits* dataUnits = NULL;
    if(parseDataUnits(inputFile, &dataUnits, fileName)){
        fprintf(stderr, "DataUnits could not be read.\n");
        return -1;
    }

    Vector* dataUnitsAcessUnits;
    if(getDataUnitsAccessUnits(dataUnits, &dataUnitsAcessUnits) != 0 || dataUnitsAcessUnits == NULL){
        fprintf(stderr, "Error getting the data units access units.\n");
        freeDataUnits(dataUnits);
    }

    size_t numberDataUnitsAccessUnits = getSize(dataUnitsAcessUnits);
    uint32_t numBlocksPerClass[7]; //cell for 0 will remain empty
    for(uint8_t class_i = 0; class_i<7; class_i++){
        numBlocksPerClass[class_i] = 0;
    }


    ldns_rbtree_t * treeAccessUnits = ldns_rbtree_create(&accessUnitTreeNodeCompareFunction_Seq_Au_Class);
    ldns_rbtree_t * treesDescriptorsUsed[7];
    for(uint8_t class_i=1; class_i<7; class_i++){
        treesDescriptorsUsed[class_i] = ldns_rbtree_create(&descriptorTreeNodeCompareFunction);
    }

    uint32_t numberInDescriptorInClass[7][18];
    for(uint8_t classId = 1; classId<7; classId++){
        for(uint8_t descriptorId=0; descriptorId<18; descriptorId++){
            numberInDescriptorInClass[classId][descriptorId] = 0;
        }
    }

    for(size_t access_unit_i = 0; access_unit_i < numberDataUnitsAccessUnits; access_unit_i++) {
        DataUnitAccessUnit *dataUnitAccessUnit = getValue(dataUnitsAcessUnits, access_unit_i);
        uint8_t numBlocks;
        getDataUnitNumBlocks(dataUnitAccessUnit, &numBlocks);

        for(uint8_t block_i=0; block_i<numBlocks; block_i++){
            Block* block;
            getDataUnitBlock(dataUnitAccessUnit, block_i, &block);

            ldns_rbnode_t* descriptor_used_new_node;
            initializeDescriptorTreeNode(
                    &descriptor_used_new_node,
                    block->blockHeader->descriptorId
            );
            ldns_rbtree_insert(
                    treesDescriptorsUsed[dataUnitAccessUnit->AU_type.classType],
                    descriptor_used_new_node
            );

            numberInDescriptorInClass[dataUnitAccessUnit->AU_type.classType][block->blockHeader->descriptorId]++;
        }
    }

    uint8_t numDescriptors[7];
    for(uint8_t class_id=1; class_id<7; class_id++){
        numDescriptors[class_id] = 0;

        ldns_rbnode_t* currentNode = ldns_rbtree_first(treesDescriptorsUsed[class_id]);
        while (currentNode != NULL && currentNode != LDNS_RBTREE_NULL) {
            DescriptorTreeNodeKey *key = (DescriptorTreeNodeKey *) currentNode->key;

            numDescriptors[class_id] += 1;

            currentNode =ldns_rbtree_next(currentNode);
        }
    }


    for(size_t access_unit_i = 0; access_unit_i < numberDataUnitsAccessUnits; access_unit_i++) {
        DataUnitAccessUnit *dataUnitAccessUnit = getValue(dataUnitsAcessUnits, access_unit_i);
        numBlocksPerClass[dataUnitAccessUnit->AU_type.classType] += 1;


        ldns_rbnode_t* data_unit_new_node;
        initializeAccessUnitTreeNode(
                &data_unit_new_node,
                dataUnitAccessUnit->sequenceId,
                dataUnitAccessUnit->ref_start_position,
                dataUnitAccessUnit->AU_type.classType,
                dataUnitAccessUnit->accessUnitId,
                dataUnitAccessUnit
        );
        ldns_rbtree_insert(treeAccessUnits, data_unit_new_node);
    }
    uint32_t numBlocksInSequence = 0;
    for(uint8_t class_i = 0; class_i<6; class_i++){
        if(numBlocksInSequence < numBlocksPerClass[class_i]){
            numBlocksInSequence = numBlocksPerClass[class_i];
        }
    }

    MPEGGFile* mpeggFile = initFile();
    if(mpeggFile == NULL){
        fprintf(stderr, "Error creating mpeggFile.\n");
        freeDataUnits(dataUnits);
        return -1;
    }
    FileHeader* fileHeader = initFileHeader("1900");
    char* compatibleBrand = "\0\0\0\0";
    addCompatibleBrandToFileHeader(fileHeader, compatibleBrand);
    setFileHeaderToFile(mpeggFile, fileHeader);

    DatasetsGroupContainer* datasetsGroupContainer = initDatasetsGroupContainer();
    if(datasetsGroupContainer == NULL){
        fprintf(stderr, "Error creating datasetsGroupContainer.\n");
        freeFile(mpeggFile);
        freeDataUnits(dataUnits);
        return -1;
    }
    addDatasetsGroupToFile(mpeggFile, datasetsGroupContainer);
    DatasetsGroupHeader* datasetsGroupHeader = initDatasetsGroupHeader(0,0);
    setDatasetsGroupHeader(datasetsGroupContainer, datasetsGroupHeader);

    char *ref_uri = "/home/daniele/_.NAS/genome/data/human/illumina/hs37d5.fa";
    DatasetGroupId externalDatasetGroupId = 255;
    DatasetId externalDatasetId = 0;
    uint8_t checksumAlg = 0;

    Vector* checksums = initVector();
    pushBack(checksums, calloc(512/8, sizeof(uint8_t)));

    ReferenceId referenceId = 0;
    char *referenceName = "hs37d5";
    ReferenceMajorVersion majorVersion = 0;
    ReferenceMinorVersion minorVersion = 0;
    ReferencePatchVersion referencePathVersion = 0;

    DatasetsGroupReferenceGenome* referenceGenome = initDatasetsGroupReferenceGenomeExternal(
            0,
            ref_uri,
            REFERENCE_TYPE_FASTA_REF,
            externalDatasetGroupId,
            externalDatasetId,
            checksumAlg,
            checksums,
            referenceId,
            referenceName,
            majorVersion,
            minorVersion,
            referencePathVersion
    );
    setSequenceName(referenceGenome, 0, "1");


    Vector* referenceGenomes = initVector();
    pushBack(referenceGenomes, referenceGenome);
    setDatasetsGroupReferenceGenomes(datasetsGroupContainer, referenceGenomes);


    DatasetContainer* datasetContainer = initDatasetContainer();
    if(datasetContainer == NULL){
        fprintf(stderr, "Error creating datasetContainer.\n");
        freeFile(mpeggFile);
        freeDataUnits(dataUnits);
        return -1;
    }

    DatasetGroupId datasetGroupId = 0;
    DatasetId datasetId = 0;
    char* version = "1900";
    bool byteOffsetSizeFlag = false;
    bool posOffsetIsUint40 = false;
    bool nonOverlappingAURange = true;
    bool blockHeaderFlag = false;
    uint16_t sequencesCount = 1;
    uint8_t datasetType = 1;
    uint8_t numClasses = 4;
    uint8_t alphabetId = 1;
    uint32_t numUClusters = 0;
    bool uSignatureConstantLength = true;
    uint8_t uSignatureSize = 16;
    uint8_t uSignatureLength = 32;
    uint32_t numberUAccessUnits = 0;
    bool multipleAlignmentFlag = false;
    uint32_t multipleSignatureBase = 1;
    bool ordered_blocks_flag = true;

    DatasetHeader* datasetHeader = initDatasetHeader_DSC(
            datasetGroupId, datasetId, version, multipleAlignmentFlag, byteOffsetSizeFlag, posOffsetIsUint40,
            nonOverlappingAURange, sequencesCount, referenceId, datasetType, numClasses, alphabetId,
            numUClusters, uSignatureConstantLength, uSignatureSize, uSignatureLength, numberUAccessUnits,
            multipleSignatureBase, ordered_blocks_flag
    );
    setBlocksInSequence(datasetHeader, 0, (uint32_t)numBlocksInSequence);
    setThresholdForSequence(datasetHeader, 0, 100000);
    setDatasetHeader(datasetContainer, datasetHeader);
    addDatasetsContainer(datasetsGroupContainer, datasetContainer);

    uint8_t descriptorIdToIndex[7][18];
    uint64_t streamContainersTotalSize = 0;
    StreamContainer*** streams = (StreamContainer***)calloc(7, sizeof(StreamContainer**));
    for(uint8_t class_id=1; class_id<7; class_id++) {
        streams[class_id] = (StreamContainer**)calloc(18, sizeof(StreamContainer*));
        uint8_t descriptor_i=0;
        for(uint8_t descriptor_id=0; descriptor_id<18; descriptor_id++) {
            if(numberInDescriptorInClass[class_id][descriptor_id] != 0) {

                descriptorIdToIndex[class_id][descriptor_id] = descriptor_i;
                streams[class_id][descriptor_id] = initStreamContainer();

                ClassType classType;
                classType.classType = class_id;

                printf("initialising %i %i\n", class_id, descriptor_id);
                fflush(stdout);
                StreamHeader* streamHeader = initStreamHeader(
                        streams[class_id][descriptor_id],
                        descriptor_id,
                        classType,
                        numberInDescriptorInClass[class_id][descriptor_id]
                );

                setStreamHeader(streams[class_id][descriptor_id], streamHeader);
                addStreamContainerToDataset(datasetContainer, streams[class_id][descriptor_id]);

                streamContainersTotalSize += getSizeStreamContainer(streams[class_id][descriptor_id]);
                descriptor_i++;
            }
        }
    }


    uint8_t class_i = 0;
    for(uint8_t class_id = 1; class_id<7; class_id++) {
        if (numBlocksPerClass[class_id] != 0) {
            ClassType classType;
            classType.classType = class_id;
            setClassType(datasetHeader, class_i, classType);


            setNumberDescriptorsInClass(datasetHeader, class_i, numDescriptors[class_id]);

            uint8_t descriptor_i=0;
            ldns_rbnode_t* currentNode = ldns_rbtree_first(treesDescriptorsUsed[class_id]);
            while (currentNode != NULL && currentNode != LDNS_RBTREE_NULL) {
                DescriptorTreeNodeKey *key = (DescriptorTreeNodeKey *) currentNode->key;

                setDescriptorIdInClass(datasetHeader, class_i, descriptor_i, key->descriptorId);

                currentNode =ldns_rbtree_next(currentNode);
                descriptor_i++;
            }
            class_i++;
        }
    }

    Vector* parameters;
    if(getDataUnitsParameters(dataUnits, &parameters) != 0){
        freeFile(mpeggFile);
        freeDataUnits(dataUnits);
        return -1;
    }
    size_t parametersCount = getSize(parameters);

    Vector* datasetParameters = initVector();
    if(datasetParameters == NULL){
        freeFile(mpeggFile);
        freeDataUnits(dataUnits);
        return -1;
    }

    for(size_t parameters_i = 0; parameters_i < parametersCount; parameters_i++){
        ParametersSet* parametersSet = getValue(parameters, parameters_i);
        pushBack(datasetParameters, initDatasetParametersWithParameters(
                0,0,parametersSet->parent_parameter_setId, parametersSet->parameter_setId,
                parametersSet->dataset_type,
                parametersSet->alphabet_ID,
                parametersSet->reads_length,
                parametersSet->number_of_template_segments_minus1,
                parametersSet->max_au_data_unit_size,
                parametersSet->pos_40_bits,
                parametersSet->qv_depth,
                parametersSet->as_depth,
                initByteArrayCopying(parametersSet->data)
        ));
    }
    setDatasetParameters(datasetContainer, datasetParameters);

    DatasetMasterIndexTable* datasetMasterIndexTable = initDatasetMasterIndexTable(
            datasetContainer
    );
    setDatasetMasterIndexTable(datasetContainer, datasetMasterIndexTable);

    uint64_t auOffset = getSizeDatasetContainer(datasetContainer) - BOX_HEADER_SIZE - streamContainersTotalSize;

    ldns_rbnode_t* currentNode = ldns_rbtree_first(treeAccessUnits);
    while (currentNode != NULL && currentNode != LDNS_RBTREE_NULL){
        DataUnitAccessUnitTreeNodeKey* key = (DataUnitAccessUnitTreeNodeKey*)currentNode->key;
        DataUnitAccessUnit* dataUnitAccessUnit =
                ((DataUnitAccessUnitTreeNodeData*)currentNode->data)->dataUnitAccessUnit;

        SequenceID sequenceID;
        sequenceID.sequenceID = key->sequenceId;

        ClassType classType;
        classType.classType = key->classType;

        setStartAndEnd(
                datasetMasterIndexTable,
                sequenceID,
                classType,
                key->auId,
                dataUnitAccessUnit->auStartPosition,
                dataUnitAccessUnit->auEndPosition
        );
        setAUOffset(datasetMasterIndexTable, sequenceID, classType, key->auId, auOffset);

        uint32_t accessUnitId;
        uint8_t numBlocks;
        uint8_t parameterSetId;
        ClassType auType;
        uint32_t readsCount;
        uint16_t mmThreshold;
        uint32_t mmCount;
        SequenceID sequenceId;
        uint64_t auStartPosition;
        uint64_t auEndPosition;
        uint64_t extendedAuStartPosition;
        uint64_t extendedAuEndPosition;

        if(
                getDataUnitAccessUnitId(dataUnitAccessUnit, &accessUnitId) != 0 ||
                getDataUnitNumBlocks(dataUnitAccessUnit, &numBlocks) != 0 ||
                getDataUnitParameterSetId(dataUnitAccessUnit, &parameterSetId) != 0 ||
                getDataUnitAUType(dataUnitAccessUnit, &auType) != 0 ||
                getDataUnitReadsCount(dataUnitAccessUnit, &readsCount) != 0 ||
                getDataUnitMMThreshold(dataUnitAccessUnit, &mmThreshold) != 0 ||
                getDataUnitMMCount(dataUnitAccessUnit, &mmCount) != 0 ||
                getDataUnitSequenceId(dataUnitAccessUnit, &sequenceId) != 0 ||
                getDataUnitAuStartPosition(dataUnitAccessUnit, &auStartPosition) != 0 ||
                getDataUnitAuEndPosition(dataUnitAccessUnit, &auEndPosition) != 0 ||
                getDataUnitExtendedAuStartPosition(dataUnitAccessUnit, &extendedAuStartPosition) != 0 ||
                getDataUnitExtendedAuEndPosition(dataUnitAccessUnit, &extendedAuEndPosition) != 0
                ){
            freeFile(mpeggFile);
            freeDataUnits(dataUnits);
            return -1;
        }
        AccessUnitContainer *accessUnitContainer = initAccessUnitContainer(datasetContainer);

        AccessUnitHeader* accessUnitHeader = initAccessUnitHeaderWithValues(
                datasetContainer,
                accessUnitId,
                numBlocks,
                parameterSetId,
                auType,
                readsCount,
                mmThreshold,
                mmCount
        );
        setAccessUnitHeaderSequence_ID(accessUnitHeader, sequenceId);
        setAccessUnitHeaderAuStartPosition(accessUnitHeader, auStartPosition);
        setAccessUnitHeaderAuEndPosition(accessUnitHeader, auEndPosition);
        setAccessUnitHeaderAuExtendedStartPosition(accessUnitHeader, extendedAuStartPosition);
        setAccessUnitHeaderAuExtendedEndPosition(accessUnitHeader, extendedAuEndPosition);
        setAccessUnitContainerHeader(accessUnitContainer, accessUnitHeader);

        for(uint8_t block_i=0; block_i<numBlocks; block_i++){
            Block* block;
            getDataUnitBlock(dataUnitAccessUnit, block_i, &block);

            addBlockToStream(streams[auType.classType][block->blockHeader->descriptorId], cloneFromFile(block->payload));
        }

        addAccessUnitToDataset(datasetContainer, accessUnitContainer);

        auOffset += getAccessUnitContainerSize(accessUnitContainer);

        currentNode =ldns_rbtree_next(currentNode);
    }

    streamContainersTotalSize = 0;
    for(uint8_t class_id=1; class_id<7; class_id++) {
        for(uint8_t descriptor_id=0; descriptor_id<18; descriptor_id++) {
            if(numberInDescriptorInClass[class_id][descriptor_id] != 0) {
                streamContainersTotalSize += getSizeStreamContainer(streams[class_id][descriptor_id]);
            }
        }
    }

    uint64_t initialBlockOffset = getSizeDatasetContainer(datasetContainer)
                                  - BOX_HEADER_SIZE  - streamContainersTotalSize;
    initialBlockOffset += 2*BOX_HEADER_SIZE + 6;

    uint64_t currentBlockOffset = initialBlockOffset;
    for(uint8_t class_id=1; class_id<7; class_id++) {
        for (uint8_t descriptor_id = 0; descriptor_id < 18; descriptor_id++) {
            if (numberInDescriptorInClass[class_id][descriptor_id] != 0) {
                currentNode = ldns_rbtree_first(treeAccessUnits);
                while (currentNode != NULL && currentNode != LDNS_RBTREE_NULL) {
                    DataUnitAccessUnitTreeNodeKey *key = (DataUnitAccessUnitTreeNodeKey *) currentNode->key;
                    DataUnitAccessUnit *dataUnitAccessUnit =
                            ((DataUnitAccessUnitTreeNodeData *) currentNode->data)->dataUnitAccessUnit;


                    uint32_t accessUnitId;
                    uint8_t numBlocks;
                    uint8_t parameterSetId;
                    ClassType auType;
                    uint32_t readsCount;
                    uint16_t mmThreshold;
                    uint32_t mmCount;
                    SequenceID sequenceId;
                    uint64_t auStartPosition;
                    uint64_t auEndPosition;
                    uint64_t extendedAuStartPosition;
                    uint64_t extendedAuEndPosition;

                    if (
                            getDataUnitAccessUnitId(dataUnitAccessUnit, &accessUnitId) != 0 ||
                            getDataUnitNumBlocks(dataUnitAccessUnit, &numBlocks) != 0 ||
                            getDataUnitParameterSetId(dataUnitAccessUnit, &parameterSetId) != 0 ||
                            getDataUnitAUType(dataUnitAccessUnit, &auType) != 0 ||
                            getDataUnitReadsCount(dataUnitAccessUnit, &readsCount) != 0 ||
                            getDataUnitMMThreshold(dataUnitAccessUnit, &mmThreshold) != 0 ||
                            getDataUnitMMCount(dataUnitAccessUnit, &mmCount) != 0 ||
                            getDataUnitSequenceId(dataUnitAccessUnit, &sequenceId) != 0 ||
                            getDataUnitAuStartPosition(dataUnitAccessUnit, &auStartPosition) != 0 ||
                            getDataUnitAuEndPosition(dataUnitAccessUnit, &auEndPosition) != 0 ||
                            getDataUnitExtendedAuStartPosition(dataUnitAccessUnit, &extendedAuStartPosition) != 0 ||
                            getDataUnitExtendedAuEndPosition(dataUnitAccessUnit, &extendedAuEndPosition) != 0
                            ) {
                        freeFile(mpeggFile);
                        freeDataUnits(dataUnits);
                        return -1;
                    }

                    for(uint8_t block_i=0; block_i<numBlocks; block_i++){
                        Block* block;
                        getDataUnitBlock(dataUnitAccessUnit, block_i, &block);

                        if(auType.classType == class_id && block->blockHeader->descriptorId == descriptor_id ) {

                            uint8_t descriptor_i;
                            getDescriptorIndexForType(
                                    datasetHeader,
                                    auType,
                                    block->blockHeader->descriptorId,
                                    &descriptor_i
                            );
                            setOffset(
                                    datasetMasterIndexTable,
                                    sequenceId,
                                    auType,
                                    accessUnitId,
                                    descriptor_i,
                                    currentBlockOffset
                            );
                            currentBlockOffset += getBlockSize(block);
                        }
                    }
                    currentNode =ldns_rbtree_next(currentNode);
                }
                currentBlockOffset += 2*BOX_HEADER_SIZE + 6;
            }
        }
    }

    FILE* outputFile = fopen(outputFileName, "wb");
    writeFile(mpeggFile, outputFile);

    freeDataUnits(dataUnits);
    freeFile(mpeggFile);

    return 0;
}

int readAndWriteByteStreams(char *filename, char* outputFilename){
    FILE* inputFile = fopen(filename,"rb");
    if(inputFile == NULL){
        fprintf(stderr,"File not found.\n");
        return -1;
    }
    MPEGGFile* mpeggFile = parseFile(inputFile, filename);
    fclose(inputFile);

    if (mpeggFile == NULL){
        fprintf(stderr, "mpeggFile could not be read.\n");
        return -1;
    }else{
        FILE* outputFile = fopen(outputFilename,"wb");


        if(outputFile == NULL){
            fprintf(stderr,"Output mpeggFile could not be opened.\n");
        }else{
            DatasetsGroupContainer* datasetsGroupContainer = getDatasetGroupContainerByIndex(mpeggFile, 0);
            DatasetContainer* datasetContainer = getDatasetContainerByIndex(datasetsGroupContainer, 0);

            size_t numberParameters;
            getNumberParameters(datasetContainer, &numberParameters);

            for(size_t parameters_i = 0; parameters_i<numberParameters; parameters_i++) {
                DatasetParameters* datasetParameters;
                getDatasetParameters(datasetContainer, parameters_i, &datasetParameters);
                ParametersSet* dataUnitParameterSet = initParametersSet(
                        datasetParameters->parent_parameter_set_ID,
                        datasetParameters->parameter_set_ID,
                        datasetParameters->dataset_type,
                        datasetParameters->alphabet_ID,
                        datasetParameters->reads_length,
                        datasetParameters->number_of_template_segments_minus1,
                        datasetParameters->max_au_data_unit_size,
                        datasetParameters->pos_40_bits,
                        datasetParameters->qv_depth,
                        datasetParameters->as_depth,
                        datasetParameters->parameters
                );
                writeParametersSet(dataUnitParameterSet, outputFile);
                freeDataUnitsParametersSet(dataUnitParameterSet);
            }

            if(isMITFlagSet(getDatasetHeader(datasetContainer))) {
                writeAccessUnitsFromMIT_seq_au_class(datasetContainer, outputFile);
            }else{
                writeAccessUnitsNotFromMIT(datasetContainer, outputFile);
            }
        }
        
        freeFile(mpeggFile);
    }
    return 0;
}

DataUnits* extractRequestedInformationFromMPEGGFile(char* fileName, DatasetGroupRequest request){
    FILE* inputFile = fopen(fileName,"rb");
    if(inputFile == NULL){
        fprintf(stderr,"File not found.\n");
        return NULL;
    }
    MPEGGFile* mpeggFile = parseFile(inputFile, fileName);
    fclose(inputFile);

    if (mpeggFile == NULL){
        fprintf(stderr, "mpeggFile could not be read.\n");
        return NULL;
    }

    DataUnits* dataUnits = initDataUnits();

    DatasetsGroupContainer* datasetsGroupContainer = getDatasetGroupContainerById(mpeggFile, request.datasetGroupId);
    for(uint16_t datasetRequest_i = 0; datasetRequest_i<request.numDatasetsRequests; datasetRequest_i++){
        DatasetContainer* datasetContainer = getDatasetContainerById(
                datasetsGroupContainer, request.datasetRequests[datasetRequest_i].datasetId
        );
        extractRequestedInformationFromDataset(0, datasetContainer, request.datasetRequests[datasetRequest_i],
                                               dataUnits);
    }
    freeFile(mpeggFile);

    return dataUnits;
}

void extractRequestedInformationFromDataset(DatasetGroupId datasetGroupId, DatasetContainer *datasetContainer,
                                            DatasetRequest request, DataUnits *dataUnits) {
    if(isMITFlagSet(datasetContainer->datasetHeader)){
        extractAccessUnitsNotFromMIT(
                datasetGroupId,
                datasetContainer,
                request,
                dataUnits
        );
    }else{
        extractAccessUnitsFromMIT(
                datasetGroupId,
                datasetContainer,
                request,
                dataUnits
        );
    }
}

/*int extractAccessUnitFromMIT(
        DataUnits* dataUnits,
        DatasetContainer* datasetContainer
){

    uint16_t num_sequences = getSequencesCount(getDatasetHeader(datasetContainer));
    for (uint16_t sequence_i = 0; sequence_i < num_sequences; sequence_i++) {
        uint32_t numAUs = getBlocksInSequence(getDatasetHeader(datasetContainer), sequence_i);
        for (uint32_t au_i = 0; au_i < numAUs; au_i++) {
            uint8_t numClasses = getClassesCount(getDatasetHeader(datasetContainer));
            for (uint8_t class_i = 0; class_i < numClasses; class_i++) {
                SequenceID sequenceID = getSequenceId(datasetContainer->datasetHeader, sequence_i);
                DataUnitAccessUnit *dataUnitAccessUnit = getDataUnitAccessUnit(
                        datasetContainer,
                        sequenceID,
                        class_i,
                        au_i
                );
                if (dataUnitAccessUnit == NULL) continue;

                uint8_t datasetType = getDatasetType(getDatasetHeader(datasetContainer));
                bool pos40Bits = isPosOffsetUint40(getDatasetHeader(datasetContainer));
                bool writeSuccessful = writeDataUnitAccessUnit(dataUnitAccessUnit, false, datasetType, pos40Bits, outputFile);
                if (!writeSuccessful) {
                    fprintf(stderr, "Error while writing.\n");
                    return -1;
                }

                freeDataUnitAccessUnit(dataUnitAccessUnit);
            }
        }
    }
    return 0;

}*/


int extractAccessUnitsFromMIT(
        DatasetGroupId datasetGroupId,
        DatasetContainer* datasetContainer,
        DatasetRequest datasetRequest,
        DataUnits* dataUnits
){
    uint16_t num_sequences = getSequencesCount(getDatasetHeader(datasetContainer));
    for (uint16_t sequence_i = 0; sequence_i < num_sequences; sequence_i++) {
        uint32_t numAUs = getBlocksInSequence(getDatasetHeader(datasetContainer), sequence_i);
        for (uint32_t au_i = 0; au_i < numAUs; au_i++) {
            uint8_t numClasses = getClassesCount(getDatasetHeader(datasetContainer));
            for (uint8_t class_i = 0; class_i < numClasses; class_i++) {
                SequenceID sequenceID = getSequenceId(datasetContainer->datasetHeader, sequence_i);
                DataUnitAccessUnit *dataUnitAccessUnit = getDataUnitAccessUnit(
                        datasetContainer,
                        sequenceID,
                        class_i,
                        au_i
                );
                if (dataUnitAccessUnit == NULL) continue;

                addDataUnitAccessUnitAndDependenciesIfRequired(
                        dataUnitAccessUnit, dataUnits, datasetRequest, datasetGroupId, datasetContainer
                );
            }
        }
    }
    return 0;
}


int extractAccessUnitsNotFromMIT(
        DatasetGroupId datasetGroupId,
        DatasetContainer* datasetContainer,
        DatasetRequest datasetRequest,
        DataUnits* dataUnits
){
    Vector* accessUnits = getDataUnitAccessUnits(datasetContainer);
    size_t numberAccessUnits = getSize(accessUnits);
    for(size_t accessUnit_i = 0; accessUnit_i < numberAccessUnits; accessUnit_i++){
        AccessUnitContainer* accessUnitContainer = getValue(accessUnits, accessUnit_i);
        AccessUnitHeader* accessUnitHeader = getAccessUnitHeader(accessUnitContainer);
        DataUnitAccessUnit* dataUnitAccessUnit = initDataUnitAccessUnit(
                getAccessUnitId(accessUnitHeader),
                getNumBlocks(accessUnitHeader),
                getParametersSetId(accessUnitHeader),
                getAccessUnitType(accessUnitHeader),
                getReadsCount(accessUnitHeader),
                getMMThreshold(accessUnitHeader),
                getMMCount(accessUnitHeader),
                getReferenceSequence(accessUnitHeader),
                getReferenceStart(accessUnitHeader),
                getReferenceEnd(accessUnitHeader),
                getAccessUnitSequenceID(accessUnitHeader),
                getAccessUnitStart(accessUnitHeader),
                getAccessUnitEnd(accessUnitHeader),
                getExtendedAccessUnitStart(accessUnitHeader),
                getExtendedAccessUnitEnd(accessUnitHeader)
        );

        uint8_t numBlocks = getNumBlocks(accessUnitHeader);
        Vector* blocks = getBlocks(accessUnitContainer);
        for(uint8_t block_i = 0; block_i<numBlocks; block_i++){
            Block* block = getValue(blocks, block_i);
            BlockHeader* blockHeader = block->blockHeader;
            FromFile* data = cloneFromFile(block->payload);

            Block* newBlock = initBlockWithHeader(
                    blockHeader->descriptorId,
                    blockHeader->payloadSize,
                    data
            );
            DataUnitBlockHeader* dataUnitBlockHeader =
                    initDataUnitBlockHeader(
                            blockHeader->descriptorId,
                            blockHeader->payloadSize
                    );
            addBlockToDataUnitAccessUnit(dataUnitAccessUnit, newBlock, dataUnitBlockHeader);
        }

        addDataUnitAccessUnitAndDependenciesIfRequired(
                dataUnitAccessUnit, dataUnits, datasetRequest, datasetGroupId, datasetContainer
        );
    }
    return 0;
}

void addDataUnitAccessUnitAndDependenciesIfRequired(DataUnitAccessUnit *dataUnitAccessUnit, DataUnits *dataUnits,
                                                    DatasetRequest datasetRequest, DatasetGroupId datatasetGroupId,
                                                    DatasetContainer *datasetContainer) {
    if( dataUnitAccessUnitIntersects(dataUnitAccessUnit, datasetRequest)){

        uint8_t parameterIdDataUnitsScope = addParametersInformation(
                dataUnits,
                datasetContainer,
                dataUnitAccessUnit->parameterSetId,
                datatasetGroupId
        );

        dataUnitAccessUnit->parameterSetId = parameterIdDataUnitsScope;
        addDataUnitAccessUnit(dataUnits, dataUnitAccessUnit);

    }
}

uint8_t addParametersInformation(
        DataUnits* dataUnits,
        DatasetContainer* datasetContainer,
        uint8_t parametersId,
        DatasetGroupId datasetGroupId
){
    DatasetParameters* datasetParameters;
    getDatasetParametersById(datasetContainer, parametersId, &datasetParameters);

    uint8_t parameterSetIdDataUnitsScope;
    if(datasetParameters->parameter_set_ID != datasetParameters->parent_parameter_set_ID) {
        addParametersInformation(
                dataUnits, datasetContainer, datasetParameters->parent_parameter_set_ID, datasetGroupId
        );
    }

    if (!getParametersIdDataUnitsScope(
            dataUnits,
            datasetGroupId,
            datasetContainer->datasetHeader->datasetId,
            parametersId,
            &parameterSetIdDataUnitsScope
    )){
        addDataUnitDatasetParameters(dataUnits, datasetParameters, &parameterSetIdDataUnitsScope);
    }
    return parameterSetIdDataUnitsScope;
}

bool dataUnitAccessUnitIntersects(DataUnitAccessUnit* dataUnitAccessUnit, DatasetRequest datasetRequest){
    for(uint16_t sequenceRequest_i=0; sequenceRequest_i < datasetRequest.numSequences; sequenceRequest_i++){
        if(dataUnitAccessUnit->sequenceId.sequenceID
            == datasetRequest.sequenceRequest[sequenceRequest_i].sequenceID.sequenceID
        ){
            SequenceRequest sequenceRequest = datasetRequest.sequenceRequest[sequenceRequest_i];
            ClassRequest classRequest = sequenceRequest.classRequest[dataUnitAccessUnit->AU_type.classType-1];

            uint64_t accessUnitStart = dataUnitAccessUnit->auStartPosition;
            uint64_t accessUnitEnd = dataUnitAccessUnit->auEndPosition;
            uint64_t classRequestStart = classRequest.start;
            uint64_t classRequestEnd = classRequest.end;

            uint64_t maxStart = accessUnitStart > classRequestStart ? accessUnitStart : classRequestStart;
            uint64_t minEnd = accessUnitEnd < classRequestEnd ? accessUnitEnd : classRequestEnd;
            if(maxStart <= minEnd){
                return true;
            }
        }
    }
    return false;
}