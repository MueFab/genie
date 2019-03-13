#include "gtest/gtest.h"

extern "C" {
#include "format/EncodingParameters.h"
}


DecoderConfigurationTypeCABAC* getTestingDecoderConfigurationTypeCABAC(){
    uint8_t num_subsequences = 5;
    auto * descriptorSubsequenceID = (uint16_t*)malloc(sizeof(uint16_t)* num_subsequences);
    for(uint8_t subsequence_i=0; subsequence_i<num_subsequences; subsequence_i++){
        descriptorSubsequenceID[subsequence_i] = subsequence_i;
    }
    auto ** transform_subseq_parameters =
            (Transform_subseq_parametersType**)malloc(sizeof(Transform_subseq_parametersType)*5);
    transform_subseq_parameters[0] = constructNoTransformSubseqParameters();
    transform_subseq_parameters[1] = constructEqualityCodingTransformSubseqParameters();
    transform_subseq_parameters[2] = constructMatchCodingSubseqTransformSubseqParameters(255);
    transform_subseq_parameters[3] = constructRLECodingSubseqTransformSubseqParameters(5);
    auto * merge_coding_shift_size = (uint8_t*)malloc(sizeof(uint8_t)*2);
    merge_coding_shift_size[0]=1;
    merge_coding_shift_size[1]=2;
    transform_subseq_parameters[4] = constructMergeCodingSubseqTransformSubseqParameters(
            2, merge_coding_shift_size
    );

    auto ** transform_id_subsyms = (TransformSubSymIdEnum**)malloc(5*sizeof(TransformSubSymIdEnum*));
    transform_id_subsyms[0] = (TransformSubSymIdEnum*)malloc(1* sizeof(TransformSubSymIdEnum));
    transform_id_subsyms[0][0] = SubSym_NO_TRANSFORM;
    transform_id_subsyms[1] = (TransformSubSymIdEnum*)malloc(2* sizeof(TransformSubSymIdEnum));
    transform_id_subsyms[1][0] = SubSym_NO_TRANSFORM;
    transform_id_subsyms[1][1] = SubSym_DIFF_CODING;
    transform_id_subsyms[2] = (TransformSubSymIdEnum*)malloc(3* sizeof(TransformSubSymIdEnum));
    transform_id_subsyms[2][0] = SubSym_NO_TRANSFORM;
    transform_id_subsyms[2][1] = SubSym_DIFF_CODING;
    transform_id_subsyms[2][2] = SubSym_LUT_TRANSFORM;
    transform_id_subsyms[3] = (TransformSubSymIdEnum*)malloc(2* sizeof(TransformSubSymIdEnum));
    transform_id_subsyms[3][0] = SubSym_NO_TRANSFORM;
    transform_id_subsyms[3][1] = SubSym_DIFF_CODING;
    transform_id_subsyms[4] = (TransformSubSymIdEnum*)malloc(2* sizeof(TransformSubSymIdEnum));
    transform_id_subsyms[4][0] = SubSym_NO_TRANSFORM;
    transform_id_subsyms[4][1] = SubSym_DIFF_CODING;

    auto *** support_values = (Support_valuesType***)malloc(5*sizeof(Support_valuesType**));
    support_values[0] = (Support_valuesType**)malloc(1 * sizeof(Support_valuesType*));
    support_values[0][0] = constructSupportValues(3,4,1,false,false);
    support_values[1] = (Support_valuesType**)malloc(2 * sizeof(Support_valuesType*));
    support_values[1][0] = constructSupportValues(3,4,1,false,false);
    support_values[1][1] = constructSupportValues(3,4,1,false,false);
    support_values[2] = (Support_valuesType**)malloc(3 * sizeof(Support_valuesType*));
    support_values[2][0] = constructSupportValues(3,4,1,false,false);
    support_values[2][1] = constructSupportValues(3,4,1,false,false);
    support_values[2][2] = constructSupportValues(3,4,1,false,false);
    support_values[3] = (Support_valuesType**)malloc(2 * sizeof(Support_valuesType*));
    support_values[3][0] = constructSupportValues(3,4,1,false,false);
    support_values[3][1] = constructSupportValues(3,4,1,false,false);
    support_values[4] = (Support_valuesType**)malloc(2 * sizeof(Support_valuesType*));
    support_values[4][0] = constructSupportValues(3,4,1,false,false);
    support_values[4][1] = constructSupportValues(3,4,1,false,false);

    auto *** cabac_binarizations = (Cabac_binarizationsType***)malloc(5*sizeof(Cabac_binarizationsType));
    cabac_binarizations[0] = (Cabac_binarizationsType**)malloc(1 * sizeof(Cabac_binarizationsType));
    cabac_binarizations[0][0] = constructCabacBinarizationBinaryCoding_Bypass();
    cabac_binarizations[1] = (Cabac_binarizationsType**)malloc(2 * sizeof(Cabac_binarizationsType));
    cabac_binarizations[1][0] = constructCabacBinarizationBinaryCoding_Bypass();
    {
        auto * contextInitVals = (uint8_t*) malloc(sizeof(uint8_t)*2);
        contextInitVals[0] = 0;
        contextInitVals[1] = 1;
        cabac_binarizations[1][1] = constructCabacBinarizationBinaryCoding_NotBypass(
                true, 2, contextInitVals, false
        );
    }
    cabac_binarizations[2] = (Cabac_binarizationsType**)malloc(3 * sizeof(Cabac_binarizationsType));
    {
        auto * contextInitVals = (uint8_t*) malloc(sizeof(uint8_t)*2);
        contextInitVals[0] = 0;
        contextInitVals[1] = 1;
        cabac_binarizations[2][0] = constructCabacBinarizationDoubleTruncatedUnary_NotBypass(
                2,3, true, 2, contextInitVals, false
        );
    }
    cabac_binarizations[2][1] = constructCabacBinarizationExponentialGolomb_Bypass();
    {
        auto * contextInitVals = (uint8_t*) malloc(sizeof(uint8_t)*2);
        contextInitVals[0] = 0;
        contextInitVals[1] = 1;
        cabac_binarizations[2][2] = constructCabacBinarizationExponentialGolomb_NotBypass(
                true, 2, contextInitVals, false
        );
    }
    cabac_binarizations[3] = (Cabac_binarizationsType**)malloc(2 * sizeof(Cabac_binarizationsType));
    cabac_binarizations[3][0] = constructCabacBinarizationSignedExponentialGolomb_Bypass();
    {
        auto * contextInitVals = (uint8_t*) malloc(sizeof(uint8_t)*2);
        contextInitVals[0] = 0;
        contextInitVals[1] = 1;
        cabac_binarizations[3][1] = constructCabacBinarizationSignedExponentialGolomb_NotBypass(
                true, 2, contextInitVals, false
        );
    }
    cabac_binarizations[3][1] = constructCabacBinarizationTruncatedExponentialGolomb_Bypass(2);
    cabac_binarizations[4] = (Cabac_binarizationsType**)malloc(2*sizeof(Cabac_binarizationsType));
    {
        auto * contextInitVals = (uint8_t*) malloc(sizeof(uint8_t)*2);
        contextInitVals[0] = 0;
        contextInitVals[1] = 1;
        cabac_binarizations[4][0] = constructCabacBinarizationTruncatedExponentialGolomb_NotBypass(
                2,true, 2, contextInitVals, false
        );
    }
    cabac_binarizations[4][1] = constructCabacBinarizationSignedTruncatedExponentialGolomb_Bypass(2);


    return constructDecoderConfigurationCABAC(
            static_cast<uint8_t>(num_subsequences - 1),
            descriptorSubsequenceID,
            transform_subseq_parameters,
            transform_id_subsyms,
            support_values,
            cabac_binarizations
    );
}

