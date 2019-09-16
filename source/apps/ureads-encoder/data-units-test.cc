#include <DataUnits/DataUnits.h>
#include <gtest/gtest.h>

class dataUnitsTest : public ::testing::Test {
   protected:
    virtual void SetUp() {
        // Code here will be called immediately before each test
    }

    virtual void TearDown() {
        // Code here will be called immediately after each test
    }
};

TEST_F(dataUnitsTest, constructEncodingParametersSingleAlignmentNoComputedTest) {
    DataUnits* dataUnits = initDataUnits();

    // change to something meaningful
    Encoding_ParametersType* encodingParameters = NULL;
    // Since Marrakech unclear what the parent does. Here we give the same id, therefore the parameters are top of the
    // line.
    uint16_t parent_parameter_setId = 1;
    uint16_t parameter_setId = 1;
    DataUnitParametersSet* dataUnitParametersSet =
        initParametersSet(parent_parameter_setId, parameter_setId, encodingParameters);
    addDataUnitParameters(dataUnits, dataUnitParametersSet);

    uint32_t accessUnitId = 0;
    uint8_t numBlocks = 1;  // Straightforward: the number of blocks (that we have still to add to the dataUnit)
    ClassType AU_type;
    AU_type.classType = CLASS_TYPE_CLASS_U;  // do not remember why I had to wrap the enum in a struct
    uint32_t readsCount = 500;
    uint16_t mmThreshold = 0;  // this argument and the next are only important for access units N or M
    uint32_t mmCount = 0;
    SequenceID referenceSequenceId;  // this argument and the next two are only important if dataset is reference
    uint64_t referenceStartPos = 0;
    uint64_t referenceEndPos = 0;
    SequenceID sequenceId;  // remaining variables are only important if access unit is mapped
    uint64_t auStartPosition;
    uint64_t auEndPosition;
    uint64_t extendedAUStartPosition;
    uint64_t extendedAUEndPosition;

    DataUnitAccessUnit* dataUnitAccessUnit =
        initDataUnitAccessUnit(accessUnitId, numBlocks, parameter_setId, AU_type, readsCount, mmThreshold, mmCount,
                               referenceSequenceId, referenceStartPos, referenceEndPos, sequenceId, auStartPosition,
                               auEndPosition, extendedAUStartPosition, extendedAUEndPosition);

    uint8_t descriptorId = 0;
    uint32_t payloadSize = 5;
    char* payloadInMemory = (char*)calloc(payloadSize, sizeof(char));
    Block* block = initBlockWithHeaderPayloadInMemory(descriptorId, payloadSize, payloadInMemory);
    DataUnitBlockHeader* dataUnitBlockHeader = initDataUnitBlockHeader(descriptorId, payloadSize);
    addBlockToDataUnitAccessUnit(dataUnitAccessUnit, block, dataUnitBlockHeader);
}