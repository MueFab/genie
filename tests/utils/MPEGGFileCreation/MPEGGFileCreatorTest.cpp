#include <utils/MPEGGFileCreation/MPEGGFileCreator.h>
#include "gtest/gtest.h"
#include <fstream>

extern "C" {
#include "format/EncodingParameters.h"
}

class mpeggFileCreatorTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        FILE* tmpRefA = fopen("testRefA", "w");
        fprintf(tmpRefA,"ACTG");
        fclose(tmpRefA);

        FILE* tmpRefB = fopen("testRefB", "w");
        fprintf(tmpRefB,"GTCA");
        fclose(tmpRefB);

        FILE* fakePayload1 = fopen("fakePayload1", "w");
        fprintf(fakePayload1,"fakePayload1");
        fclose(fakePayload1);

        FILE* fakePayload2 = fopen("fakePayload2", "w");
        fprintf(fakePayload2,"fakePayload2");
        fclose(fakePayload2);
    }

    virtual void TearDown()
    {
        remove("testRefA");
        remove("testRefB");
        remove("fakePayload1");
        remove("fakePayload2");
    }
};


TEST_F(mpeggFileCreatorTest, transformSubseqParameters)
{

    MPEGGFileCreator fileCreator;
    DatasetGroup* datasetGroup1 = fileCreator.addDatasetGroup();
    std::vector<std::string> dataFiles;
    dataFiles.emplace_back("testRefA");
    dataFiles.emplace_back("testRefB");
    std::vector<uint64_t> startRefPosition;
    std::vector<uint64_t> endRefPosition;
    startRefPosition.push_back(0);
    endRefPosition.push_back(4);
    startRefPosition.push_back(4);
    endRefPosition.push_back(8);
    datasetGroup1->addFakeInternalReference("ref1","seq1",dataFiles, startRefPosition, endRefPosition);



    std::vector<Class_type> existing_classes;
    existing_classes.push_back(CLASS_P);
    existing_classes.push_back(CLASS_M);

    std::map<Class_type, std::vector<uint8_t>> descriptorsIdPerClass;
    descriptorsIdPerClass[CLASS_P].push_back(0);
    descriptorsIdPerClass[CLASS_P].push_back(1);
    descriptorsIdPerClass[CLASS_M].push_back(0);
    descriptorsIdPerClass[CLASS_M].push_back(1);

    std::map<uint16_t, std::map<Class_type, std::vector<AccessUnit>>> accessUnitsAligned;
    accessUnitsAligned[0][CLASS_P].push_back(AccessUnit(
            {{0,"fakePayload1"},{1,"fakePayload2"}},
            0,
            0,
            CLASS_P,
            10005,
            20005
            ));
    accessUnitsAligned[0][CLASS_P].push_back(AccessUnit(
            {{0,"fakePayload1"},{1,"fakePayload2"}},
            1,
            0,
            CLASS_P,
            20005,
            30005
    ));
    accessUnitsAligned[0][CLASS_M].push_back(AccessUnit(
            {{0,"fakePayload1"},{1,"fakePayload2"}},
            0,
            0,
            CLASS_P,
            10005,
            20005
    ));
    accessUnitsAligned[0][CLASS_M].push_back(AccessUnit(
            {{0,"fakePayload1"},{1,"fakePayload2"}},
            1,
            0,
            CLASS_P,
            20005,
            30005
    ));
    std::vector<AccessUnit> accessUnitsUnaligned;
    accessUnitsUnaligned.push_back(AccessUnit(
            {{0,"fakePayload1"},{1,"fakePayload2"}},
            0,
            0,
            CLASS_U,
            0,
            0
    ));
    accessUnitsUnaligned.push_back(AccessUnit(
            {{0,"fakePayload1"},{1,"fakePayload2"}},
            1,
            0,
            CLASS_U,
            0,
            0
    ));

    std::vector<std::string> parametersFilenames = {"fakePayload1"};



    datasetGroup1->addDatasetData(
            existing_classes,
            descriptorsIdPerClass,
            accessUnitsAligned,
            accessUnitsUnaligned,
            parametersFilenames
    );


    fileCreator.write("testOutput0");

}