void testDecoderConfigurationCabac(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC){
    uint8_t num_subsequences = 5;
    uint8_t num_subsequences_buffer;
    EXPECT_EQ(SUCCESS, getNumDescriptorSubsequenceCfgsMinus1(decoderConfigurationTypeCABAC, &num_subsequences_buffer));
    EXPECT_EQ(num_subsequences-1, num_subsequences_buffer);

    for(uint8_t subsequence_i=0; subsequence_i<num_subsequences; subsequence_i++){
        uint16_t subsequenceID;
        EXPECT_EQ(SUCCESS, getDescriptorSubsequenceId(decoderConfigurationTypeCABAC,subsequence_i, &subsequenceID));
        EXPECT_EQ(subsequence_i, subsequenceID);
    }

    uint8_t transform_subseq_counter;
    TransformSubSymIdEnum transformSubSymIdEnum_buffer;
    EXPECT_EQ(SUCCESS, getTransformSubseqCounter(decoderConfigurationTypeCABAC, 0, &transform_subseq_counter));
    EXPECT_EQ(1, transform_subseq_counter);
    EXPECT_EQ(SUCCESS, getTransformIdSubSym(decoderConfigurationTypeCABAC, 0, 0, &transformSubSymIdEnum_buffer));
    EXPECT_EQ(SubSym_NO_TRANSFORM, transformSubSymIdEnum_buffer);

    EXPECT_EQ(SUCCESS, getTransformSubseqCounter(decoderConfigurationTypeCABAC, 1, &transform_subseq_counter));
    EXPECT_EQ(2, transform_subseq_counter);
    EXPECT_EQ(SUCCESS, getTransformIdSubSym(decoderConfigurationTypeCABAC, 1, 0, &transformSubSymIdEnum_buffer));
    EXPECT_EQ(SubSym_NO_TRANSFORM, transformSubSymIdEnum_buffer);
    EXPECT_EQ(SUCCESS, getTransformIdSubSym(decoderConfigurationTypeCABAC, 1, 1, &transformSubSymIdEnum_buffer));
    EXPECT_EQ(SubSym_DIFF_CODING, transformSubSymIdEnum_buffer);

    EXPECT_EQ(SUCCESS, getTransformSubseqCounter(decoderConfigurationTypeCABAC, 2, &transform_subseq_counter));
    EXPECT_EQ(3, transform_subseq_counter);
    EXPECT_EQ(SUCCESS, getTransformIdSubSym(decoderConfigurationTypeCABAC, 2, 0, &transformSubSymIdEnum_buffer));
    EXPECT_EQ(SubSym_NO_TRANSFORM, transformSubSymIdEnum_buffer);
    EXPECT_EQ(SUCCESS, getTransformIdSubSym(decoderConfigurationTypeCABAC, 2, 1, &transformSubSymIdEnum_buffer));
    EXPECT_EQ(SubSym_DIFF_CODING, transformSubSymIdEnum_buffer);
    EXPECT_EQ(SUCCESS, getTransformIdSubSym(decoderConfigurationTypeCABAC, 2, 2, &transformSubSymIdEnum_buffer));
    EXPECT_EQ(SubSym_LUT_TRANSFORM, transformSubSymIdEnum_buffer);

    EXPECT_EQ(SUCCESS, getTransformSubseqCounter(decoderConfigurationTypeCABAC, 3, &transform_subseq_counter));
    EXPECT_EQ(2, transform_subseq_counter);
    EXPECT_EQ(SUCCESS, getTransformIdSubSym(decoderConfigurationTypeCABAC, 3, 0, &transformSubSymIdEnum_buffer));
    EXPECT_EQ(SubSym_NO_TRANSFORM, transformSubSymIdEnum_buffer);
    EXPECT_EQ(SUCCESS, getTransformIdSubSym(decoderConfigurationTypeCABAC, 3, 1, &transformSubSymIdEnum_buffer));
    EXPECT_EQ(SubSym_DIFF_CODING, transformSubSymIdEnum_buffer);

    EXPECT_EQ(SUCCESS, getTransformSubseqCounter(decoderConfigurationTypeCABAC, 4, &transform_subseq_counter));
    EXPECT_EQ(2, transform_subseq_counter);
    EXPECT_EQ(SUCCESS, getTransformIdSubSym(decoderConfigurationTypeCABAC, 4, 0, &transformSubSymIdEnum_buffer));
    EXPECT_EQ(SubSym_NO_TRANSFORM, transformSubSymIdEnum_buffer);
    EXPECT_EQ(SUCCESS, getTransformIdSubSym(decoderConfigurationTypeCABAC, 4, 1, &transformSubSymIdEnum_buffer));
    EXPECT_EQ(SubSym_DIFF_CODING, transformSubSymIdEnum_buffer);


    Transform_subseq_parametersType* transform_subseq_parameters_buffer;

    auto ** transform_subseq_parameters =
            (Transform_subseq_parametersType**)malloc(sizeof(Transform_subseq_parametersType)*5);
    transform_subseq_parameters[0] = constructNoTransformSubseqParameters();
    transform_subseq_parameters[1] = constructEqualityCodingTransformSubseqParameters();
    transform_subseq_parameters[2] = constructMatchCodingSubseqTransformSubseqParameters(255);
    transform_subseq_parameters[3] = constructRLECodingSubseqTransformSubseqParameters(5);
    auto * merge_coding_shift_size = (uint8_t*)malloc(sizeof(uint8_t)*2);
    merge_coding_shift_size[0]=1;
    merge_coding_shift_size[1]=2;
    transform_subseq_parameters[4] = constructMergeCodingSubseqTransformSubseqParameters(
            2, merge_coding_shift_size
    );

    auto ** transform_id_subsyms = (TransformSubSymIdEnum**)malloc(5*sizeof(TransformSubSymIdEnum*));
    transform_id_subsyms[0] = (TransformSubSymIdEnum*)malloc(1* sizeof(TransformSubSymIdEnum));
    transform_id_subsyms[0][0] = SubSym_NO_TRANSFORM;
    transform_id_subsyms[1] = (TransformSubSymIdEnum*)malloc(2* sizeof(TransformSubSymIdEnum));
    transform_id_subsyms[1][0] = SubSym_NO_TRANSFORM;
    transform_id_subsyms[1][1] = SubSym_DIFF_CODING;
    transform_id_subsyms[2] = (TransformSubSymIdEnum*)malloc(3* sizeof(TransformSubSymIdEnum));
    transform_id_subsyms[2][0] = SubSym_NO_TRANSFORM;
    transform_id_subsyms[2][1] = SubSym_DIFF_CODING;
    transform_id_subsyms[2][2] = SubSym_LUT_TRANSFORM;
    transform_id_subsyms[3] = (TransformSubSymIdEnum*)malloc(2* sizeof(TransformSubSymIdEnum));
    transform_id_subsyms[3][0] = SubSym_NO_TRANSFORM;
    transform_id_subsyms[3][1] = SubSym_DIFF_CODING;
    transform_id_subsyms[4] = (TransformSubSymIdEnum*)malloc(2* sizeof(TransformSubSymIdEnum));
    transform_id_subsyms[4][0] = SubSym_NO_TRANSFORM;
    transform_id_subsyms[4][1] = SubSym_DIFF_CODING;

    auto *** support_values = (Support_valuesType***)malloc(5*sizeof(Support_valuesType**));
    support_values[0] = (Support_valuesType**)malloc(1 * sizeof(Support_valuesType*));
    support_values[0][0] = constructSupportValues(3,4,1,false,false);
    support_values[1] = (Support_valuesType**)malloc(2 * sizeof(Support_valuesType*));
    support_values[1][0] = constructSupportValues(3,4,1,false,false);
    support_values[1][1] = constructSupportValues(3,4,1,false,false);
    support_values[2] = (Support_valuesType**)malloc(3 * sizeof(Support_valuesType*));
    support_values[2][0] = constructSupportValues(3,4,1,false,false);
    support_values[2][1] = constructSupportValues(3,4,1,false,false);
    support_values[2][2] = constructSupportValues(3,4,1,false,false);
    support_values[3] = (Support_valuesType**)malloc(2 * sizeof(Support_valuesType*));
    support_values[3][0] = constructSupportValues(3,4,1,false,false);
    support_values[3][1] = constructSupportValues(3,4,1,false,false);
    support_values[4] = (Support_valuesType**)malloc(2 * sizeof(Support_valuesType*));
    support_values[4][0] = constructSupportValues(3,4,1,false,false);
    support_values[4][1] = constructSupportValues(3,4,1,false,false);

    auto *** cabac_binarizations = (Cabac_binarizationsType***)malloc(5*sizeof(Cabac_binarizationsType));
    cabac_binarizations[0] = (Cabac_binarizationsType**)malloc(1 * sizeof(Cabac_binarizationsType));
    cabac_binarizations[0][0] = constructCabacBinarizationBinaryCoding_Bypass();
    cabac_binarizations[1] = (Cabac_binarizationsType**)malloc(2 * sizeof(Cabac_binarizationsType));
    cabac_binarizations[1][0] = constructCabacBinarizationBinaryCoding_Bypass();
    {
        auto * contextInitVals = (uint8_t*) malloc(sizeof(uint8_t)*2);
        contextInitVals[0] = 0;
        contextInitVals[1] = 1;
        cabac_binarizations[1][1] = constructCabacBinarizationBinaryCoding_NotBypass(
                true, 2, contextInitVals, false
        );
    }
    cabac_binarizations[2] = (Cabac_binarizationsType**)malloc(3 * sizeof(Cabac_binarizationsType));
    {
        auto * contextInitVals = (uint8_t*) malloc(sizeof(uint8_t)*2);
        contextInitVals[0] = 0;
        contextInitVals[1] = 1;
        cabac_binarizations[2][0] = constructCabacBinarizationDoubleTruncatedUnary_NotBypass(
                2,3, true, 2, contextInitVals, false
        );
    }
    cabac_binarizations[2][1] = constructCabacBinarizationExponentialGolomb_Bypass();
    {
        auto * contextInitVals = (uint8_t*) malloc(sizeof(uint8_t)*2);
        contextInitVals[0] = 0;
        contextInitVals[1] = 1;
        cabac_binarizations[2][2] = constructCabacBinarizationExponentialGolomb_NotBypass(
                true, 2, contextInitVals, false
        );
    }
    cabac_binarizations[3] = (Cabac_binarizationsType**)malloc(2 * sizeof(Cabac_binarizationsType));
    cabac_binarizations[3][0] = constructCabacBinarizationSignedExponentialGolomb_Bypass();
    {
        auto * contextInitVals = (uint8_t*) malloc(sizeof(uint8_t)*2);
        contextInitVals[0] = 0;
        contextInitVals[1] = 1;
        cabac_binarizations[3][1] = constructCabacBinarizationSignedExponentialGolomb_NotBypass(
                true, 2, contextInitVals, false
        );
    }
    cabac_binarizations[3][1] = constructCabacBinarizationTruncatedExponentialGolomb_Bypass(2);
    cabac_binarizations[4] = (Cabac_binarizationsType**)malloc(2*sizeof(Cabac_binarizationsType));
    {
        auto * contextInitVals = (uint8_t*) malloc(sizeof(uint8_t)*2);
        contextInitVals[0] = 0;
        contextInitVals[1] = 1;
        cabac_binarizations[4][0] = constructCabacBinarizationTruncatedExponentialGolomb_NotBypass(
                2,true, 2, contextInitVals, false
        );
    }
    cabac_binarizations[4][1] = constructCabacBinarizationSignedTruncatedExponentialGolomb_Bypass(2);
}

