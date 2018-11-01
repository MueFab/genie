#include "gtest/gtest.h"

extern "C" {
#include "format/EncodingParameters.h"
}

class encodingParametersTest : public ::testing::Test
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


TEST_F(encodingParametersTest, transformSubseqParameters)
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

TEST_F(encodingParametersTest, binarization)
{
    uint8_t binarization_ID;
    bool bypassFlag;
    bool expectedBIFUllCtxMode = true;
    bool BIFullCtxMode;
    uint8_t expectedCTruncExpGolParam = 1;
    uint8_t cTruncExpGolParam;
    uint8_t expectedCMaxDTU = 2;
    uint8_t cMaxDTU;
    uint8_t expectedSplitUnitSize = 4;
    uint8_t splitUnitSize;
    bool expectedAdaptive_mode_flag = true;
    bool adaptiveModeFlag;
    uint16_t expectedNumContexts = 3;
    uint16_t numContexts;
    uint8_t  expectedContextInitialization[] ={1,2,3};
    uint8_t* contextInitialization;
    bool expectedShareSubSymCtxFlag = true;
    bool shareSubSymCtxFlag;
    bool expectedSegmentCtxFlag = true;
    bool segmentCtxFlag;
    uint16_t expectedSegmentCtxLen = 15;
    uint16_t segmentCtxLen;
    uint16_t expectedSegmentCtxOffset = 33;
    uint16_t segmentCtxOffset;
    Cabac_context_parametersType* cabacContextParameters;


    Cabac_binarizationsType* cabacBinarization = constructCabacBinarizationBinaryCoding_NotBypass(
            expectedBIFUllCtxMode,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag,
            expectedSegmentCtxFlag,
            expectedSegmentCtxLen,
            expectedSegmentCtxOffset
    );
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_BinaryCoding);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == false);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == SUCCESS);
    EXPECT_TRUE(BIFullCtxMode == expectedBIFUllCtxMode);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(cabacContextParameters != NULL);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters,&numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);

    cabacBinarization = constructCabacBinarizationBinaryCoding_Bypass(
            expectedBIFUllCtxMode
    );
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_BinaryCoding);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == true);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == SUCCESS);
    EXPECT_TRUE(BIFullCtxMode == expectedBIFUllCtxMode);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);


    cabacBinarization = constructCabacBinarizationTruncatedUnary_NotBypass(
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag,
            expectedSegmentCtxFlag,
            expectedSegmentCtxLen,
            expectedSegmentCtxOffset
    );
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_TruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == false);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(cabacContextParameters != NULL);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters,&numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);

    cabacBinarization = constructCabacBinarizationTruncatedUnary_Bypass();
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_TruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == true);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    cabacBinarization = constructCabacBinarizationExponentialGolomb_NotBypass(
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag,
            expectedSegmentCtxFlag,
            expectedSegmentCtxLen,
            expectedSegmentCtxOffset
    );
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_ExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == false);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters, &numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);
    EXPECT_TRUE(getSegmentCtxFlag(cabacContextParameters, &segmentCtxFlag)==SUCCESS);
    EXPECT_TRUE(segmentCtxFlag == expectedSegmentCtxFlag);
    EXPECT_TRUE(getSegmentCtxLen(cabacContextParameters, &segmentCtxLen)==SUCCESS);
    EXPECT_TRUE(segmentCtxLen == expectedSegmentCtxLen);
    EXPECT_TRUE(getSegmentCtxOffset(cabacContextParameters, &segmentCtxOffset) == SUCCESS);
    EXPECT_TRUE(segmentCtxOffset == expectedSegmentCtxOffset);


    cabacBinarization = constructCabacBinarizationExponentialGolomb_Bypass();
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_ExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == true);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    cabacBinarization = constructCabacBinarizationSignedExponentialGolomb_NotBypass(
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag,
            expectedSegmentCtxFlag,
            expectedSegmentCtxLen,
            expectedSegmentCtxOffset
    );
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == false);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters, &numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);
    EXPECT_TRUE(getSegmentCtxFlag(cabacContextParameters, &segmentCtxFlag)==SUCCESS);
    EXPECT_TRUE(segmentCtxFlag == expectedSegmentCtxFlag);
    EXPECT_TRUE(getSegmentCtxLen(cabacContextParameters, &segmentCtxLen)==SUCCESS);
    EXPECT_TRUE(segmentCtxLen == expectedSegmentCtxLen);
    EXPECT_TRUE(getSegmentCtxOffset(cabacContextParameters, &segmentCtxOffset) == SUCCESS);
    EXPECT_TRUE(segmentCtxOffset == expectedSegmentCtxOffset);

    cabacBinarization = constructCabacBinarizationSignedExponentialGolomb_Bypass();
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == true);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    cabacBinarization = constructCabacBinarizationTruncatedExponentialGolomb_NotBypass(
            expectedCTruncExpGolParam,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag,
            expectedSegmentCtxFlag,
            expectedSegmentCtxLen,
            expectedSegmentCtxOffset
    );
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_TruncatedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == false);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == SUCCESS);
    EXPECT_TRUE(cTruncExpGolParam == expectedCTruncExpGolParam);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters, &numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);
    EXPECT_TRUE(getSegmentCtxFlag(cabacContextParameters, &segmentCtxFlag)==SUCCESS);
    EXPECT_TRUE(segmentCtxFlag == expectedSegmentCtxFlag);
    EXPECT_TRUE(getSegmentCtxLen(cabacContextParameters, &segmentCtxLen)==SUCCESS);
    EXPECT_TRUE(segmentCtxLen == expectedSegmentCtxLen);
    EXPECT_TRUE(getSegmentCtxOffset(cabacContextParameters, &segmentCtxOffset) == SUCCESS);
    EXPECT_TRUE(segmentCtxOffset == expectedSegmentCtxOffset);


    cabacBinarization =  constructCabacBinarizationTruncatedExponentialGolomb_Bypass(cTruncExpGolParam);
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_TruncatedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == true);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == SUCCESS);
    EXPECT_TRUE(expectedCTruncExpGolParam == cTruncExpGolParam);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    cabacBinarization = constructCabacBinarizationSignedTruncatedExponentialGolomb_NotBypass(
            expectedCTruncExpGolParam,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag,
            expectedSegmentCtxFlag,
            expectedSegmentCtxLen,
            expectedSegmentCtxOffset
    );
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedTruncatedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == false);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == SUCCESS);
    EXPECT_TRUE(cTruncExpGolParam == expectedCTruncExpGolParam);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters, &numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);
    EXPECT_TRUE(getSegmentCtxFlag(cabacContextParameters, &segmentCtxFlag)==SUCCESS);
    EXPECT_TRUE(segmentCtxFlag == expectedSegmentCtxFlag);
    EXPECT_TRUE(getSegmentCtxLen(cabacContextParameters, &segmentCtxLen)==SUCCESS);
    EXPECT_TRUE(segmentCtxLen == expectedSegmentCtxLen);
    EXPECT_TRUE(getSegmentCtxOffset(cabacContextParameters, &segmentCtxOffset) == SUCCESS);
    EXPECT_TRUE(segmentCtxOffset == expectedSegmentCtxOffset);

    cabacBinarization =  constructCabacBinarizationSignedTruncatedExponentialGolomb_Bypass(cTruncExpGolParam);
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedTruncatedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == true);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == SUCCESS);
    EXPECT_TRUE(expectedCTruncExpGolParam == cTruncExpGolParam);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    cabacBinarization = constructCabacBinarizationSplitUnitwiseTruncatedUnary_NotBypass(
            expectedSplitUnitSize,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag,
            expectedSegmentCtxFlag,
            expectedSegmentCtxLen,
            expectedSegmentCtxOffset
    );
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SplitUnitWiseTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == false);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == SUCCESS);
    EXPECT_TRUE(expectedSplitUnitSize == splitUnitSize);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters, &numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);
    EXPECT_TRUE(getSegmentCtxFlag(cabacContextParameters, &segmentCtxFlag)==SUCCESS);
    EXPECT_TRUE(segmentCtxFlag == expectedSegmentCtxFlag);
    EXPECT_TRUE(getSegmentCtxLen(cabacContextParameters, &segmentCtxLen)==SUCCESS);
    EXPECT_TRUE(segmentCtxLen == expectedSegmentCtxLen);
    EXPECT_TRUE(getSegmentCtxOffset(cabacContextParameters, &segmentCtxOffset) == SUCCESS);
    EXPECT_TRUE(segmentCtxOffset == expectedSegmentCtxOffset);

    cabacBinarization =  constructCabacBinarizationSplitUnitwiseTruncatedUnary_Bypass(expectedSplitUnitSize);
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SplitUnitWiseTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == true);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(expectedCTruncExpGolParam == cTruncExpGolParam);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == SUCCESS);
    EXPECT_TRUE(expectedSplitUnitSize == splitUnitSize);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    cabacBinarization = constructCabacBinarizationSignedSplitUnitwiseTruncatedUnary_NotBypass(
            expectedSplitUnitSize,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag,
            expectedSegmentCtxFlag,
            expectedSegmentCtxLen,
            expectedSegmentCtxOffset
    );
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedSplitUnitWiseTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == false);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == SUCCESS);
    EXPECT_TRUE(expectedSplitUnitSize == splitUnitSize);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters, &numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);
    EXPECT_TRUE(getSegmentCtxFlag(cabacContextParameters, &segmentCtxFlag)==SUCCESS);
    EXPECT_TRUE(segmentCtxFlag == expectedSegmentCtxFlag);
    EXPECT_TRUE(getSegmentCtxLen(cabacContextParameters, &segmentCtxLen)==SUCCESS);
    EXPECT_TRUE(segmentCtxLen == expectedSegmentCtxLen);
    EXPECT_TRUE(getSegmentCtxOffset(cabacContextParameters, &segmentCtxOffset) == SUCCESS);
    EXPECT_TRUE(segmentCtxOffset == expectedSegmentCtxOffset);

    cabacBinarization =  constructCabacBinarizationSignedSplitUnitwiseTruncatedUnary_Bypass(expectedSplitUnitSize);
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedSplitUnitWiseTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == true);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(expectedCTruncExpGolParam == cTruncExpGolParam);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == SUCCESS);
    EXPECT_TRUE(expectedSplitUnitSize == splitUnitSize);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    cabacBinarization = constructCabacBinarizationDoubleTruncatedUnary_NotBypass(
            expectedCMaxDTU,
            expectedSplitUnitSize,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag,
            expectedSegmentCtxFlag,
            expectedSegmentCtxLen,
            expectedSegmentCtxOffset
    );
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_DoubleTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == false);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == SUCCESS);
    EXPECT_TRUE(cMaxDTU == expectedCMaxDTU);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == SUCCESS);
    EXPECT_TRUE(expectedSplitUnitSize == splitUnitSize);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters, &numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);
    EXPECT_TRUE(getSegmentCtxFlag(cabacContextParameters, &segmentCtxFlag)==SUCCESS);
    EXPECT_TRUE(segmentCtxFlag == expectedSegmentCtxFlag);
    EXPECT_TRUE(getSegmentCtxLen(cabacContextParameters, &segmentCtxLen)==SUCCESS);
    EXPECT_TRUE(segmentCtxLen == expectedSegmentCtxLen);
    EXPECT_TRUE(getSegmentCtxOffset(cabacContextParameters, &segmentCtxOffset) == SUCCESS);
    EXPECT_TRUE(segmentCtxOffset == expectedSegmentCtxOffset);

    cabacBinarization =  constructCabacBinarizationDoubleTruncatedUnary_Bypass(expectedCMaxDTU, expectedSplitUnitSize);
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_DoubleTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == true);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(expectedCTruncExpGolParam == cTruncExpGolParam);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == SUCCESS);
    EXPECT_TRUE(cMaxDTU == expectedCMaxDTU);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == SUCCESS);
    EXPECT_TRUE(expectedSplitUnitSize == splitUnitSize);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);





    cabacBinarization = constructCabacBinarizationSignedDoubleTruncatedUnary_NotBypass(
            expectedCMaxDTU,
            expectedSplitUnitSize,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag,
            expectedSegmentCtxFlag,
            expectedSegmentCtxLen,
            expectedSegmentCtxOffset
    );
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedDoubleTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == false);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == SUCCESS);
    EXPECT_TRUE(cMaxDTU == expectedCMaxDTU);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == SUCCESS);
    EXPECT_TRUE(expectedSplitUnitSize == splitUnitSize);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters, &numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);
    EXPECT_TRUE(getSegmentCtxFlag(cabacContextParameters, &segmentCtxFlag)==SUCCESS);
    EXPECT_TRUE(segmentCtxFlag == expectedSegmentCtxFlag);
    EXPECT_TRUE(getSegmentCtxLen(cabacContextParameters, &segmentCtxLen)==SUCCESS);
    EXPECT_TRUE(segmentCtxLen == expectedSegmentCtxLen);
    EXPECT_TRUE(getSegmentCtxOffset(cabacContextParameters, &segmentCtxOffset) == SUCCESS);
    EXPECT_TRUE(segmentCtxOffset == expectedSegmentCtxOffset);

    cabacBinarization =  constructCabacBinarizationSignedDoubleTruncatedUnary_Bypass(expectedCMaxDTU, expectedSplitUnitSize);
    EXPECT_TRUE(cabacBinarization != NULL);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedDoubleTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag == true);
    EXPECT_TRUE(getBIFullCtxMode(cabacBinarization, &BIFullCtxMode) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(expectedCTruncExpGolParam == cTruncExpGolParam);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == SUCCESS);
    EXPECT_TRUE(cMaxDTU == expectedCMaxDTU);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == SUCCESS);
    EXPECT_TRUE(expectedSplitUnitSize == splitUnitSize);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
}

