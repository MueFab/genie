//
// Created by daniel on 1/11/18.
//

#include <string>
#include <cstring>
#include <vector>
#include <coding/CoderConstants.h>
#include <coding/ReturnStructures.h>
#include "MPEGGFileCreator.h"


DatasetGroup::DatasetGroup(DatasetGroupId datasetGroupId) {
    id = datasetGroupId;
    datasetsGroupContainer = initDatasetsGroupContainer();
    DatasetsGroupHeader* datasetsGroupHeader = initDatasetsGroupHeader(datasetGroupId, 1);
    setDatasetsGroupHeader(datasetsGroupContainer, datasetsGroupHeader);
}

DatasetsGroupContainer *DatasetGroup::getDatasetsGroupContainer() const {
    return datasetsGroupContainer;
}

MPEGGFileCreator::MPEGGFileCreator() {
    file = initFile();
    char minorVersion[] = "1900";
    FileHeader *fileHeader = initFileHeader(minorVersion);
    setFileHeaderToFile(file, fileHeader);
}

DatasetGroup* MPEGGFileCreator::addDatasetGroup() {
    auto * datasetGroup = new DatasetGroup(datasetsGroupCreated);
    datasetsGroupCreated++;
    addDatasetsGroupToFile(file, datasetGroup->getDatasetsGroupContainer());
    return datasetGroup;
}

FakeInternalReference::FakeInternalReference(
    const std::string &referenceURI,
    const std::string &sequenceName,
    DatasetGroupId datasetGroupId,
    DatasetId  datasetId,
    ReferenceId referenceId,
    const std::vector<std::string>& referenceFiles,
    const std::vector<uint64_t>& accessUnitsStarts,
    const std::vector<uint64_t>& accessUnitsEnds
): accessUnitsStarts(accessUnitsStarts), accessUnitsEnds(accessUnitsEnds) {
    char* refURI = (char*)calloc(100,sizeof(char));
    referenceURI.copy(refURI,99);
    FakeInternalReference::datasetId = datasetId;
    FakeInternalReference::referenceId = referenceId;


    referenceGenome = initDatasetsGroupReferenceGenome(
            datasetGroupId,
            datasetId,
            referenceId,
            refURI,
            1,
            2,
            3
    );
    resizeSequences(referenceGenome, 1);
    char* sequenceNameBuffer = (char*)calloc(100,sizeof(char));
    sequenceName.copy(sequenceNameBuffer,99);
    setSequenceName(referenceGenome,0, sequenceNameBuffer);

    FakeInternalReference::referenceFiles = referenceFiles;
}