Decoder_configuration_tokentype* getTestingDecoderConfigurationTokenType(){
    auto * transformSubSymIdEnum0 = (TransformSubSymIdEnum *)(malloc(sizeof(TransformSubSymIdEnum)));
    transformSubSymIdEnum0[0] = SubSym_NO_TRANSFORM;
    auto supportValues0 = (Support_valuesType**)malloc(sizeof(Support_valuesType*));
    supportValues0[0] = constructSupportValues(3,4,1,false,false);
    auto cabacBinarizations0 = (Cabac_binarizationsType**)malloc(sizeof(Cabac_binarizationsType*));
    cabacBinarizations0[0] = constructCabacBinarizationBinaryCoding_Bypass();

    auto * transformSubSymIdEnum1 = (TransformSubSymIdEnum *)(malloc(sizeof(TransformSubSymIdEnum)));
    transformSubSymIdEnum1[0] = SubSym_NO_TRANSFORM;
    auto supportValues1 = (Support_valuesType**)malloc(sizeof(Support_valuesType*));
    supportValues1[0] = constructSupportValues(3,4,1,false,false);
    auto cabacBinarizations1 = (Cabac_binarizationsType**)malloc(sizeof(Cabac_binarizationsType*));
    cabacBinarizations1[0] = constructCabacBinarizationBinaryCoding_Bypass();

    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac0 =
            constructDecoder_configuration_tokentype_cabac(
                    constructNoTransformSubseqParameters(),
                    transformSubSymIdEnum0,
                    supportValues0,
                    cabacBinarizations0
            );
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac1 =
            constructDecoder_configuration_tokentype_cabac(
                    constructNoTransformSubseqParameters(),
                    transformSubSymIdEnum1,
                    supportValues1,
                    cabacBinarizations1
            );
    return constructDecoderConfigurationTokentype(
            2,
            decoder_configuration_tokentype_cabac0,
            decoder_configuration_tokentype_cabac1
    );
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
    freeTransformSubseqParameters(transformSubseqParameters);

    transformSubseqParameters = constructEqualityCodingTransformSubseqParameters();
    responseCall = getTransformSubSeqId(transformSubseqParameters, &transformSubSeqId);
    EXPECT_EQ(responseCall, SUCCESS);
    EXPECT_EQ(transformSubSeqId, SubSeq_EQUALITY_CODING);

    responseCall = getMatchCodingBufferSize(transformSubseqParameters, &matchCoding);
    EXPECT_EQ(responseCall, FIELDs_EXISTANCE_CONDITIONS_NOT_MET);

    responseCall = getRLECodingGuard(transformSubseqParameters, &rleCodingGuard);
    EXPECT_EQ(responseCall, FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    freeTransformSubseqParameters(transformSubseqParameters);

    transformSubseqParameters = constructMatchCodingSubseqTransformSubseqParameters(expectedMatchCoding);
    responseCall = getTransformSubSeqId(transformSubseqParameters, &transformSubSeqId);
    EXPECT_EQ(responseCall, SUCCESS);
    EXPECT_EQ(transformSubSeqId, SubSeq_MATCH_CODING);

    responseCall = getMatchCodingBufferSize(transformSubseqParameters, &matchCoding);
    EXPECT_EQ(responseCall, SUCCESS);
    EXPECT_EQ(matchCoding, expectedMatchCoding);

    responseCall = getRLECodingGuard(transformSubseqParameters, &rleCodingGuard);
    EXPECT_EQ(responseCall, FIELDs_EXISTANCE_CONDITIONS_NOT_MET);
    freeTransformSubseqParameters(transformSubseqParameters);

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
    freeTransformSubseqParameters(transformSubseqParameters);
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
    uint8_t* argumentContextInitialization;
    uint8_t* contextInitialization;
    bool expectedShareSubSymCtxFlag = true;
    Cabac_context_parametersType* cabacContextParameters;


    argumentContextInitialization = (uint8_t*)malloc(sizeof(uint8_t) * expectedNumContexts);
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        argumentContextInitialization[context_i] = expectedContextInitialization[context_i];
    }

    Cabac_binarizationsType* cabacBinarization = constructCabacBinarizationBinaryCoding_NotBypass(
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            argumentContextInitialization,
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
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        EXPECT_EQ(argumentContextInitialization[context_i], expectedContextInitialization[context_i]);
    }

    freeCabac_binarization(cabacBinarization);

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
    freeCabac_binarization(cabacBinarization);

    argumentContextInitialization = (uint8_t*)malloc(sizeof(uint8_t) * expectedNumContexts);
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        argumentContextInitialization[context_i] = expectedContextInitialization[context_i];
    }
    cabacBinarization = constructCabacBinarizationTruncatedUnary_NotBypass(0,
                                                                           expectedAdaptive_mode_flag,
                                                                           expectedNumContexts,
                                                                           argumentContextInitialization,
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
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        EXPECT_EQ(argumentContextInitialization[context_i], expectedContextInitialization[context_i]);
    }
    freeCabac_binarization(cabacBinarization);

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
    freeCabac_binarization(cabacBinarization);

    argumentContextInitialization = (uint8_t*)malloc(sizeof(uint8_t) * expectedNumContexts);
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        argumentContextInitialization[context_i] = expectedContextInitialization[context_i];
    }
    cabacBinarization = constructCabacBinarizationExponentialGolomb_NotBypass(
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            argumentContextInitialization,
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
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        EXPECT_EQ(argumentContextInitialization[context_i], expectedContextInitialization[context_i]);
    }
    freeCabac_binarization(cabacBinarization);


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
    freeCabac_binarization(cabacBinarization);

    argumentContextInitialization = (uint8_t*)malloc(sizeof(uint8_t) * expectedNumContexts);
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        argumentContextInitialization[context_i] = expectedContextInitialization[context_i];
    }
    cabacBinarization = constructCabacBinarizationSignedExponentialGolomb_NotBypass(
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            argumentContextInitialization,
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
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        EXPECT_EQ(argumentContextInitialization[context_i], expectedContextInitialization[context_i]);
    }
    freeCabac_binarization(cabacBinarization);

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
    freeCabac_binarization(cabacBinarization);

    argumentContextInitialization = (uint8_t*)malloc(sizeof(uint8_t) * expectedNumContexts);
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        argumentContextInitialization[context_i] = expectedContextInitialization[context_i];
    }
    cabacBinarization = constructCabacBinarizationTruncatedExponentialGolomb_NotBypass(
            expectedCTruncExpGolParam,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            argumentContextInitialization,
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
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        EXPECT_EQ(argumentContextInitialization[context_i], expectedContextInitialization[context_i]);
    }
    freeCabac_binarization(cabacBinarization);


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
    freeCabac_binarization(cabacBinarization);

    argumentContextInitialization = (uint8_t*)malloc(sizeof(uint8_t) * expectedNumContexts);
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        argumentContextInitialization[context_i] = expectedContextInitialization[context_i];
    }
    cabacBinarization = constructCabacBinarizationSignedTruncatedExponentialGolomb_NotBypass(
            expectedCTruncExpGolParam,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            argumentContextInitialization,
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
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        EXPECT_EQ(argumentContextInitialization[context_i], expectedContextInitialization[context_i]);
    }
    freeCabac_binarization(cabacBinarization);

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
    freeCabac_binarization(cabacBinarization);

    argumentContextInitialization = (uint8_t*)malloc(sizeof(uint8_t) * expectedNumContexts);
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        argumentContextInitialization[context_i] = expectedContextInitialization[context_i];
    }
    cabacBinarization = constructCabacBinarizationSplitUnitwiseTruncatedUnary_NotBypass(
            expectedSplitUnitSize,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            argumentContextInitialization,
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
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        EXPECT_EQ(argumentContextInitialization[context_i], expectedContextInitialization[context_i]);
    }
    freeCabac_binarization(cabacBinarization);

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
    freeCabac_binarization(cabacBinarization);

    argumentContextInitialization = (uint8_t*)malloc(sizeof(uint8_t) * expectedNumContexts);
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        argumentContextInitialization[context_i] = expectedContextInitialization[context_i];
    }
    cabacBinarization = constructCabacBinarizationSignedSplitUnitwiseTruncatedUnary_NotBypass(
            expectedSplitUnitSize,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            argumentContextInitialization,
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
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        EXPECT_EQ(argumentContextInitialization[context_i], expectedContextInitialization[context_i]);
    }
    freeCabac_binarization(cabacBinarization);

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
    freeCabac_binarization(cabacBinarization);

    argumentContextInitialization = (uint8_t*)malloc(sizeof(uint8_t) * expectedNumContexts);
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        argumentContextInitialization[context_i] = expectedContextInitialization[context_i];
    }
    cabacBinarization = constructCabacBinarizationDoubleTruncatedUnary_NotBypass(
            expectedCMaxDTU,
            expectedSplitUnitSize,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            argumentContextInitialization,
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
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        EXPECT_EQ(argumentContextInitialization[context_i], expectedContextInitialization[context_i]);
    }
    freeCabac_binarization(cabacBinarization);

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
    freeCabac_binarization(cabacBinarization);



    argumentContextInitialization = (uint8_t*)malloc(sizeof(uint8_t) * expectedNumContexts);
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        argumentContextInitialization[context_i] = expectedContextInitialization[context_i];
    }
    cabacBinarization = constructCabacBinarizationSignedDoubleTruncatedUnary_NotBypass(
            expectedCMaxDTU,
            expectedSplitUnitSize,
            expectedAdaptive_mode_flag,
            expectedNumContexts,
            argumentContextInitialization,
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
    for(uint16_t context_i=0; context_i<expectedNumContexts; context_i++){
        EXPECT_EQ(argumentContextInitialization[context_i], expectedContextInitialization[context_i]);
    }
    freeCabac_binarization(cabacBinarization);

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
    freeCabac_binarization(cabacBinarization);
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
    freeDecoderConfigurationTypeCABAC(decoderConfigurationTypeCABAC, 20, 20);
}

