//
// Created by daniel on 1/11/18.
//

#ifndef GENIE_MASTERINDEXTABLECREATOR_H
#define GENIE_MASTERINDEXTABLECREATOR_H
extern "C"{
#include "format/Boxes.h"
}

#include <set>
#include <vector>
#include <map>
#include <string>
#include "coding/ReturnStructures.h"


class FakeInternalReference{
private:
    DatasetsGroupReferenceGenome* referenceGenome;
    DatasetId datasetId;
    ReferenceId referenceId;
    std::vector<std::string> referenceFiles;
    std::vector<uint64_t> accessUnitsStarts;
    std::vector<uint64_t> accessUnitsEnds;
public:
    DatasetsGroupReferenceGenome *getReferenceGenome();
    FakeInternalReference(
            const std::string &referenceURI,
            const std::string &sequenceName,
            DatasetGroupId datasetGroupId,
            DatasetId  datasetId,
            ReferenceId referenceId,
            const std::vector<std::string>& referenceFiles,
            const std::vector<uint64_t>& accessUnitsStarts,
            const std::vector<uint64_t>& accessUnitsEnds
    );
    DatasetId getDatasetId() const;
    ReferenceId getReferenceId() const;
    void addAsDatasetToDatasetGroup(
            DatasetsGroupContainer* datasetsGroupContainer,
            DatasetGroupId datasetGroupId
    );
};

class InternalReference{
private:
    DatasetsGroupReferenceGenome* referenceGenome;
    DatasetId datasetId;
    ReferenceId referenceId;
    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> referenceFiles;
    std::vector<uint64_t> accessUnitsStarts;
    std::vector<uint64_t> accessUnitsEnds;
public:
    DatasetsGroupReferenceGenome *getReferenceGenome();
    InternalReference(
            const std::string &referenceURI,
            const std::string &sequenceName,
            DatasetGroupId datasetGroupId,
            DatasetId  datasetId,
            ReferenceId referenceId,
            const generated_aus_ref & generatedAusRef
    );
    DatasetId getDatasetId() const;
    ReferenceId getReferenceId() const;
    void addAsDatasetToDatasetGroup(
            DatasetsGroupContainer* datasetsGroupContainer,
            DatasetGroupId datasetGroupId
    );
};

class AccessUnit{
private:
    std::map<uint8_t, std::map<uint8_t, std::string>> blocksList;
    uint32_t accessUnitId;
    uint16_t parameter_set_id;
    uint8_t au_type;
    uint32_t reads_count = 0;
    uint16_t mm_threshold = 0;
    uint32_t mm_count = 0;
    uint64_t start;
    uint64_t end;
public:
    AccessUnit(const std::map<uint8_t, std::map<uint8_t, std::string>> &blocksList, uint32_t accessUnitId,
               uint16_t parameter_set_id,
               uint8_t au_type, uint64_t start, uint64_t end);

    uint64_t getStart() const;
    uint64_t getEnd() const;
    AccessUnitContainer *createAccessUnitContainer(DatasetContainer *datasetContainer);
};

class Dataset{
private:
    std::vector<Class_type> existing_aligned_classes;
    std::map<Class_type, std::vector<uint8_t >> descriptorsIdPerClass;
    std::map<uint16_t , std::map<Class_type, std::vector<AccessUnit> > > accessUnitsAligned;
    std::vector<AccessUnit> accessUnitsUnaligned;
    DatasetGroupId datasetGroupId;
    DatasetId datasetId = 1;
    uint16_t majorBrand_DatasetHeader = 1;
    uint8_t uSignatureSize=0;
    uint32_t numberUAccessUnits;
    uint32_t multipleSignatureBase=0;
    char version_DatasetHeader[5] = "1900";
    bool unmappedIndexingFlag = true;
    bool byteOffsetSizeFlag = false;
    bool byteOffsetIsUint64 = true;
    bool nonOverlappingAURange = false;
    bool blockHeaderFlag = true;
    uint16_t sequencesCount = 5;
    ReferenceId referenceId = 1;
    uint8_t datasetType = 1;
    uint8_t alphabetId = 1;
    uint32_t numUClusters = 4;
    bool uSignatureConstantLength = true;
    uint8_t uSignatureLength = 0;
    bool multipleAlignmentFlag = false;
    std::vector<std::string> parametersFilenames;
public:
    Dataset(const std::vector<Class_type> &existing_aligned_classes,
                const std::map<Class_type, std::vector<uint8_t>> &descriptorsIdPerClass,
                const std::map<uint16_t, std::map<Class_type, std::vector<AccessUnit>>> &accessUnitsAligned,
                const std::vector<AccessUnit> &accessUnitsUnaligned, DatasetGroupId datasetGroupId,
                DatasetId datasetId, const std::vector<std::string> & parametersFilenames);

    DatasetContainer* constructDataset();
    void addAsDatasetToDatasetGroup(
            DatasetsGroupContainer* datasetsGroupContainer
    );
};

class DatasetGroup{
private:
    DatasetGroupId id;
    DatasetId datasetsCreated = 0;
    ReferenceId referenceId = 0;
    DatasetsGroupContainer* datasetsGroupContainer;
public:
    DatasetsGroupContainer *getDatasetsGroupContainer() const;
    explicit DatasetGroup(DatasetGroupId datasetGroupId);
    FakeInternalReference addFakeInternalReference(
            std::string referenceURI,
            std::string sequenceName,
            const std::vector<std::string> & dataFiles,
            const std::vector<uint64_t>& accessUnitsStarts,
            const std::vector<uint64_t>& accessUnitsEnds
    );
    InternalReference addInternalReference(
            std::string referenceURI,
            std::string sequenceName,
            const generated_aus_ref & generatedAusRef
    );
    Dataset addDatasetData(
            const std::vector<Class_type> &existing_aligned_classes,
            const std::map<Class_type, std::vector<uint8_t>> &descriptorsIdPerClass,
            const std::map<uint16_t, std::map<Class_type, std::vector<AccessUnit>>> &accessUnitsAligned,
            const std::vector<AccessUnit> &accessUnitsUnaligned,
            const std::vector<std::string> &parametersFilename
    );


};

class MPEGGFileCreator {
private:
    File* file = NULL;
    DatasetGroupId datasetsGroupCreated = 0;
public:
    MPEGGFileCreator();
    DatasetGroup * addDatasetGroup();
    bool write(const std::string& filename);
};


#endif //GENIE_MASTERINDEXTABLECREATOR_H
