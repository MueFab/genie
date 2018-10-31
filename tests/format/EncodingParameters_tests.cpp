#include "gtest/gtest.h"
#include "format/EncodingParameters_tests.h"

class encoding_parameters_test : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        // Code here will be called immediately before each test
    }

    virtual void TearDown()
    {
        // Code here will be called immediately after each test
    }
};


TEST_F(encoding_parameters_test, transformSubseqParameters)
{
    Transform_subseq_parametersType* transformSubseqParameters;
    TransformSubSeqIdEnum transformSubSeqId;
    uint16_t expectedMatchCoding = 512;
    uint16_t matchCoding;
    uint8_t expectedRleCodingGuard = 3;
    uint8_t rleCodingGuard;
    EncodingParametersRC responseCall;

    transformSubseqParameters = constructNoTransformSubseqParameters();
    responseCall = getTransformSubSeqId(transformSubseqParameters, &transformSubSeqId);
    EXPECT_EQ(responseCall, SUCCESS);
    EXPECT_EQ(transformSubSeqId, SubSeq_NO_TRANSFORM);

    responseCall = getMatchCodingBufferSize(transformSubseqParameters, &matchCoding);
    EXPECT_EQ(responseCall, FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    responseCall = getRLECodingGuard(transformSubseqParameters, &rleCodingGuard);
    EXPECT_EQ(responseCall, FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    transformSubseqParameters = constructEqualityCodingTransformSubseqParameters();
    responseCall = getTransformSubSeqId(transformSubseqParameters, &transformSubSeqId);
    EXPECT_EQ(responseCall, SUCCESS);
    EXPECT_EQ(transformSubSeqId, SubSeq_EQUALITY_CODING);

    responseCall = getMatchCodingBufferSize(transformSubseqParameters, &matchCoding);
    EXPECT_EQ(responseCall, FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    responseCall = getRLECodingGuard(transformSubseqParameters, &rleCodingGuard);
    EXPECT_EQ(responseCall, FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    transformSubseqParameters = constructMatchCodingSubseqTransformSubseqParameters(expectedMatchCoding);
    responseCall = getTransformSubSeqId(transformSubseqParameters, &transformSubSeqId);
    EXPECT_EQ(responseCall, SUCCESS);
    EXPECT_EQ(transformSubSeqId, SubSeq_MATCH_CODING);

    responseCall = getMatchCodingBufferSize(transformSubseqParameters, &matchCoding);
    EXPECT_EQ(responseCall, SUCCESS);
    EXPECT_EQ(matchCoding, expectedMatchCoding);

    responseCall = getRLECodingGuard(transformSubseqParameters, &rleCodingGuard);
    EXPECT_EQ(responseCall, FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    transformSubseqParameters = constructRLECodingSubseqTransformSubseqParameters(expectedRleCodingGuard);
    responseCall = getTransformSubSeqId(transformSubseqParameters, &transformSubSeqId);
    EXPECT_EQ(responseCall, SUCCESS);
    EXPECT_EQ(transformSubSeqId, SubSeq_RLE_CODING);

    responseCall = getMatchCodingBufferSize(transformSubseqParameters, &matchCoding);
    EXPECT_EQ(responseCall, FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_EQ(matchCoding, expectedMatchCoding);

    responseCall = getRLECodingGuard(transformSubseqParameters, &rleCodingGuard);
    EXPECT_EQ(responseCall , SUCCESS);
    EXPECT_EQ(rleCodingGuard , expectedRleCodingGuard);

    transformSubseqParameters = constructRLEQVCodingSubseqTransformSubseqParameters();
    responseCall = getTransformSubSeqId(transformSubseqParameters, &transformSubSeqId);
    EXPECT_EQ(responseCall , SUCCESS);
    EXPECT_EQ(transformSubSeqId , SubSeq_RLE_QV_CODING);

    responseCall = getMatchCodingBufferSize(transformSubseqParameters, &matchCoding);
    EXPECT_EQ(responseCall , FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    responseCall = getRLECodingGuard(transformSubseqParameters, &rleCodingGuard);
    EXPECT_EQ(responseCall , FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
}

