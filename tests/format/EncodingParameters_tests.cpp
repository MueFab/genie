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
}

TEST_F(encodingParametersTest, binarization)
{
    uint8_t binarization_ID;
    bool bypassFlag;
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
    Cabac_context_parametersType* cabacContextParameters;


    Cabac_binarizationsType* cabacBinarization = constructCabacBinarizationBinaryCoding_NotBypass(
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag);

    EXPECT_TRUE(nullptr != cabacBinarization);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_BinaryCoding);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(!bypassFlag);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(nullptr != cabacContextParameters);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters,&numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);

    cabacBinarization = constructCabacBinarizationBinaryCoding_Bypass();
    EXPECT_TRUE(nullptr != cabacBinarization);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_BinaryCoding);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);


    cabacBinarization = constructCabacBinarizationTruncatedUnary_NotBypass(0,
                                                                           expectedAdaptive_mode_flag,
                                                                           expectedNumContexts,
                                                                           expectedContextInitialization,
                                                                           expectedShareSubSymCtxFlag);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_TruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(!bypassFlag);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(cabacContextParameters != nullptr);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters,&numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);

    cabacBinarization = constructCabacBinarizationTruncatedUnary_Bypass(0);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_TruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    cabacBinarization = constructCabacBinarizationExponentialGolomb_NotBypass(
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_ExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(!bypassFlag);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters, &numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);


    cabacBinarization = constructCabacBinarizationExponentialGolomb_Bypass();
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_ExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    cabacBinarization = constructCabacBinarizationSignedExponentialGolomb_NotBypass(
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(!bypassFlag);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == SUCCESS);
    EXPECT_TRUE(getAdaptiveModeFlag(cabacContextParameters, &adaptiveModeFlag) == SUCCESS);
    EXPECT_TRUE(adaptiveModeFlag == expectedAdaptive_mode_flag);
    EXPECT_TRUE(getContextInitializationValues(cabacContextParameters, &numContexts, &contextInitialization)==SUCCESS);
    EXPECT_TRUE(numContexts == expectedNumContexts);
    EXPECT_TRUE(contextInitialization == expectedContextInitialization);

    cabacBinarization = constructCabacBinarizationSignedExponentialGolomb_Bypass();
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag);
    EXPECT_TRUE(getCTruncExpGolParam(cabacBinarization, &cTruncExpGolParam) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getcMaxDTU(cabacBinarization, &cMaxDTU) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getSplitUnitSize(cabacBinarization, &splitUnitSize) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacContextParameters(cabacBinarization, &cabacContextParameters) == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    cabacBinarization = constructCabacBinarizationTruncatedExponentialGolomb_NotBypass(
            expectedCTruncExpGolParam,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            expectedContextInitialization,
            expectedShareSubSymCtxFlag);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_TruncatedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(!bypassFlag);
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


    cabacBinarization =  constructCabacBinarizationTruncatedExponentialGolomb_Bypass(cTruncExpGolParam);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_TruncatedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag);
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
            expectedShareSubSymCtxFlag);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedTruncatedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(!bypassFlag);
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

    cabacBinarization =  constructCabacBinarizationSignedTruncatedExponentialGolomb_Bypass(cTruncExpGolParam);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedTruncatedExponentialGolomb);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag);
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
            expectedShareSubSymCtxFlag);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SplitUnitWiseTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(!bypassFlag);
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

    cabacBinarization =  constructCabacBinarizationSplitUnitwiseTruncatedUnary_Bypass(expectedSplitUnitSize);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SplitUnitWiseTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag);
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
            expectedShareSubSymCtxFlag);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedSplitUnitWiseTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(!bypassFlag);
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

    cabacBinarization =  constructCabacBinarizationSignedSplitUnitwiseTruncatedUnary_Bypass(expectedSplitUnitSize);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedSplitUnitWiseTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag);
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
            expectedShareSubSymCtxFlag);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_DoubleTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(!bypassFlag);
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

    cabacBinarization =  constructCabacBinarizationDoubleTruncatedUnary_Bypass(expectedCMaxDTU, expectedSplitUnitSize);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_DoubleTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag);
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
            expectedShareSubSymCtxFlag);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedDoubleTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(!bypassFlag);
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

    cabacBinarization =  constructCabacBinarizationSignedDoubleTruncatedUnary_Bypass(expectedCMaxDTU, expectedSplitUnitSize);
    EXPECT_TRUE(cabacBinarization != nullptr);
    EXPECT_TRUE(getBinarizationId(cabacBinarization, &binarization_ID)==SUCCESS);
    EXPECT_TRUE(binarization_ID == BinarizationID_SignedDoubleTruncatedUnary);
    EXPECT_TRUE(getBypassFlag(cabacBinarization, &bypassFlag)==SUCCESS);
    EXPECT_TRUE(bypassFlag);
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
    auto ** transformSubSymIdEnums = (TransformSubSymIdEnum**)malloc(20*sizeof(TransformSubSymIdEnum*));
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

    auto *** supportValuesCollection = (Support_valuesType***)malloc(20*sizeof(Support_valuesType**));
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

    auto *** cabacBinarizations = (Cabac_binarizationsType***)malloc(20*sizeof(Cabac_binarizationsType**));
    for(int descriptor_i=0; descriptor_i<20; descriptor_i++) {
        cabacBinarizations[descriptor_i] = (Cabac_binarizationsType **) malloc(3 * sizeof(Cabac_binarizationsType *));
    }
    for(int descriptor_i=0; descriptor_i<20; descriptor_i++) {
        for(int transform_Id_subsym=0; transform_Id_subsym<3; transform_Id_subsym++){
            cabacBinarizations[descriptor_i][transform_Id_subsym]= constructCabacBinarizationTruncatedUnary_Bypass(0);
        }
    }
    auto * descriptor_subsequence_id = (uint16_t*)malloc(20*sizeof(uint16_t));
    for(int descriptor_subsequence_i=0; descriptor_subsequence_i<20; descriptor_subsequence_i++){
        descriptor_subsequence_id[descriptor_subsequence_i] = (uint16_t) descriptor_subsequence_i;
    }

    auto ** transformSubseqParameters2
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
                    default:
                    case 3:
                        newOne = constructRLECodingSubseqTransformSubseqParameters(50);
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

                auto descriptorSubSequence =
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
                    case 0:
                        EXPECT_EQ(SUCCESS, rcTransformSubSymCounter);
                        EXPECT_TRUE(resultSubSeqId == SubSeq_NO_TRANSFORM);
                        EXPECT_TRUE(rcBufferSize == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(rcCodingguard == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(transformSubseqCounter == 1);
                        break;
                    case 1:
                        EXPECT_EQ(SUCCESS, rcTransformSubSymCounter);
                        EXPECT_TRUE(resultSubSeqId == SubSeq_EQUALITY_CODING);
                        EXPECT_TRUE(rcBufferSize == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(rcCodingguard == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(transformSubseqCounter == 2);
                        break;
                    case 2:
                        EXPECT_EQ(SUCCESS, rcTransformSubSymCounter);
                        EXPECT_TRUE(resultSubSeqId == SubSeq_MATCH_CODING);
                        EXPECT_TRUE(rcBufferSize == SUCCESS);
                        EXPECT_TRUE(resultBufferSize == 40);
                        EXPECT_TRUE(rcCodingguard == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(transformSubseqCounter == 3);
                        break;
                    default:
                    case 3:
                        EXPECT_EQ(SUCCESS, rcTransformSubSymCounter);
                        EXPECT_TRUE(resultSubSeqId == SubSeq_RLE_CODING);
                        EXPECT_TRUE(rcBufferSize == FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
                        EXPECT_TRUE(rcCodingguard == SUCCESS);
                        EXPECT_TRUE(transformSubseqCounter == 2);
                        EXPECT_TRUE(resultCodingGuard == 50);
                        break;
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
                    auto expected_codingSymbolSize = (uint8_t) (symbolSizeInequality ? 5 : 3);
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

TEST_F(encodingParametersTest, decoderConfigurationTokentypeCABAC)
{
    uint8_t expected_output_symbol_size_tokentype = 64;
    uint8_t expected_output_symbol_size = 32;
    uint8_t expected_coding_symbols_tokentype_numbers = 8;
    uint8_t expected_coding_symbol_tokentype_size[64];
    for(int i=0; i<64; i++){
        expected_coding_symbol_tokentype_size[i] = (uint8_t) ((i < 8) ? 8 : 0);
    }
    uint8_t expected_coding_symbol_size = 66;
    uint8_t expected_coding_order = 1;
    bool expected_share_subsym_lut_flag = true;
    bool expected_share_subsym_prv_flag = true;
    Cabac_binarizationsType* expectedCabacBinarizations = constructCabacBinarizationTruncatedUnary_Bypass(0);

    uint8_t retrieved_output_symbol_size_tokentype;
    uint8_t retrieved_output_symbol_size;
    uint8_t retrieved_coding_symbols_tokentype_numbers;
    uint8_t* retrieved_coding_symbol_tokentype_size;
    uint8_t retrieved_coding_symbol_size;
    uint8_t retrieved_coding_order;
    bool retrieved_share_subsym_lut_flag;
    Cabac_binarizationsType* retrievedCabacBinarizations;


    TransformSubSymIdEnum transformSubSymIdEnumToken = SubSym_NO_TRANSFORM;
    Support_valuesType* expected_support_values = constructSupportValues(
            expected_output_symbol_size,
            expected_coding_symbol_size,
            expected_coding_order,
            expected_share_subsym_lut_flag,
            expected_share_subsym_prv_flag
    );

    Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCABAC = contructDecoder_configuration_tokentypeCABAC(
            expected_output_symbol_size_tokentype,
            expected_coding_symbols_tokentype_numbers,
            expected_coding_symbol_tokentype_size,
            transformSubSymIdEnumToken,
            expected_support_values,
            expectedCabacBinarizations
    );
    EXPECT_TRUE(decoderConfigurationTokentypeCABAC != nullptr);
    EXPECT_TRUE(getOutputSymbolSizeToken(decoderConfigurationTokentypeCABAC, &retrieved_output_symbol_size_tokentype)==SUCCESS);
    EXPECT_TRUE(expected_output_symbol_size_tokentype == retrieved_output_symbol_size_tokentype);
    EXPECT_TRUE(getCodingSymbolsNumbers(decoderConfigurationTokentypeCABAC, &retrieved_coding_symbols_tokentype_numbers)==SUCCESS);
    EXPECT_TRUE(retrieved_coding_symbols_tokentype_numbers == expected_coding_symbols_tokentype_numbers);
    EXPECT_TRUE(getCodingSymbolSizes(
            decoderConfigurationTokentypeCABAC,
            &retrieved_coding_symbol_size,
            &retrieved_coding_symbol_tokentype_size
    ) == SUCCESS
    );
    EXPECT_TRUE(retrieved_coding_symbols_tokentype_numbers == expected_coding_symbols_tokentype_numbers);
    for(int i=0; i<retrieved_coding_symbol_size; i++) {
        EXPECT_TRUE(retrieved_coding_symbol_tokentype_size[i] == expected_coding_symbol_tokentype_size[i]);
    }
    EXPECT_TRUE(getDecoderConfigurationTokentypeCabacSupportOutputSymbolSize(
            decoderConfigurationTokentypeCABAC,
            &retrieved_output_symbol_size
    ) == SUCCESS);
    EXPECT_TRUE(getDecoderConfigurationTokentypeCabacSupportCodingSymbolSize(
            decoderConfigurationTokentypeCABAC,
            &retrieved_coding_symbol_size
    )==SUCCESS);
    EXPECT_TRUE(retrieved_coding_symbol_size == expected_coding_symbol_size);
    EXPECT_TRUE(getDecoderConfigurationTokentypeCabacSupportCodingOrder(
            decoderConfigurationTokentypeCABAC,
            &retrieved_coding_order
    ) == SUCCESS);
    EXPECT_TRUE(retrieved_coding_order == expected_coding_order);
    EXPECT_TRUE(getDecoderConfigurationTokentypeCabacSupportShareSubsymLutFlag(
            decoderConfigurationTokentypeCABAC,
            &retrieved_share_subsym_lut_flag
    )==FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getDecoderConfigurationTokentypeCabacSupportShareSubsymPrvFlag(
            decoderConfigurationTokentypeCABAC,
            &retrieved_share_subsym_lut_flag
    )==FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    EXPECT_TRUE(getCabacBinarizationsToken(
            decoderConfigurationTokentypeCABAC,
            &retrievedCabacBinarizations
    )==SUCCESS);
    EXPECT_TRUE(expectedCabacBinarizations == retrievedCabacBinarizations);


    uint8_t expectedRleGuard = 50;
    uint8_t retrievedRleGuard;
    Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCABAC2 = contructDecoder_configuration_tokentypeCABAC(
            expected_output_symbol_size_tokentype,
            expected_coding_symbols_tokentype_numbers,
            expected_coding_symbol_tokentype_size,
            transformSubSymIdEnumToken,
            expected_support_values,
            expectedCabacBinarizations
    );
    Decoder_configuration_tokentype* decoderConfigurationTokentype = constructDecoderConfigurationTokentype(
            expectedRleGuard,
            decoderConfigurationTokentypeCABAC,
            decoderConfigurationTokentypeCABAC2
    );
    Decoder_configuration_tokentype_cabac* retrievedDecoderConfigurationTokentype1;
    Decoder_configuration_tokentype_cabac* retrievedDecoderConfigurationTokentype2;
    EXPECT_TRUE(getRLEGuardTokenType(
            decoderConfigurationTokentype,
            &retrievedRleGuard
    )==SUCCESS);
    EXPECT_TRUE(retrievedRleGuard == expectedRleGuard);
    EXPECT_TRUE(getDecoderConfigurationTokentypeCabac_order0(
            decoderConfigurationTokentype,
            &retrievedDecoderConfigurationTokentype1
    ) == SUCCESS);
    EXPECT_TRUE(retrievedDecoderConfigurationTokentype1 == decoderConfigurationTokentypeCABAC);
    EXPECT_TRUE(getDecoderConfigurationTokentypeCabac_order1(
            decoderConfigurationTokentype,
            &retrievedDecoderConfigurationTokentype2
    )==SUCCESS);
    EXPECT_TRUE(retrievedDecoderConfigurationTokentype2 == decoderConfigurationTokentypeCABAC2);
}