/*TEST_F(encodingParametersTest, decoderConfigurationTokentypeCABAC)
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
}*/

TEST_F(encodingParametersTest, constructEncodingParametersSingleAlignmentNoComputedTest){
    uint8_t datasetType = 1;
    uint8_t alphabetId = 1;
    uint32_t reads_length = 100;
    uint8_t number_of_template_segments_minus1 = 1;
    uint32_t max_au_data_unit_size = 0xffff;
    bool pos40bits = false;
    uint8_t qv_depth = 3;
    uint8_t as_depth = 1;
    uint8_t numClasses = 1;
    auto * classIDs_argument = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        classIDs_argument[class_i] = 1;
    }
    auto * classIDs_check = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        classIDs_check[class_i] = 1;
    }
    uint16_t numGroups = 1;
    char** rgroupId_argument = (char**)malloc(sizeof(char*)*numGroups);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        rgroupId_argument[group_i] = (char*)malloc(sizeof(char)*12);
        strncpy(rgroupId_argument[group_i],"testGroupId",12);
    }
    char** rgroupId_check = (char**)malloc(sizeof(char*)*numGroups);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        rgroupId_check[group_i] = (char*)malloc(sizeof(char)*12);
        strncpy(rgroupId_check[group_i],"testGroupId",12);
    }
    bool multipleAlignmentFlag = false;
    bool splicedReadsFlag = true;
    bool multipleSignatureFlag = true;
    uint32_t multipleSignatureBase = 32;
    uint8_t U_signature_size = 3;
    bool crps_flag = false;
    Parameter_set_crpsType* parameterSetCrps = nullptr;

    uint8_t datasetType_buffer;
    uint8_t alphabetId_buffer;
    uint32_t reads_length_buffer;
    uint8_t number_of_template_segments_minus1_buffer;
    uint32_t max_au_data_unit_size_buffer;
    bool pos40bits_buffer;
    uint8_t qv_depth_buffer;
    uint8_t as_depth_buffer;
    uint8_t numClasses_buffer;
    uint8_t* classId_buffer;
    uint16_t numGroups_buffer;
    char* rgroupId_buffer;
    bool multipleAlignmentFlag_buffer;
    bool splicedReadsFlag_buffer;
    bool multipleSignatureFlag_buffer;
    uint32_t multipleSignatureBase_buffer;
    uint8_t U_signature_size_buffer;
    bool crps_flag_buffer;
    Parameter_set_crpsType* parameterSetCrps_buffer;

    Encoding_ParametersType* encodingParameters = constructEncodingParametersSingleAlignmentNoComputed(
            datasetType,
            alphabetId,
            reads_length,
            number_of_template_segments_minus1,
            max_au_data_unit_size,
            pos40bits,
            qv_depth,
            as_depth,
            numClasses,
            classIDs_argument,
            numGroups,
            (char**)rgroupId_argument,
            splicedReadsFlag,
            multipleSignatureFlag,
            multipleSignatureBase,
            U_signature_size
    );

    EXPECT_NE(nullptr, encodingParameters);
    EXPECT_EQ(SUCCESS, getDatasetTypeParameters(encodingParameters, &datasetType_buffer));
    EXPECT_EQ(datasetType, datasetType_buffer);
    EXPECT_EQ(SUCCESS, getAlphabetID(encodingParameters, &alphabetId_buffer));
    EXPECT_EQ(alphabetId, alphabetId_buffer);
    EXPECT_EQ(SUCCESS, getReadsLength(encodingParameters, &reads_length_buffer));
    EXPECT_EQ(reads_length, reads_length_buffer);
    EXPECT_EQ(SUCCESS, getNumberOfTemplateSegmentsMinus1(encodingParameters, &number_of_template_segments_minus1_buffer));
    EXPECT_EQ(number_of_template_segments_minus1, number_of_template_segments_minus1_buffer);
    EXPECT_EQ(SUCCESS, getMaxAUDataUnitSize(encodingParameters, &max_au_data_unit_size_buffer));
    EXPECT_EQ(max_au_data_unit_size_buffer, max_au_data_unit_size);
    EXPECT_EQ(SUCCESS, getPos40Bits(encodingParameters, &pos40bits_buffer));
    EXPECT_EQ(pos40bits, pos40bits_buffer);
    EXPECT_EQ(SUCCESS, getQVDepth(encodingParameters, &qv_depth_buffer));
    EXPECT_EQ(qv_depth, qv_depth_buffer);
    EXPECT_EQ(SUCCESS, getASDepth(encodingParameters, &as_depth_buffer));
    EXPECT_EQ(as_depth, as_depth_buffer);
    EXPECT_EQ(SUCCESS, getNumClasses(encodingParameters, &numClasses_buffer));
    EXPECT_EQ(numClasses, numClasses_buffer);
    EXPECT_EQ(SUCCESS, getClassIds(encodingParameters, &numClasses_buffer, &classId_buffer));
    EXPECT_EQ(numClasses, numClasses_buffer);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        EXPECT_EQ(classIDs_check[class_i], classId_buffer[class_i]);
    }

    /*EncodingParametersRC getCABACDecoderConfiguration(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t descriptorID,
            DecoderConfigurationTypeCABAC** decoder_configuration_cabac
    );
    EncodingParametersRC getCABACDecoderConfigurationTokenType(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t descriptorID,
            Decoder_configuration_tokentype** decoderConfigurationTokentypeCABAC
    );*/
    EXPECT_EQ(SUCCESS, getNumGroups(encodingParameters, &numGroups_buffer));
    EXPECT_EQ(numGroups, numGroups_buffer);
    for(uint8_t group_i=0; group_i<numGroups; group_i++){
        EXPECT_EQ(SUCCESS, getReadGroupId(encodingParameters, group_i, &rgroupId_buffer));
        EXPECT_STREQ(rgroupId_check[group_i], rgroupId_buffer);
    }
    EXPECT_EQ(SUCCESS, getMultipleAlignments_flag(encodingParameters, &multipleAlignmentFlag_buffer));
    EXPECT_EQ(multipleAlignmentFlag, multipleAlignmentFlag_buffer);
    EXPECT_EQ(SUCCESS, getSplicedReadsFlag(encodingParameters, &splicedReadsFlag_buffer));
    EXPECT_EQ(splicedReadsFlag, splicedReadsFlag_buffer);
    EXPECT_EQ(SUCCESS, getMultipleSignatureBaseParameters(encodingParameters,&multipleSignatureBase_buffer));
    EXPECT_EQ(multipleSignatureBase, multipleSignatureBase_buffer);
    EXPECT_EQ(SUCCESS, getSignatureSize(encodingParameters, &U_signature_size_buffer));
    EXPECT_EQ(U_signature_size, U_signature_size_buffer);
    /*EncodingParametersRC getQVCodingMode(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t* qv_coding_mode
    );*/
    /*EncodingParametersRC getQVPSflag(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            bool* qvps_flag
    );*/
    /*EncodingParametersRC getParameterSetQvps(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            Parameter_set_qvpsType** pParameter_set_qvpsType
    );*/
    /*EncodingParametersRC getQvps_preset_id(
            Encoding_ParametersType *encodingParameters,
            uint8_t classIndex,
            uint8_t *qvps_preset_ID
    );*/
    EXPECT_EQ(SUCCESS, getCrpsFlag(encodingParameters, &crps_flag_buffer));
    EXPECT_EQ(crps_flag, crps_flag_buffer);
    EXPECT_EQ(FIELDs_EXISTANCE_CONDITIONS_NOT_MET, getParameterSetCrps(encodingParameters, &parameterSetCrps_buffer));


    free(classIDs_check);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        free(rgroupId_check[group_i]);
    }
    free(rgroupId_check);
    freeEncodingParameters(encodingParameters);

}