void FakeInternalReference::addAsDatasetToDatasetGroup(
        DatasetsGroupContainer* datasetsGroupContainer,
        DatasetGroupId datasetGroupId
){
    DatasetContainer* datasetContainer = initDatasetContainer();


    DatasetId datasetId = FakeInternalReference::datasetId;
    char version[]="1900";
    bool unmapped_indexing_flag = false;
    bool byteOffsetSizeFlag = false;
    bool posOffsetIsUint40 = false;
    bool nonOverlappingAURange = true;
    bool blockHeaderFlag = true;
    uint16_t sequencesCount = 1;
    ReferenceId referenceId = FakeInternalReference::referenceId;
    uint8_t datasetType = 2;
    uint8_t numClasses = 1;
    uint8_t alphabetId = 0;
    uint32_t numUClusters = 0;
    bool uSignatureConstantLength = true;
    uint8_t uSignatureSize = 0;
    uint8_t uSignatureLength = 0;
    uint32_t numberUAccessUnits = 0;
    bool multipleAlignmentFlag = false;
    uint32_t multipleSignatureBase = 0;

    DatasetHeader* datasetHeader = initDatasetHeader(
            datasetGroupId, datasetId, version, unmapped_indexing_flag, byteOffsetSizeFlag, posOffsetIsUint40,
            nonOverlappingAURange, blockHeaderFlag, sequencesCount, referenceId, datasetType, numClasses, alphabetId,
            numUClusters, uSignatureConstantLength, uSignatureSize, uSignatureLength, numberUAccessUnits,
            multipleAlignmentFlag, multipleSignatureBase
    );

    setMITFlag(datasetHeader, false);

    setSequenceId(datasetHeader, 0,0);
    setBlocksInSequence(datasetHeader, 0, (uint32_t)referenceFiles.size());

    setThresholdForSequence(datasetHeader, 0,0);

    setClassType(datasetHeader, 0, 1);


    setNumberDescriptorsInClass(datasetHeader, 0, 1);
    setDescriptorIdInClass(datasetHeader, 0, 0, 0);

    setDatasetHeader(datasetContainer, datasetHeader);

    uint32_t access_unit_ID = 0;

    for (auto &referenceFile : referenceFiles) {
        AccessUnitContainer* accessUnitContainer = initAccessUnitContainer(datasetContainer);

        uint8_t num_blocks = 1;
        uint16_t parameter_set_ID = 0;
        uint8_t au_type = 1;
        uint32_t reads_count = 1;
        uint16_t mm_threshold = 0;
        uint32_t mm_count = 0;

        AccessUnitHeader* accessUnitHeader = initAccessUnitHeaderWithValues(
                datasetContainer,
                access_unit_ID,
                num_blocks,
                parameter_set_ID,
                au_type,
                reads_count,
                mm_threshold,
                mm_count
        );
        setReferenceSequenceId(
                accessUnitHeader,
                1
        );
        setReferenceStartPosition(
                 accessUnitHeader,
                 accessUnitsStarts[access_unit_ID]
        );
        setReferenceEndPosition(
                accessUnitHeader,
                accessUnitsEnds[access_unit_ID]
        );
        setAccessUnitContainerHeader(accessUnitContainer, accessUnitHeader);


        FromFile* fromFile = initFromFileWithFilename(referenceFile.c_str());
        Block* block = initBlock(datasetContainer, fromFile);
        uint8_t descriptorId = 0;
        bool paddingFlag = false;
        uint32_t payloadSize = (uint32_t) getFromFileSize(fromFile);
        BlockHeader* blockHeader = initBlockHeader(
                datasetContainer,
                descriptorId,
                paddingFlag,
                payloadSize
        );
        setBlockHeader(block, blockHeader);
        addBlock(accessUnitContainer, block);

        addAccessUnitToDataset(datasetContainer, accessUnitContainer);
        access_unit_ID++;
    }
    addDatasetsContainer(datasetsGroupContainer, datasetContainer);
}

AccessUnitContainer * AccessUnit::createAccessUnitContainer(DatasetContainer *datasetContainer) {
    AccessUnitContainer* accessUnitContainer = initAccessUnitContainer(datasetContainer);

    AccessUnitHeader* accessUnitHeader = initAccessUnitHeaderWithValues(
        datasetContainer,
        accessUnitId,
        (uint8_t) blocksList.size(),
        parameter_set_id,
        au_type,
        reads_count,
        mm_threshold,
        mm_count
    );


    setAccessUnitContainerHeader(accessUnitContainer, accessUnitHeader);
    for(auto& entryBlockList : blocksList){
        Vector* payloads = initVector();
        uint8_t numberSubstreamsForDescriptor = numberOfDescriptorSubsequences[entryBlockList.first];
        //todo add check for variable cases

        for(uint8_t substreamId=0; substreamId<numberSubstreamsForDescriptor; substreamId++){

            const auto& entryForSubstream = entryBlockList.second.find(substreamId);
            if(entryForSubstream == entryBlockList.second.end()){
                pushBack(payloads, nullptr);
            }else{
                pushBack(payloads, initFromFileWithFilename(entryForSubstream->second.c_str()));
            }
        }

        Block* block = initBlockMultiplePayloads(datasetContainer, payloads);
        BlockHeader* blockHeader = initBlockHeader(
                datasetContainer,
                entryBlockList.first,
                false,
                (uint32_t)getBlockSize(block)
        );
        setBlockHeader(block, blockHeader);
        addBlock(accessUnitContainer, block);
    }

    return accessUnitContainer;
}

AccessUnit::AccessUnit(const std::map<uint8_t, std::map<uint8_t, std::string>> &blocksList, uint32_t accessUnitId,
                       uint16_t parameter_set_id, uint8_t au_type, uint64_t start, uint64_t end) : blocksList(
        blocksList), accessUnitId(accessUnitId), parameter_set_id(parameter_set_id), au_type(au_type), start(start),
                                                                                                   end(end) {}