TEST_F(encodingParametersTest, decoderConfigurationTypeCABAC)
{
    TransformSubSymIdEnum** transformSubSymIdEnums = (TransformSubSymIdEnum**)malloc(20*sizeof(TransformSubSymIdEnum*));
    for(int descriptor_i=0; descriptor_i<20; descriptor_i++) {
        transformSubSymIdEnums[descriptor_i]=(TransformSubSymIdEnum *)malloc(20*sizeof(TransformSubSymIdEnum));
        for(int transform_Id_subsym=0; transform_Id_subsym<3; transform_Id_subsym++){
            if(transform_Id_subsym==0) {
                transformSubSymIdEnums[descriptor_i][transform_Id_subsym] = SubSym_NO_TRANSFORM;
            }else{
                transformSubSymIdEnums[descriptor_i][transform_Id_subsym] = SubSym_LUT_TRANSFORM;
            }
        }

    }

    Support_valuesType*** supportValuesCollection = (Support_valuesType***)malloc(20*sizeof(Support_valuesType**));
    for(int descriptor_i=0; descriptor_i<20; descriptor_i++) {
        supportValuesCollection[descriptor_i] = (Support_valuesType **) malloc(3 * sizeof(Support_valuesType *));
    }

    for(int transformSubSeq = 0; transformSubSeq<5; transformSubSeq++){
        for(int symbolSizeInequality=0 ; symbolSizeInequality<2; symbolSizeInequality++){
            for(int coding_order=0; coding_order<2; coding_order++){
                for(int transform_Id_subsym=0; transform_Id_subsym<3; transform_Id_subsym++) {
                    supportValuesCollection
                    [transformSubSeq + 10 * symbolSizeInequality + 5 * coding_order][transform_Id_subsym] =
                            constructSupportValues(
                                    4,
                                    (uint8_t) (symbolSizeInequality ? 5 : 3),
                                    (uint8_t) coding_order,
                                    true,
                                    true
                            );
                }
            }
        }
    }

    Cabac_binarizationsType*** cabacBinarizations = (Cabac_binarizationsType***)malloc(20*sizeof(Cabac_binarizationsType**));
    for(int descriptor_i=0; descriptor_i<20; descriptor_i++) {
        cabacBinarizations[descriptor_i] = (Cabac_binarizationsType **) malloc(3 * sizeof(Cabac_binarizationsType *));
    }
    for(int descriptor_i=0; descriptor_i<20; descriptor_i++) {
        for(int transform_Id_subsym=0; transform_Id_subsym<3; transform_Id_subsym++){
            cabacBinarizations[descriptor_i][transform_Id_subsym]=constructCabacBinarizationTruncatedUnary_Bypass();
        }
    }
    uint16_t* descriptor_subsequence_id = (uint16_t*)malloc(20*sizeof(uint16_t));
    for(int descriptor_subsequence_i=0; descriptor_subsequence_i<20; descriptor_subsequence_i++){
        descriptor_subsequence_id[descriptor_subsequence_i] = (uint16_t) descriptor_subsequence_i;
    }

    Transform_subseq_parametersType** transformSubseqParameters2
            = (Transform_subseq_parametersType**)malloc(20*sizeof(Transform_subseq_parametersType*));
    for(int transformSubSeq = 0; transformSubSeq<5; transformSubSeq++) {
        for (int symbolSizeInequality = 0; symbolSizeInequality < 2; symbolSizeInequality++) {
            for (int coding_order = 0; coding_order < 2; coding_order++) {
                Transform_subseq_parametersType* newOne;
                switch (transformSubSeq){
                    case 0:
                        newOne = constructNoTransformSubseqParameters();
                        break;
                    case 1:
                        newOne = constructEqualityCodingTransformSubseqParameters();
                        break;
                    case 2:
                        newOne = constructMatchCodingSubseqTransformSubseqParameters(40);
                        break;
                    case 3:
                        newOne = constructRLECodingSubseqTransformSubseqParameters(50);
                        break;
                    default:
                    case 4:
                        newOne = constructRLEQVCodingSubseqTransformSubseqParameters();
                        break;

                }
                transformSubseqParameters2[transformSubSeq + 10 * symbolSizeInequality + 5 * coding_order] = newOne;
            }
        }
    }

    DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC = constructDecoderConfigurationCABAC(
            19,
            descriptor_subsequence_id,
            transformSubseqParameters2,
            transformSubSymIdEnums,
            supportValuesCollection,
            cabacBinarizations
    );

    uint8_t numDescriptorSubsequenceCfgsMinus1;

    EXPECT_TRUE(getNumDescriptorSubsequenceCfgsMinus1(
            decoderConfigurationTypeCABAC,
            &numDescriptorSubsequenceCfgsMinus1
    ) == SUCCESS);
    EXPECT_TRUE(numDescriptorSubsequenceCfgsMinus1 == 19);
    for(int transformSubSeq = 0; transformSubSeq<5; transformSubSeq++) {
        for (int symbolSizeInequality = 0; symbolSizeInequality < 2; symbolSizeInequality++) {
            for (uint8_t coding_order = 0; coding_order < 2; coding_order++) {

                uint16_t descriptorSubSequence =
                        (uint16_t) (transformSubSeq + 10 * symbolSizeInequality + 5 * coding_order);

                uint16_t retrievedId;
                getDescriptorSubsequenceId(
                        decoderConfigurationTypeCABAC,
                        (uint16_t) descriptorSubSequence,
                        &retrievedId
                );

                EXPECT_TRUE(descriptorSubSequence == retrievedId);

                Transform_subseq_parametersType* transformSubseqParameter;
                getTransformSubseqParameters(decoderConfigurationTypeCABAC, descriptorSubSequence, &transformSubseqParameter);

                TransformSubSeqIdEnum resultSubSeqId;
                uint16_t resultBufferSize;
                uint8_t resultCodingGuard;
                EncodingParametersRC rcSubSeqId = getTransformSubSeqId(
                        transformSubseqParameter,
                        &resultSubSeqId
                );
                EncodingParametersRC rcBufferSize = getMatchCodingBufferSize(
                        transformSubseqParameter,
                        &resultBufferSize
                );
                EncodingParametersRC rcCodingguard = getRLECodingGuard(
                        transformSubseqParameter,
                        &resultCodingGuard
                );

                uint8_t transformSubseqCounter;
                EncodingParametersRC rcTransformSubSymCounter = getTransformSubseqCounter(
                        decoderConfigurationTypeCABAC,
                        descriptorSubSequence,
                        &transformSubseqCounter
                );

                EXPECT_TRUE(rcSubSeqId == SUCCESS);
                switch(transformSubSeq){
                    default:
                    case 0:
                        EXPECT_TRUE(resultSubSeqId == SubSeq_NO_TRANSFORM);
                        EXPECT_TRUE(rcBufferSize == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(rcCodingguard == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(transformSubseqCounter == 1);
                        break;
                    case 1:
                        EXPECT_TRUE(resultSubSeqId == SubSeq_EQUALITY_CODING);
                        EXPECT_TRUE(rcBufferSize == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(rcCodingguard == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(transformSubseqCounter == 2);
                        break;
                    case 2:
                        EXPECT_TRUE(resultSubSeqId == SubSeq_MATCH_CODING);
                        EXPECT_TRUE(rcBufferSize == SUCCESS);
                        EXPECT_TRUE(resultBufferSize == 40);
                        EXPECT_TRUE(rcCodingguard == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(transformSubseqCounter == 3);
                        break;
                    case 3:
                        EXPECT_TRUE(resultSubSeqId == SubSeq_RLE_CODING);
                        EXPECT_TRUE(rcBufferSize == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(rcCodingguard == SUCCESS);
                        EXPECT_TRUE(transformSubseqCounter == 2);
                        EXPECT_TRUE(resultCodingGuard == 50);
                        break;
                    case 4:
                        EXPECT_TRUE(resultSubSeqId == SubSeq_RLE_QV_CODING);
                        EXPECT_TRUE(rcBufferSize == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(rcCodingguard == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(transformSubseqCounter == 1);
                }

                for(uint16_t transformSubsym_i = 0; transformSubsym_i < transformSubseqCounter; transformSubsym_i++){
                    TransformSubSymIdEnum transformSubSymIdEnum;
                    EXPECT_TRUE(getTransformIdSubSym(
                            decoderConfigurationTypeCABAC,
                            descriptorSubSequence,
                            transformSubsym_i,
                            &transformSubSymIdEnum
                    )==SUCCESS);
                    if(transformSubsym_i==0) {
                        EXPECT_TRUE(transformSubSymIdEnum == SubSym_NO_TRANSFORM);
                    }else{
                        EXPECT_TRUE(transformSubSymIdEnum == SubSym_LUT_TRANSFORM);
                    }

                    uint8_t expected_outputSymbolSize = 4;
                    uint8_t expected_codingSymbolSize = (uint8_t) (symbolSizeInequality ? 5 : 3);
                    uint8_t expected_coding_order = coding_order;
                    bool expected_shareSubSymLutFlag = true;
                    bool expected_shareSubsymPrvFlag = true;
                    EncodingParametersRC expected_outputSymbolSize_RC = SUCCESS;
                    EncodingParametersRC expected_codingSymbolSize_RC = SUCCESS;
                    EncodingParametersRC expected_coding_order_RC = SUCCESS;
                    EncodingParametersRC expected_shareSubSymLutFlag_RC = FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
                    EncodingParametersRC expected_shareSubsymPrvFlag_RC = FIELDs_EXISTANCE_CONDITIONS_NOT_MET;

                    uint8_t retrieved_outputSymbolSize;
                    uint8_t retrieved_codingSymbolSize;
                    uint8_t retrieved_coding_order;
                    bool retrieved_shareSubSymLutFlag;
                    bool retrieved_shareSubsymPrvFlag;
                    EncodingParametersRC retrieved_outputSymbolSize_RC = getSupportValueOutputSymbolSize(
                            decoderConfigurationTypeCABAC,
                            descriptorSubSequence,
                            transformSubsym_i,
                            &retrieved_outputSymbolSize
                    );
                    EncodingParametersRC retrieved_codingSymbolSize_RC = getSupportValueCodingSymbolSize(
                            decoderConfigurationTypeCABAC,
                            descriptorSubSequence,
                            transformSubsym_i,
                            &retrieved_codingSymbolSize
                    );
                    EncodingParametersRC retrieved_coding_order_RC = getSupportValueCodingOrder(
                            decoderConfigurationTypeCABAC,
                            descriptorSubSequence,
                            transformSubsym_i,
                            &retrieved_coding_order
                    );
                    EncodingParametersRC retrieved_shareSubSymLutFlag_RC = getSupportValueShareSubsymLUTFlag(
                            decoderConfigurationTypeCABAC,
                            descriptorSubSequence,
                            transformSubsym_i,
                            &retrieved_shareSubSymLutFlag
                    );
                    EncodingParametersRC retrieved_shareSubsymPrvFlag_RC = getSupportValueShareSubsymPRVFlag(
                            decoderConfigurationTypeCABAC,
                            descriptorSubSequence,
                            transformSubsym_i,
                            &retrieved_shareSubsymPrvFlag
                    );

                    if(!symbolSizeInequality && coding_order>0){
                        expected_shareSubsymPrvFlag_RC = SUCCESS;
                        if(transformSubSymIdEnum  == SubSym_LUT_TRANSFORM){
                            expected_shareSubSymLutFlag_RC = SUCCESS;
                        }
                    }

                    EXPECT_TRUE(expected_outputSymbolSize == retrieved_outputSymbolSize);
                    EXPECT_TRUE(expected_codingSymbolSize == retrieved_codingSymbolSize);
                    EXPECT_TRUE(expected_coding_order == retrieved_coding_order);
                    if(expected_shareSubSymLutFlag_RC==SUCCESS) {
                        EXPECT_TRUE(expected_shareSubSymLutFlag == retrieved_shareSubSymLutFlag);
                    }
                    if(expected_shareSubsymPrvFlag_RC==SUCCESS) {
                        EXPECT_TRUE(expected_shareSubsymPrvFlag == retrieved_shareSubsymPrvFlag);
                    }
                    EXPECT_TRUE(expected_outputSymbolSize_RC == retrieved_outputSymbolSize_RC);
                    EXPECT_TRUE(expected_codingSymbolSize_RC == retrieved_codingSymbolSize_RC);
                    EXPECT_TRUE(expected_coding_order_RC == retrieved_coding_order_RC);
                    EXPECT_TRUE(expected_shareSubSymLutFlag_RC == retrieved_shareSubSymLutFlag_RC);
                    EXPECT_TRUE(expected_shareSubsymPrvFlag_RC == retrieved_shareSubsymPrvFlag_RC);

                    Cabac_binarizationsType* retrievedCabacBinarization;
                    EXPECT_TRUE(getCABACBinarizations(
                            decoderConfigurationTypeCABAC,
                            descriptorSubSequence,
                            transformSubsym_i,
                            &retrievedCabacBinarization
                    ) == SUCCESS);
                    EXPECT_TRUE(retrievedCabacBinarization->binarization_ID == BinarizationID_TruncatedUnary);
                }
            }
        }
    } 
}