TEST_F(encodingParametersTest, constructEncodingParametersMultipleAlignmentNoComputedTest){
    uint8_t datasetType = 1;
    uint8_t alphabetId = 1;
    uint32_t reads_length = 100;
    uint8_t number_of_template_segments_minus1 = 1;
    uint32_t max_au_data_unit_size = 0xffff;
    bool pos40bits = false;
    uint8_t qv_depth = 3;
    uint8_t as_depth = 1;
    uint8_t numClasses = 1;
    auto * classIDs_argument = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        classIDs_argument[class_i] = 1;
    }
    auto * classIDs_check = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        classIDs_check[class_i] = 1;
    }
    uint16_t numGroups = 1;
    char** rgroupId_argument = (char**)malloc(sizeof(char*)*numGroups);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        rgroupId_argument[group_i] = (char*)malloc(sizeof(char)*12);
        strncpy(rgroupId_argument[group_i],"testGroupId",12);
    }
    char** rgroupId_check = (char**)malloc(sizeof(char*)*numGroups);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        rgroupId_check[group_i] = (char*)malloc(sizeof(char)*12);
        strncpy(rgroupId_check[group_i],"testGroupId",12);
    }
    bool multipleAlignmentFlag = true;
    bool splicedReadsFlag = true;
    uint32_t multipleSignatureBase = 32;
    uint8_t U_signature_size = 3;
    bool crps_flag = false;

    uint8_t datasetType_buffer;
    uint8_t alphabetId_buffer;
    uint32_t reads_length_buffer;
    uint8_t number_of_template_segments_minus1_buffer;
    uint32_t max_au_data_unit_size_buffer;
    bool pos40bits_buffer;
    uint8_t qv_depth_buffer;
    uint8_t as_depth_buffer;
    uint8_t numClasses_buffer;
    uint8_t* classId_buffer;
    uint16_t numGroups_buffer;
    char* rgroupId_buffer;
    bool multipleAlignmentFlag_buffer;
    bool splicedReadsFlag_buffer;
    uint32_t multipleSignatureBase_buffer;
    uint8_t U_signature_size_buffer;
    bool crps_flag_buffer;
    Parameter_set_crpsType* parameterSetCrps_buffer;

    Encoding_ParametersType* encodingParameters = constructEncodingParametersMultipleAlignmentNoComputed(
            datasetType,
            alphabetId,
            reads_length,
            number_of_template_segments_minus1,
            max_au_data_unit_size,
            pos40bits,
            qv_depth,
            as_depth,
            numClasses,
            classIDs_argument,
            numGroups,
            (char **) rgroupId_argument,
            splicedReadsFlag,
            multipleSignatureBase,
            U_signature_size);

    EXPECT_NE(nullptr, encodingParameters);
    EXPECT_EQ(SUCCESS, getDatasetTypeParameters(encodingParameters, &datasetType_buffer));
    EXPECT_EQ(datasetType, datasetType_buffer);
    EXPECT_EQ(SUCCESS, getAlphabetID(encodingParameters, &alphabetId_buffer));
    EXPECT_EQ(alphabetId, alphabetId_buffer);
    EXPECT_EQ(SUCCESS, getReadsLength(encodingParameters, &reads_length_buffer));
    EXPECT_EQ(reads_length, reads_length_buffer);
    EXPECT_EQ(SUCCESS, getNumberOfTemplateSegmentsMinus1(encodingParameters, &number_of_template_segments_minus1_buffer));
    EXPECT_EQ(number_of_template_segments_minus1, number_of_template_segments_minus1_buffer);
    EXPECT_EQ(SUCCESS, getMaxAUDataUnitSize(encodingParameters, &max_au_data_unit_size_buffer));
    EXPECT_EQ(max_au_data_unit_size_buffer, max_au_data_unit_size);
    EXPECT_EQ(SUCCESS, getPos40Bits(encodingParameters, &pos40bits_buffer));
    EXPECT_EQ(pos40bits, pos40bits_buffer);
    EXPECT_EQ(SUCCESS, getQVDepth(encodingParameters, &qv_depth_buffer));
    EXPECT_EQ(qv_depth, qv_depth_buffer);
    EXPECT_EQ(SUCCESS, getASDepth(encodingParameters, &as_depth_buffer));
    EXPECT_EQ(as_depth, as_depth_buffer);
    EXPECT_EQ(SUCCESS, getNumClasses(encodingParameters, &numClasses_buffer));
    EXPECT_EQ(numClasses, numClasses_buffer);
    EXPECT_EQ(SUCCESS, getClassIds(encodingParameters, &numClasses_buffer, &classId_buffer));
    EXPECT_EQ(numClasses, numClasses_buffer);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        EXPECT_EQ(classIDs_check[class_i], classId_buffer[class_i]);
    }
    /*EncodingParametersRC getCABACDecoderConfiguration(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t descriptorID,
            DecoderConfigurationTypeCABAC** decoder_configuration_cabac
    );
    EncodingParametersRC getCABACDecoderConfigurationTokenType(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t descriptorID,
            Decoder_configuration_tokentype** decoderConfigurationTokentypeCABAC
    );*/
    EXPECT_EQ(SUCCESS, getNumGroups(encodingParameters, &numGroups_buffer));
    EXPECT_EQ(numGroups, numGroups_buffer);
    for(uint8_t group_i=0; group_i<numGroups; group_i++){
        EXPECT_EQ(SUCCESS, getReadGroupId(encodingParameters, group_i, &rgroupId_buffer));
        EXPECT_STREQ(rgroupId_check[group_i], rgroupId_buffer);
    }
    EXPECT_EQ(SUCCESS, getMultipleAlignments_flag(encodingParameters, &multipleAlignmentFlag_buffer));
    EXPECT_EQ(multipleAlignmentFlag, multipleAlignmentFlag_buffer);
    EXPECT_EQ(SUCCESS, getSplicedReadsFlag(encodingParameters, &splicedReadsFlag_buffer));
    EXPECT_EQ(splicedReadsFlag, splicedReadsFlag_buffer);
    EXPECT_EQ(SUCCESS, getMultipleSignatureBaseParameters(encodingParameters,&multipleSignatureBase_buffer));
    EXPECT_EQ(multipleSignatureBase, multipleSignatureBase_buffer);
    EXPECT_EQ(SUCCESS, getSignatureSize(encodingParameters, &U_signature_size_buffer));
    EXPECT_EQ(U_signature_size, U_signature_size_buffer);
    /*EncodingParametersRC getQVCodingMode(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t* qv_coding_mode
    );*/
    /*EncodingParametersRC getQVPSflag(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            bool* qvps_flag
    );*/
    /*EncodingParametersRC getParameterSetQvps(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            Parameter_set_qvpsType** pParameter_set_qvpsType
    );*/
    /*EncodingParametersRC getQvps_preset_id(
            Encoding_ParametersType *encodingParameters,
            uint8_t classIndex,
            uint8_t *qvps_preset_ID
    );*/
    EXPECT_EQ(SUCCESS, getCrpsFlag(encodingParameters, &crps_flag_buffer));
    EXPECT_EQ(crps_flag, crps_flag_buffer);
    EXPECT_EQ(FIELDs_EXISTANCE_CONDITIONS_NOT_MET, getParameterSetCrps(encodingParameters, &parameterSetCrps_buffer));


    free(classIDs_check);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        free(rgroupId_check[group_i]);
    }
    free(rgroupId_check);
    freeEncodingParameters(encodingParameters);

}