uint64_t AccessUnit::getStart() const {
    return start;
}

uint64_t AccessUnit::getEnd() const {
    return end;
}


DatasetsGroupReferenceGenome *FakeInternalReference::getReferenceGenome() {
    return referenceGenome;
}

DatasetId FakeInternalReference::getDatasetId() const {
    return datasetId;
}

ReferenceId FakeInternalReference::getReferenceId() const {
    return referenceId;
}

DatasetContainer* Dataset::constructDataset() {
    DatasetContainer* datasetContainer = initDatasetContainer();
    auto numClassesAligned = static_cast<uint8_t>(existing_aligned_classes.size());
    uint8_t numClasses = static_cast<uint8_t>(numClassesAligned + numberUAccessUnits > 0 ? 1 : 0);


    DatasetHeader* datasetHeader = initDatasetHeader(
            datasetGroupId, datasetId, version_DatasetHeader, unmappedIndexingFlag,
            byteOffsetSizeFlag, byteOffsetIsUint64, nonOverlappingAURange, blockHeaderFlag, sequencesCount, referenceId,
            datasetType, numClasses, alphabetId, numUClusters, uSignatureConstantLength, uSignatureSize,
            uSignatureLength, numberUAccessUnits, multipleAlignmentFlag, multipleSignatureBase);

    setMITFlag(datasetHeader, true);


    uint8_t numberSequences = (uint8_t )accessUnitsAligned.size();

    std::map<uint16_t, long> ausPerSequence;
    for (auto &entrySequenceClassAUs : accessUnitsAligned) {
        ausPerSequence[entrySequenceClassAUs.first] = 0;
        for (auto &entryClassAUs : entrySequenceClassAUs.second){
            if(entryClassAUs.second.size() > ausPerSequence[entrySequenceClassAUs.first]){
                ausPerSequence[entrySequenceClassAUs.first] = entryClassAUs.second.size();
            }
        }
    }

    uint16_t sequence_index = 0;
    for (auto &entrySequenceClassAUs : accessUnitsAligned) {
        setSequenceId(datasetHeader, sequence_index, entrySequenceClassAUs.first);

        setBlocksInSequence(datasetHeader, sequence_index, (uint32_t) ausPerSequence[entrySequenceClassAUs.first]);

        setThresholdForSequence(datasetHeader, sequence_index,0);
        sequence_index++;
    }

    uint8_t class_index = 0;
    for(uint8_t class_it=0; class_it<numClasses; class_it++){
        setClassType(datasetHeader, class_index, existing_aligned_classes[class_index]);
    }

    for(uint8_t i=0;i<numClasses; i++){
        setNumberDescriptorsInClass(datasetHeader, i,(uint8_t) descriptorsIdPerClass[existing_aligned_classes[i]].size());
    }
    for(uint8_t classIndex=0;classIndex<numClasses; classIndex++){
        for(uint8_t descriptor_i=0; descriptor_i<descriptorsIdPerClass[existing_aligned_classes[classIndex]].size(); descriptor_i++){
            setDescriptorIdInClass(
                    datasetHeader,
                    classIndex,
                    descriptor_i,
                    descriptorsIdPerClass[existing_aligned_classes[classIndex]][descriptor_i]
            );
        }
    }
    setDatasetHeader(datasetContainer, datasetHeader);

    uint64_t currentOffset =
            getSizeDatasetHeader(datasetHeader);

    Vector* datasetParametersVector = initVector();
    for(auto& paramFilename : parametersFilenames) {
        DatasetParameters *datasetParameters = initDatasetParameters();
        defineContentDatasetParameters(datasetParameters, paramFilename.c_str());
        pushBack(datasetParametersVector, datasetParameters);

        currentOffset += getSizeDatasetParameters(datasetParameters);
    }

    setDatasetParameters(datasetContainer, datasetParametersVector);

    DatasetMasterIndexTable* datasetMasterIndexTable = initDatasetMasterIndexTable(datasetContainer);
    setDatasetMasterIndexTable(datasetContainer, datasetMasterIndexTable);

    sequence_index = 0;

    currentOffset += getSizeDatasetMasterIndexTable(datasetMasterIndexTable);

    for(auto &entrySequenceClassAUs : accessUnitsAligned){
        for(uint8_t class_i=0; class_i<numClassesAligned; class_i++){
            Class_type classType = existing_aligned_classes[class_i];
            auto entryClassAccessUnits = entrySequenceClassAUs.second.find(classType);
            if(entryClassAccessUnits != entrySequenceClassAUs.second.end()){
                auto AUstoAdd = (uint8_t) entryClassAccessUnits->second.size();
                for(uint8_t au_i=0; au_i<AUstoAdd; au_i++){
                    AccessUnit & accessUnit = entryClassAccessUnits->second[au_i];
                    uint64_t start = accessUnit.getStart();
                    uint64_t end = accessUnit.getEnd();
                    setStartEndAndOffset(
                            datasetMasterIndexTable,
                            sequence_index,
                            class_i,
                            au_i,
                            start,
                            end,
                            currentOffset
                    );
                    AccessUnitContainer* accessUnitContainer = accessUnit.createAccessUnitContainer(datasetContainer);
                    addAccessUnitToDataset(datasetContainer, accessUnitContainer);
                    currentOffset += getAccessUnitContainerSize(accessUnitContainer);
                }
            }
        }
        sequence_index++;
    }

    uint32_t uAcessUnit_id = 0;
    for(auto & uAccessUnit : accessUnitsUnaligned){
        AccessUnitContainer* accessUnitContainer = uAccessUnit.createAccessUnitContainer(datasetContainer);
        setUnalignedOffset(datasetMasterIndexTable, uAcessUnit_id, currentOffset);
        addAccessUnitToDataset(datasetContainer, accessUnitContainer);
        currentOffset += getAccessUnitContainerSize(accessUnitContainer);
        uAcessUnit_id++;
    }

    return datasetContainer;
}