TEST_F(encodingParametersTest, constructEncodingParametersSingleAlignmentComputedRefTest){
    uint8_t datasetType = 1;
    uint8_t alphabetId = 1;
    uint32_t reads_length = 100;
    uint8_t number_of_template_segments_minus1 = 1;
    uint32_t max_au_data_unit_size = 0xffff;
    bool pos40bits = false;
    uint8_t qv_depth = 3;
    uint8_t as_depth = 1;
    uint8_t numClasses = 1;
    auto * classIDs_argument = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        classIDs_argument[class_i] = 1;
    }
    auto * classIDs_check = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        classIDs_check[class_i] = 1;
    }
    uint16_t numGroups = 1;
    char** rgroupId_argument = (char**)malloc(sizeof(char*)*numGroups);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        rgroupId_argument[group_i] = (char*)malloc(sizeof(char)*12);
        strncpy(rgroupId_argument[group_i],"testGroupId",12);
    }
    char** rgroupId_check = (char**)malloc(sizeof(char*)*numGroups);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        rgroupId_check[group_i] = (char*)malloc(sizeof(char)*12);
        strncpy(rgroupId_check[group_i],"testGroupId",12);
    }
    bool multipleAlignmentFlag = false;
    bool splicedReadsFlag = true;
    uint32_t multipleSignatureBase = 32;
    uint8_t U_signature_size = 3;
    bool crps_flag = true;
    Parameter_set_crpsType* parameterSetCrps = constructParameterSetCrps(CrAlg_GlobalAssembly, 1, 8);


    uint8_t datasetType_buffer;
    uint8_t alphabetId_buffer;
    uint32_t reads_length_buffer;
    uint8_t number_of_template_segments_minus1_buffer;
    uint32_t max_au_data_unit_size_buffer;
    bool pos40bits_buffer;
    uint8_t qv_depth_buffer;
    uint8_t as_depth_buffer;
    uint8_t numClasses_buffer;
    uint8_t* classId_buffer;
    uint16_t numGroups_buffer;
    char* rgroupId_buffer;
    bool multipleAlignmentFlag_buffer;
    bool splicedReadsFlag_buffer;
    uint32_t multipleSignatureBase_buffer;
    uint8_t U_signature_size_buffer;
    bool crps_flag_buffer;
    Parameter_set_crpsType* parameterSetCrps_buffer;

    Encoding_ParametersType* encodingParameters = constructEncodingParametersSingleAlignmentComputedRef(
            datasetType,
            alphabetId,
            reads_length,
            number_of_template_segments_minus1,
            max_au_data_unit_size,
            pos40bits,
            qv_depth,
            as_depth,
            numClasses,
            classIDs_argument,
            numGroups,
            (char **) rgroupId_argument,
            splicedReadsFlag,
            multipleSignatureBase,
            U_signature_size, parameterSetCrps);

    EXPECT_NE(nullptr, encodingParameters);
    EXPECT_EQ(SUCCESS, getDatasetTypeParameters(encodingParameters, &datasetType_buffer));
    EXPECT_EQ(datasetType, datasetType_buffer);
    EXPECT_EQ(SUCCESS, getAlphabetID(encodingParameters, &alphabetId_buffer));
    EXPECT_EQ(alphabetId, alphabetId_buffer);
    EXPECT_EQ(SUCCESS, getReadsLength(encodingParameters, &reads_length_buffer));
    EXPECT_EQ(reads_length, reads_length_buffer);
    EXPECT_EQ(SUCCESS, getNumberOfTemplateSegmentsMinus1(encodingParameters, &number_of_template_segments_minus1_buffer));
    EXPECT_EQ(number_of_template_segments_minus1, number_of_template_segments_minus1_buffer);
    EXPECT_EQ(SUCCESS, getMaxAUDataUnitSize(encodingParameters, &max_au_data_unit_size_buffer));
    EXPECT_EQ(max_au_data_unit_size_buffer, max_au_data_unit_size);
    EXPECT_EQ(SUCCESS, getPos40Bits(encodingParameters, &pos40bits_buffer));
    EXPECT_EQ(pos40bits, pos40bits_buffer);
    EXPECT_EQ(SUCCESS, getQVDepth(encodingParameters, &qv_depth_buffer));
    EXPECT_EQ(qv_depth, qv_depth_buffer);
    EXPECT_EQ(SUCCESS, getASDepth(encodingParameters, &as_depth_buffer));
    EXPECT_EQ(as_depth, as_depth_buffer);
    EXPECT_EQ(SUCCESS, getNumClasses(encodingParameters, &numClasses_buffer));
    EXPECT_EQ(numClasses, numClasses_buffer);
    EXPECT_EQ(SUCCESS, getClassIds(encodingParameters, &numClasses_buffer, &classId_buffer));
    EXPECT_EQ(numClasses, numClasses_buffer);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        EXPECT_EQ(classIDs_check[class_i], classId_buffer[class_i]);
    }
    /*EncodingParametersRC getCABACDecoderConfiguration(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t descriptorID,
            DecoderConfigurationTypeCABAC** decoder_configuration_cabac
    );
    EncodingParametersRC getCABACDecoderConfigurationTokenType(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t descriptorID,
            Decoder_configuration_tokentype** decoderConfigurationTokentypeCABAC
    );*/
    EXPECT_EQ(SUCCESS, getNumGroups(encodingParameters, &numGroups_buffer));
    EXPECT_EQ(numGroups, numGroups_buffer);
    for(uint8_t group_i=0; group_i<numGroups; group_i++){
        EXPECT_EQ(SUCCESS, getReadGroupId(encodingParameters, group_i, &rgroupId_buffer));
        EXPECT_STREQ(rgroupId_check[group_i], rgroupId_buffer);
    }
    EXPECT_EQ(SUCCESS, getMultipleAlignments_flag(encodingParameters, &multipleAlignmentFlag_buffer));
    EXPECT_EQ(multipleAlignmentFlag, multipleAlignmentFlag_buffer);
    EXPECT_EQ(SUCCESS, getSplicedReadsFlag(encodingParameters, &splicedReadsFlag_buffer));
    EXPECT_EQ(splicedReadsFlag, splicedReadsFlag_buffer);
    EXPECT_EQ(SUCCESS, getMultipleSignatureBaseParameters(encodingParameters,&multipleSignatureBase_buffer));
    EXPECT_EQ(multipleSignatureBase, multipleSignatureBase_buffer);
    EXPECT_EQ(SUCCESS, getSignatureSize(encodingParameters, &U_signature_size_buffer));
    EXPECT_EQ(U_signature_size, U_signature_size_buffer);
    /*EncodingParametersRC getQVCodingMode(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t* qv_coding_mode
    );*/
    /*EncodingParametersRC getQVPSflag(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            bool* qvps_flag
    );*/
    /*EncodingParametersRC getParameterSetQvps(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            Parameter_set_qvpsType** pParameter_set_qvpsType
    );*/
    /*EncodingParametersRC getQvps_preset_id(
            Encoding_ParametersType *encodingParameters,
            uint8_t classIndex,
            uint8_t *qvps_preset_ID
    );*/
    EXPECT_EQ(SUCCESS, getCrpsFlag(encodingParameters, &crps_flag_buffer));
    EXPECT_EQ(crps_flag, crps_flag_buffer);
    EXPECT_EQ(SUCCESS, getParameterSetCrps(encodingParameters, &parameterSetCrps_buffer));
    EXPECT_EQ(parameterSetCrps, parameterSetCrps_buffer);


    free(classIDs_check);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        free(rgroupId_check[group_i]);
    }
    free(rgroupId_check);
    freeEncodingParameters(encodingParameters);

}