Dataset::Dataset(const std::vector<Class_type> &existing_aligned_classes,
                 const std::map<Class_type, std::vector<uint8_t>> &descriptorsIdPerClass,
                 const std::map<uint16_t, std::map<Class_type, std::vector<AccessUnit>>> &accessUnitsAligned,
                 const std::vector<AccessUnit> &accessUnitsUnaligned, DatasetGroupId datasetGroupId,
                 DatasetId datasetId,
                 const std::vector<std::string>& parametersFilenames
                 ) : existing_aligned_classes(existing_aligned_classes),
                                        descriptorsIdPerClass(descriptorsIdPerClass),
                                        accessUnitsAligned(accessUnitsAligned),
                                        accessUnitsUnaligned(accessUnitsUnaligned),
                                       datasetGroupId(datasetGroupId),
                                       datasetId(datasetId),
                                       parametersFilenames(parametersFilenames){
    numberUAccessUnits = (uint32_t) accessUnitsUnaligned.size();
}

void Dataset::addAsDatasetToDatasetGroup(
        DatasetsGroupContainer* datasetsGroupContainer
){
    DatasetContainer* datasetContainer = constructDataset();
    addDatasetsContainer(datasetsGroupContainer,datasetContainer);


}

FakeInternalReference DatasetGroup::addFakeInternalReference(
        std::string referenceURI,
        std::string sequenceName,
        const std::vector<std::string> & dataFiles,
        const std::vector<uint64_t>& accessUnitsStarts,
        const std::vector<uint64_t>& accessUnitsEnds
){
    FakeInternalReference fakeInternalReference(
        referenceURI,
        sequenceName,
        id,
        datasetsCreated,
        referenceId,
        dataFiles,
        accessUnitsStarts,
        accessUnitsEnds
    );
    datasetsCreated++;
    referenceId++;


    fakeInternalReference.addAsDatasetToDatasetGroup(getDatasetsGroupContainer(), id);
    Vector* referenceGenomeVector = initVector();
    pushBack(referenceGenomeVector, fakeInternalReference.getReferenceGenome());
    setDatasetsGroupReferenceGenomes(datasetsGroupContainer, referenceGenomeVector);
    return fakeInternalReference;
}