TEST_F(encodingParametersTest, constructEncodingParametersMultipleAlignmentComputedRefTest){
    uint8_t datasetType = 1;
    uint8_t alphabetId = 1;
    uint32_t reads_length = 100;
    uint8_t number_of_template_segments_minus1 = 1;
    uint32_t max_au_data_unit_size = 0xffff;
    bool pos40bits = false;
    uint8_t qv_depth = 3;
    uint8_t as_depth = 1;
    uint8_t numClasses = 1;
    auto * classIDs_argument = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        classIDs_argument[class_i] = 1;
    }
    auto * classIDs_check = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        classIDs_check[class_i] = 1;
    }
    uint16_t numGroups = 1;
    char** rgroupId_argument = (char**)malloc(sizeof(char*)*numGroups);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        rgroupId_argument[group_i] = (char*)malloc(sizeof(char)*12);
        strncpy(rgroupId_argument[group_i],"testGroupId",12);
    }
    char** rgroupId_check = (char**)malloc(sizeof(char*)*numGroups);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        rgroupId_check[group_i] = (char*)malloc(sizeof(char)*12);
        strncpy(rgroupId_check[group_i],"testGroupId",12);
    }
    bool multipleAlignmentFlag = true;
    bool splicedReadsFlag = true;
    uint32_t multipleSignatureBase = 32;
    uint8_t U_signature_size = 3;
    bool crps_flag = true;
    Parameter_set_crpsType* parameterSetCrps = constructParameterSetCrps(CrAlg_GlobalAssembly, 1, 8);


    uint8_t datasetType_buffer;
    uint8_t alphabetId_buffer;
    uint32_t reads_length_buffer;
    uint8_t number_of_template_segments_minus1_buffer;
    uint32_t max_au_data_unit_size_buffer;
    bool pos40bits_buffer;
    uint8_t qv_depth_buffer;
    uint8_t as_depth_buffer;
    uint8_t numClasses_buffer;
    uint8_t* classId_buffer;
    uint16_t numGroups_buffer;
    char* rgroupId_buffer;
    bool multipleAlignmentFlag_buffer;
    bool splicedReadsFlag_buffer;
    uint32_t multipleSignatureBase_buffer;
    uint8_t U_signature_size_buffer;
    bool crps_flag_buffer;
    Parameter_set_crpsType* parameterSetCrps_buffer;

    Encoding_ParametersType* encodingParameters = constructEncodingParametersMultipleAlignmentComputedRef(
            datasetType,
            alphabetId,
            reads_length,
            number_of_template_segments_minus1,
            max_au_data_unit_size,
            pos40bits,
            qv_depth,
            as_depth,
            numClasses,
            classIDs_argument,
            numGroups,
            (char **) rgroupId_argument,
            splicedReadsFlag,
            multipleSignatureBase,
            U_signature_size, parameterSetCrps);

    EXPECT_NE(nullptr, encodingParameters);
    EXPECT_EQ(SUCCESS, getDatasetTypeParameters(encodingParameters, &datasetType_buffer));
    EXPECT_EQ(datasetType, datasetType_buffer);
    EXPECT_EQ(SUCCESS, getAlphabetID(encodingParameters, &alphabetId_buffer));
    EXPECT_EQ(alphabetId, alphabetId_buffer);
    EXPECT_EQ(SUCCESS, getReadsLength(encodingParameters, &reads_length_buffer));
    EXPECT_EQ(reads_length, reads_length_buffer);
    EXPECT_EQ(SUCCESS, getNumberOfTemplateSegmentsMinus1(encodingParameters, &number_of_template_segments_minus1_buffer));
    EXPECT_EQ(number_of_template_segments_minus1, number_of_template_segments_minus1_buffer);
    EXPECT_EQ(SUCCESS, getMaxAUDataUnitSize(encodingParameters, &max_au_data_unit_size_buffer));
    EXPECT_EQ(max_au_data_unit_size_buffer, max_au_data_unit_size);
    EXPECT_EQ(SUCCESS, getPos40Bits(encodingParameters, &pos40bits_buffer));
    EXPECT_EQ(pos40bits, pos40bits_buffer);
    EXPECT_EQ(SUCCESS, getQVDepth(encodingParameters, &qv_depth_buffer));
    EXPECT_EQ(qv_depth, qv_depth_buffer);
    EXPECT_EQ(SUCCESS, getASDepth(encodingParameters, &as_depth_buffer));
    EXPECT_EQ(as_depth, as_depth_buffer);
    EXPECT_EQ(SUCCESS, getNumClasses(encodingParameters, &numClasses_buffer));
    EXPECT_EQ(numClasses, numClasses_buffer);
    EXPECT_EQ(SUCCESS, getClassIds(encodingParameters, &numClasses_buffer, &classId_buffer));
    EXPECT_EQ(numClasses, numClasses_buffer);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        EXPECT_EQ(classIDs_check[class_i], classId_buffer[class_i]);
    }
    /*EncodingParametersRC getCABACDecoderConfiguration(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t descriptorID,
            DecoderConfigurationTypeCABAC** decoder_configuration_cabac
    );
    EncodingParametersRC getCABACDecoderConfigurationTokenType(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t descriptorID,
            Decoder_configuration_tokentype** decoderConfigurationTokentypeCABAC
    );*/
    EXPECT_EQ(SUCCESS, getNumGroups(encodingParameters, &numGroups_buffer));
    EXPECT_EQ(numGroups, numGroups_buffer);
    for(uint8_t group_i=0; group_i<numGroups; group_i++){
        EXPECT_EQ(SUCCESS, getReadGroupId(encodingParameters, group_i, &rgroupId_buffer));
        EXPECT_STREQ(rgroupId_check[group_i], rgroupId_buffer);
    }
    EXPECT_EQ(SUCCESS, getMultipleAlignments_flag(encodingParameters, &multipleAlignmentFlag_buffer));
    EXPECT_EQ(multipleAlignmentFlag, multipleAlignmentFlag_buffer);
    EXPECT_EQ(SUCCESS, getSplicedReadsFlag(encodingParameters, &splicedReadsFlag_buffer));
    EXPECT_EQ(splicedReadsFlag, splicedReadsFlag_buffer);
    EXPECT_EQ(SUCCESS, getMultipleSignatureBaseParameters(encodingParameters,&multipleSignatureBase_buffer));
    EXPECT_EQ(multipleSignatureBase, multipleSignatureBase_buffer);
    EXPECT_EQ(SUCCESS, getSignatureSize(encodingParameters, &U_signature_size_buffer));
    EXPECT_EQ(U_signature_size, U_signature_size_buffer);
    /*EncodingParametersRC getQVCodingMode(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            uint8_t* qv_coding_mode
    );*/
    /*EncodingParametersRC getQVPSflag(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            bool* qvps_flag
    );*/
    /*EncodingParametersRC getParameterSetQvps(
            Encoding_ParametersType* encodingParameters,
            uint8_t classIndex,
            Parameter_set_qvpsType** pParameter_set_qvpsType
    );*/
    /*EncodingParametersRC getQvps_preset_id(
            Encoding_ParametersType *encodingParameters,
            uint8_t classIndex,
            uint8_t *qvps_preset_ID
    );*/
    EXPECT_EQ(SUCCESS, getCrpsFlag(encodingParameters, &crps_flag_buffer));
    EXPECT_EQ(crps_flag, crps_flag_buffer);
    EXPECT_EQ(SUCCESS, getParameterSetCrps(encodingParameters, &parameterSetCrps_buffer));
    EXPECT_EQ(parameterSetCrps, parameterSetCrps_buffer);


    free(classIDs_check);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        free(rgroupId_check[group_i]);
    }
    free(rgroupId_check);
    freeEncodingParameters(encodingParameters);

}

TEST_F(encodingParametersTest, writeEncodingParametersSingleAlignmentNoComputedTest){
    uint8_t datasetType = 1;
    uint8_t alphabetId = 1;
    uint32_t reads_length = 100;
    uint8_t number_of_template_segments_minus1 = 1;
    uint32_t max_au_data_unit_size = 0xffff;
    bool pos40bits = false;
    uint8_t qv_depth = 3;
    uint8_t as_depth = 1;
    uint8_t numClasses = 2;
    auto * classIDs_argument = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        classIDs_argument[class_i] = (uint8_t)(class_i+1);
    }
    auto * classIDs_check = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        classIDs_check[class_i] = (uint8_t)(class_i+1);
    }
    uint16_t numGroups = 1;
    char** rgroupId_argument = (char**)malloc(sizeof(char*)*numGroups);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        rgroupId_argument[group_i] = (char*)malloc(sizeof(char)*12);
        strncpy(rgroupId_argument[group_i],"testGroupId",12);
    }
    char** rgroupId_check = (char**)malloc(sizeof(char*)*numGroups);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        rgroupId_check[group_i] = (char*)malloc(sizeof(char)*12);
        strncpy(rgroupId_check[group_i],"testGroupId",12);
    }
    bool splicedReadsFlag = true;
    bool multipleSignatureFlag = false;
    uint32_t multipleSignatureBase = 32;
    uint8_t U_signature_size = 3;

    Encoding_ParametersType* encodingParameters = constructEncodingParametersSingleAlignmentNoComputed(
            datasetType,
            alphabetId,
            reads_length,
            number_of_template_segments_minus1,
            max_au_data_unit_size,
            pos40bits,
            qv_depth,
            as_depth,
            numClasses,
            classIDs_argument,
            numGroups,
            (char**)rgroupId_argument,
            splicedReadsFlag,
            multipleSignatureFlag,
            multipleSignatureBase,
            U_signature_size
    );

    for(uint8_t descriptor_i=0; descriptor_i<18; descriptor_i++){
        if(descriptor_i != 11 && descriptor_i != 15){
            if(descriptor_i%2 == 0){
                DecoderConfigurationTypeCABAC* testingDecoderConf = getTestingDecoderConfigurationTypeCABAC();
                setNonClassSpecificDescriptorConfigurationAndEncodingMode(
                        encodingParameters,
                        descriptor_i,
                        testingDecoderConf,
                        0
                );
            }else{
                for(uint8_t class_i=0; class_i<numClasses; class_i++){
                    DecoderConfigurationTypeCABAC* testingDecoderConf = getTestingDecoderConfigurationTypeCABAC();
                    setClassSpecificDescriptorConfigurationAndEncodingMode(
                            encodingParameters,
                            class_i,
                            descriptor_i,
                            testingDecoderConf,
                            0
                    );
                }
            }
        }else{
            Decoder_configuration_tokentype* configurationTokentype = getTestingDecoderConfigurationTokenType();
            setNonClassSpecificDescriptorConfigurationAndEncodingMode(
                    encodingParameters, descriptor_i, configurationTokentype, 0
            );
        }
    }



    FILE* outputFile = fopen("testFile","wb");
    ASSERT_TRUE(outputFile != NULL);
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);

    writeEncoding_parameters(encodingParameters, &outputBitstream);
    writeBuffer(&outputBitstream);
    fclose(outputFile);

    FILE* inputFile = fopen("testFile","rb");
    ASSERT_TRUE(inputFile != NULL);
    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);
    Encoding_ParametersType* encodingParametersType = readEncodingParameters(&inputBitstream);


    uint8_t datasetType_buffer;
    uint8_t alphabetId_buffer;
    uint32_t reads_length_buffer;
    uint8_t number_of_template_segments_minus1_buffer;
    uint32_t max_au_data_unit_size_buffer;
    bool pos40bits_buffer;
    uint8_t qv_depth_buffer;
    uint8_t as_depth_buffer;
    uint8_t numClasses_buffer;
    uint8_t* classIds_buffer;
    uint16_t numGroups_buffer;
    char* rgroupId_buffer;
    bool splicedReadsFlag_buffer;
    bool multipleSignatureFlag_buffer;
    uint32_t multipleSignatureBase_buffer;
    uint8_t U_signature_size_buffer;

    EXPECT_EQ(SUCCESS, getDatasetTypeParameters(encodingParameters, &datasetType_buffer));
    EXPECT_EQ(datasetType, datasetType_buffer);
    EXPECT_EQ(SUCCESS, getAlphabetID(encodingParameters, &alphabetId_buffer));
    EXPECT_EQ(alphabetId, alphabetId_buffer);
    EXPECT_EQ(SUCCESS, getReadsLength(encodingParameters, &reads_length_buffer));
    EXPECT_EQ(reads_length, reads_length_buffer);
    EXPECT_EQ(SUCCESS,
            getNumberOfTemplateSegmentsMinus1(encodingParameters, &number_of_template_segments_minus1_buffer)
    );
    EXPECT_EQ(number_of_template_segments_minus1, number_of_template_segments_minus1_buffer);
    EXPECT_EQ(SUCCESS, getMaxAUDataUnitSize(encodingParameters, &max_au_data_unit_size_buffer));
    EXPECT_EQ(max_au_data_unit_size, max_au_data_unit_size_buffer);
    EXPECT_EQ(SUCCESS, getPos40Bits(encodingParameters, &pos40bits_buffer));
    EXPECT_EQ(pos40bits, pos40bits_buffer);
    EXPECT_EQ(SUCCESS, getQVDepth(encodingParameters, &qv_depth_buffer));
    EXPECT_EQ(qv_depth, qv_depth_buffer);
    EXPECT_EQ(SUCCESS, getASDepth(encodingParameters, &as_depth_buffer));
    EXPECT_EQ(as_depth, as_depth_buffer);
    EXPECT_EQ(SUCCESS, getNumClasses(encodingParameters, &numClasses_buffer));
    EXPECT_EQ(numClasses, numClasses_buffer);
    EXPECT_EQ(SUCCESS, getClassIds(encodingParameters, &numClasses_buffer, &classIds_buffer));
    EXPECT_EQ(numClasses, numClasses_buffer);
    for(uint8_t class_i=0; class_i<numClasses; class_i++){
        EXPECT_EQ(classIDs_check[class_i],classIds_buffer[class_i]);
    }
    EXPECT_EQ(SUCCESS, getNumGroups(encodingParameters, &numGroups_buffer));
    EXPECT_EQ(numGroups, numGroups_buffer);
    for(uint8_t rgroup_i=0; rgroup_i<numGroups; rgroup_i++){
        EXPECT_EQ(SUCCESS, getReadGroupId(encodingParameters, rgroup_i, &rgroupId_buffer));
        EXPECT_STREQ(rgroupId_check[rgroup_i], rgroupId_buffer);
    }

    EXPECT_EQ(SUCCESS, getSplicedReadsFlag(encodingParameters, &splicedReadsFlag_buffer));
    EXPECT_EQ(splicedReadsFlag, splicedReadsFlag_buffer);
    EXPECT_EQ(SUCCESS, getMultipleAlignments_flag(encodingParameters, &multipleSignatureFlag_buffer));
    EXPECT_EQ(multipleSignatureFlag, multipleSignatureFlag_buffer);
    EXPECT_EQ(SUCCESS, getMultipleSignatureBaseParameters(encodingParameters, &multipleSignatureBase_buffer));
    EXPECT_EQ(multipleSignatureBase, multipleSignatureBase_buffer);
    EXPECT_EQ(SUCCESS, getSignatureSize(encodingParameters, &U_signature_size_buffer));
    EXPECT_EQ(U_signature_size, U_signature_size_buffer);


    DecoderConfigurationTypeCABAC* decoder_configuration_cabac_buffer;
    EXPECT_EQ(OUT_OF_BOUNDERIES, getCABACDecoderConfiguration(encodingParameters, 6, 0, &decoder_configuration_cabac_buffer));
    EXPECT_EQ(OUT_OF_BOUNDERIES, getCABACDecoderConfiguration(encodingParameters, 0, 19, &decoder_configuration_cabac_buffer));
    EXPECT_EQ(OUT_OF_BOUNDERIES, getCABACDecoderConfiguration(encodingParameters, 0, 11, &decoder_configuration_cabac_buffer));

    EXPECT_EQ(SUCCESS, getCABACDecoderConfiguration(encodingParameters, 0, 0, &decoder_configuration_cabac_buffer));


    free(classIDs_check);
    for(uint16_t group_i=0; group_i < numGroups; group_i++){
        free(rgroupId_check[group_i]);
    }
    free(rgroupId_check);
    freeEncodingParameters(encodingParameters);

}