Dataset DatasetGroup::addDatasetData(
        const std::vector<Class_type> &existing_aligned_classes,
        const std::map<Class_type, std::vector<uint8_t>> &descriptorsIdPerClass,
        const std::map<uint16_t, std::map<Class_type, std::vector<AccessUnit>>> &accessUnitsAligned,
        const std::vector<AccessUnit> &accessUnitsUnaligned,
        const std::vector<std::string> &parametersFilename
) {
    Dataset dataset(
            existing_aligned_classes,
            descriptorsIdPerClass,
            accessUnitsAligned,
            accessUnitsUnaligned,
            id,
            datasetsCreated,
            parametersFilename
    );
    datasetsCreated++;

    dataset.addAsDatasetToDatasetGroup(datasetsGroupContainer);
    return dataset;
}


bool MPEGGFileCreator::write(const std::string &filename) {
    if(isFileValid(file)){
        FILE* outputFile = fopen(filename.c_str(),"wb");
        if(!writeFile(file,outputFile)){
            printf("Unsuccessful write.\n");
            return false;
        };
        fclose(outputFile);
        return true;
    }else{
        fprintf(stderr,"file is not valid.\n");
        return false;
    }
}

InternalReference::InternalReference(
        const std::string &referenceURI,
        const std::string &sequenceName,
        DatasetGroupId datasetGroupId,
        DatasetId  datasetId,
        ReferenceId referenceId,
        const generated_aus_ref & generatedAusRef
): accessUnitsStarts(accessUnitsStarts), accessUnitsEnds(accessUnitsEnds) {
    char* refURI = (char*)calloc(100,sizeof(char));
    referenceURI.copy(refURI,99);
    InternalReference::datasetId = datasetId;
    InternalReference::referenceId = referenceId;


    referenceGenome = initDatasetsGroupReferenceGenome(
            datasetGroupId,
            datasetId,
            referenceId,
            refURI,
            1,
            2,
            3
    );
    resizeSequences(referenceGenome, 1);
    char* sequenceNameBuffer = (char*)calloc(100,sizeof(char));
    sequenceName.copy(sequenceNameBuffer,99);
    setSequenceName(referenceGenome,0, sequenceNameBuffer);

    InternalReference::referenceFiles = referenceFiles;
}

void InternalReference::addAsDatasetToDatasetGroup(
        DatasetsGroupContainer* datasetsGroupContainer,
        DatasetGroupId datasetGroupId
){
    DatasetContainer* datasetContainer = initDatasetContainer();


    DatasetId datasetId = InternalReference::datasetId;
    char version[]="1900";
    bool unmapped_indexing_flag = false;
    bool byteOffsetSizeFlag = false;
    bool posOffsetIsUint40 = false;
    bool nonOverlappingAURange = true;
    bool blockHeaderFlag = true;
    uint16_t sequencesCount = 0;
    ReferenceId referenceId = InternalReference::referenceId;
    uint8_t datasetType = 2;
    uint8_t numClasses = 1;
    uint8_t alphabetId = 0;
    uint32_t numUClusters = 0;
    bool uSignatureConstantLength = true;
    uint8_t uSignatureSize = 0;
    uint8_t uSignatureLength = 0;
    auto numberUAccessUnits = (uint32_t) referenceFiles.size();
    bool multipleAlignmentFlag = false;
    uint32_t multipleSignatureBase = 0;

    DatasetHeader* datasetHeader = initDatasetHeader(
            datasetGroupId, datasetId, version, unmapped_indexing_flag, byteOffsetSizeFlag, posOffsetIsUint40,
            nonOverlappingAURange, blockHeaderFlag, sequencesCount, referenceId, datasetType, numClasses, alphabetId,
            numUClusters, uSignatureConstantLength, uSignatureSize, uSignatureLength, numberUAccessUnits,
            multipleAlignmentFlag, multipleSignatureBase
    );

    setMITFlag(datasetHeader, false);


    setThresholdForSequence(datasetHeader, 0,0);

    std::set<uint8_t> existingDescriptors;
    for(const auto &descriptorMapInAU : referenceFiles){
        for(const auto &subdescriptorInDescriptorMap : descriptorMapInAU){
            existingDescriptors.insert(subdescriptorInDescriptorMap.first);
        }
    }
    setNumberDescriptorsInClass(datasetHeader, 0, static_cast<uint8_t>(existingDescriptors.size()));

    uint8_t descriptorIndex = 0;
    for(uint8_t descriptorId : existingDescriptors){
        setDescriptorIdInClass(datasetHeader, 0, descriptorIndex, descriptorId);
        descriptorIndex++;
    }

    setDatasetHeader(datasetContainer, datasetHeader);

    uint32_t access_unit_ID = 0;

    for (auto & filesInAu : referenceFiles) {
        AccessUnitContainer* accessUnitContainer = initAccessUnitContainer(datasetContainer);

        uint8_t num_blocks = 1;
        uint16_t parameter_set_ID = 0;
        uint8_t au_type = 1;
        uint32_t reads_count = 1;
        uint16_t mm_threshold = 0;
        uint32_t mm_count = 0;

        AccessUnitHeader* accessUnitHeader = initAccessUnitHeaderWithValues(
                datasetContainer,
                access_unit_ID,
                num_blocks,
                parameter_set_ID,
                au_type,
                reads_count,
                mm_threshold,
                mm_count
        );
        setReferenceSequenceId(
                accessUnitHeader,
                1
        );
        setReferenceStartPosition(
                accessUnitHeader,
                accessUnitsStarts[access_unit_ID]
        );
        setReferenceEndPosition(
                accessUnitHeader,
                accessUnitsEnds[access_unit_ID]
        );
        setAccessUnitContainerHeader(accessUnitContainer, accessUnitHeader);

        for(const auto& filesInDescriptorInAu : filesInAu){
            uint8_t descriptorId = filesInDescriptorInAu.first;

            Vector* subsequencesDescriptor = initVector();
            uint32_t payloadSize = 0;

            uint8_t expectedNumberOfDescriptorSubsequences = numberOfDescriptorSubsequences[descriptorId];
            //todo check if expectedNumberOfDescriptorSubsequences is 0, if it is correct

            for(
                uint8_t descriptorSubsequence_index = 0;
                descriptorSubsequence_index<expectedNumberOfDescriptorSubsequences;
                descriptorSubsequence_index++
            ){
                if(descriptorSubsequence_index != expectedNumberOfDescriptorSubsequences-1){
                    payloadSize += 4;
                }
                auto entryForDescriptorSubsequence = filesInDescriptorInAu.second.find(descriptorSubsequence_index);
                if(
                    entryForDescriptorSubsequence != filesInDescriptorInAu.second.end()
                ){
                    FromFile* subsequencePayload =
                            initFromFileWithFilename(entryForDescriptorSubsequence->second.c_str());
                    payloadSize += getFromFileSize(subsequencePayload);
                    pushBack(subsequencesDescriptor, subsequencePayload);
                }else{
                    pushBack(subsequencesDescriptor, nullptr);
                }
            }

            Block* block = initBlockMultiplePayloads(datasetContainer, subsequencesDescriptor);


            bool paddingFlag = false;

            BlockHeader* blockHeader = initBlockHeader(
                    datasetContainer,
                    descriptorId,
                    paddingFlag,
                    payloadSize
            );
            setBlockHeader(block, blockHeader);

            addBlock(accessUnitContainer, block);

        }


        addAccessUnitToDataset(datasetContainer, accessUnitContainer);
        access_unit_ID++;
    }
    addDatasetsContainer(datasetsGroupContainer, datasetContainer);
}


DatasetsGroupReferenceGenome *InternalReference::getReferenceGenome() {
    return referenceGenome;
}

DatasetId InternalReference::getDatasetId() const {
    return datasetId;
}

ReferenceId InternalReference::getReferenceId() const {
    return referenceId;
}

InternalReference DatasetGroup::addInternalReference(
        std::string referenceURI,
        std::string sequenceName,
        const generated_aus_ref & generatedAusRef
){
    InternalReference internalReference(
            referenceURI,
            sequenceName,
            id,
            datasetsCreated,
            referenceId,
            generatedAusRef
    );
    datasetsCreated++;
    referenceId++;


    internalReference.addAsDatasetToDatasetGroup(getDatasetsGroupContainer(), id);
    Vector* referenceGenomeVector = initVector();
    pushBack(referenceGenomeVector, internalReference.getReferenceGenome());
    setDatasetsGroupReferenceGenomes(datasetsGroupContainer, referenceGenomeVector);
    return internalReference;
}