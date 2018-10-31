//
// Created by daniel on 23/10/18.
//

#include "format/EncodingParameters.h"
#include <stdlib.h>
#include "format/DataStructures/BitStreams/OutputBitstream.h"
#include <string.h>

EncodingParametersRC getCorrectedTransform_subseq_counter(
        Transform_subseq_parametersType* transform_subseq_parametersType,
        uint8_t* transform_subseq_counter
);

EncodingParametersRC getCorrectedTransform_subseq_counter(
    Transform_subseq_parametersType* transform_subseq_parametersType,
    uint8_t* transform_subseq_counter
){
    if(transform_subseq_parametersType == NULL || transform_subseq_counter == NULL){
        return NULL_PTR;
    }

    if(transform_subseq_parametersType->transform_ID_subseq == SubSeq_EQUALITY_CODING){
        *transform_subseq_counter = 2;
    }else if(transform_subseq_parametersType->transform_ID_subseq == SubSeq_MATCH_CODING){
        *transform_subseq_counter = 3;
    }else if (transform_subseq_parametersType->transform_ID_subseq == SubSeq_RLE_CODING){
        *transform_subseq_counter = 2;
    }else{
        /*DEFAULT*/
        *transform_subseq_counter = 1;
    }
    return SUCCESS;
}

EncodingParametersRC getOutputSymbolSize(Support_valuesType* support_values, uint8_t* outputSymbolSize){
    if(support_values == NULL || outputSymbolSize == NULL){
        return NULL_PTR;
    }
    *outputSymbolSize = support_values->output_symbol_size;
    return SUCCESS;
}

EncodingParametersRC getCodingSymbolSize(Support_valuesType* support_values, uint8_t* codingSymbolSize){
    if(support_values == NULL || codingSymbolSize == NULL){
        return NULL_PTR;
    }
    *codingSymbolSize = support_values->coding_symbol_size;
    return SUCCESS;
}
EncodingParametersRC getCodingOrder(Support_valuesType* support_values, uint8_t* codingOrder){
    if(support_values == NULL || codingOrder == NULL){
        return NULL_PTR;
    }
    *codingOrder = support_values->coding_order;
    return SUCCESS;
}

EncodingParametersRC getShareSubsymLUTFlag(
        Support_valuesType* support_values,
        uint8_t transform_ID_subsym,
        bool* share_subsym_lut_flag
){
    if(support_values == NULL || share_subsym_lut_flag == NULL){
        return NULL_PTR;
    }
    if(
        support_values->coding_symbol_size < support_values->output_symbol_size
        && support_values->coding_order
        && transform_ID_subsym == SubSym_LUT_TRANSFORM
    ){
        *share_subsym_lut_flag = support_values->share_subsym_lut_flag;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getShareSubsymPRVFlag(Support_valuesType* support_values, bool* share_subsym_prv_flag){
    if(support_values == NULL || share_subsym_prv_flag == NULL){
        return NULL_PTR;
    }
    if(
        support_values->coding_symbol_size < support_values->output_symbol_size
        && support_values->coding_order
    ){
        *share_subsym_prv_flag = support_values->share_subsym_prv_flag;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getBIFullCtxMode_BinarizationParameters(
        Cabac_binarization_parametersType* binarizationParameters,
        BinarizationIdEnum binarization_ID,
        bool* biFullCtxMode
){
    if(binarizationParameters == NULL || biFullCtxMode == NULL){
        return NULL_PTR;
    }
    if(
        binarization_ID == BinarizationID_BinaryCoding
    ){
        *biFullCtxMode = binarizationParameters->bIFullCtxMode;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getCTruncExpGolParam_BinarizationParameters(
        Cabac_binarization_parametersType* binarizationParameters,
        BinarizationIdEnum binarization_ID,
        uint8_t* cTruncExpGolParam
){
    if(binarizationParameters == NULL || cTruncExpGolParam == NULL){
        return NULL_PTR;
    }
    if(
        binarization_ID == BinarizationID_TruncatedExponentialGolomb
        || binarization_ID == BinarizationID_SignedTruncatedExponentialGolomb
    ){
        *cTruncExpGolParam = binarizationParameters->cTruncExpGolParam;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}
EncodingParametersRC getcMaxDTU_BinarizationParameters(
        Cabac_binarization_parametersType* binarizationParameters,
        BinarizationIdEnum binarization_ID,
        uint8_t* cMaxDTU
){
    if(binarizationParameters == NULL || cMaxDTU == NULL){
        return NULL_PTR;
    }
    if(
        binarization_ID == BinarizationID_DoubleTruncatedUnary
        || binarization_ID == BinarizationID_SignedDoubleTruncatedUnary
    ){
        *cMaxDTU = binarizationParameters->cMaxDTU;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}
EncodingParametersRC getSplitUnitSize_BinarizationParameters(
        Cabac_binarization_parametersType* binarizationParameters,
        BinarizationIdEnum binarization_ID,
        uint8_t* splitUnitSize
){
    if(binarizationParameters == NULL || splitUnitSize == NULL){
        return NULL_PTR;
    }
    if(
        binarization_ID == BinarizationID_SplitUnitWiseTruncatedUnary
        || binarization_ID == BinarizationID_SignedSplitUnitWiseTruncatedUnary
        || binarization_ID == BinarizationID_DoubleTruncatedUnary
        || binarization_ID == BinarizationID_SignedDoubleTruncatedUnary
    ){
        *splitUnitSize = binarizationParameters->splitUnitSize;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}


EncodingParametersRC getBinarizationId(Cabac_binarizationsType* cabac_binarizations, uint8_t* binarizationId){
    if(cabac_binarizations == NULL || binarizationId == NULL){
        return NULL_PTR;
    }
    *binarizationId = cabac_binarizations->binarization_ID;
    return SUCCESS;
}
EncodingParametersRC getBypassFlag(Cabac_binarizationsType* cabac_binarizations, bool* bypassFlag){
    if(cabac_binarizations == NULL || bypassFlag == NULL){
        return NULL_PTR;
    }
    *bypassFlag = cabac_binarizations->bypass_flag;
    return SUCCESS;
}
EncodingParametersRC getBIFullCtxMode(
        Cabac_binarizationsType* cabac_binarizations,
        bool* biFullCtxMode
){
    if(cabac_binarizations == NULL || cabac_binarizations->cabac_binarization_parameters == NULL || biFullCtxMode == NULL){
        return NULL_PTR;
    }
    return getBIFullCtxMode_BinarizationParameters(
            cabac_binarizations->cabac_binarization_parameters,
            cabac_binarizations->binarization_ID,
            biFullCtxMode
    );
}
EncodingParametersRC getCTruncExpGolParam(
        Cabac_binarizationsType* cabac_binarizations,
        uint8_t* cTruncExpGolParam
){
    if(cabac_binarizations == NULL || cabac_binarizations->cabac_binarization_parameters == NULL || cTruncExpGolParam == NULL){
        return NULL_PTR;
    }
    return getCTruncExpGolParam_BinarizationParameters(
            cabac_binarizations->cabac_binarization_parameters,
            cabac_binarizations->binarization_ID,
            cTruncExpGolParam
    );
}
EncodingParametersRC getcMaxDTU(
        Cabac_binarizationsType* cabac_binarizations,
        uint8_t* cMaxDTU
){
    if(cabac_binarizations == NULL || cabac_binarizations->cabac_binarization_parameters == NULL || cMaxDTU == NULL){
        return NULL_PTR;
    }
    return getcMaxDTU_BinarizationParameters(
        cabac_binarizations->cabac_binarization_parameters,
        cabac_binarizations->binarization_ID,
        cMaxDTU
    );
}
EncodingParametersRC getSplitUnitSize(
        Cabac_binarizationsType* cabac_binarizations,
        uint8_t* splitUnitSize
){
    if(cabac_binarizations == NULL || cabac_binarizations->cabac_binarization_parameters == NULL || splitUnitSize == NULL){
        return NULL_PTR;
    }
    return getSplitUnitSize_BinarizationParameters(
            cabac_binarizations->cabac_binarization_parameters,
            cabac_binarizations->binarization_ID,
            splitUnitSize
    );
}
EncodingParametersRC getCabacContextParameters(
        Cabac_binarizationsType* cabac_binarizations,
        Cabac_context_parametersType** cabac_context_parameters
){
    if(cabac_binarizations == NULL || cabac_context_parameters == NULL){
        return NULL_PTR;
    }
    if(cabac_binarizations->bypass_flag){
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    *cabac_context_parameters = cabac_binarizations->cabac_context_parameters;
    return SUCCESS;
}

EncodingParametersRC getDatasetTypeParameters(Encoding_ParametersType* encodingParameters, uint8_t* datasetType){
    if(encodingParameters == NULL || datasetType == NULL){
        return NULL_PTR;
    }
    *datasetType = encodingParameters->dataset_type;
    return SUCCESS;
}
EncodingParametersRC getAlphabetID(Encoding_ParametersType* encodingParameters, uint8_t* alphabetId){
    if(encodingParameters == NULL || alphabetId == NULL){
        return NULL_PTR;
    }
    *alphabetId = encodingParameters->alphabet_ID;
    return SUCCESS;
}
EncodingParametersRC getReadsLength(Encoding_ParametersType* encodingParameters, uint32_t* readsLength){
    if(encodingParameters == NULL || readsLength == NULL){
        return NULL_PTR;
    }
    *readsLength = encodingParameters->reads_length;
    return SUCCESS;
}
EncodingParametersRC getNumberOfTemplateSegmentsMinus1(
        Encoding_ParametersType* encodingParameters,
        uint8_t* numberOfTemplateSegmentsMinus1
){
    if(encodingParameters == NULL || numberOfTemplateSegmentsMinus1 == NULL){
        return NULL_PTR;
    }
    *numberOfTemplateSegmentsMinus1 = encodingParameters->number_of_template_segments_minus1;
    return SUCCESS;
}
EncodingParametersRC getMaxAUDataUnitSize(Encoding_ParametersType* encodingParameters, uint32_t* maxAUDataUnitSize){
    if(encodingParameters == NULL || maxAUDataUnitSize == NULL){
        return NULL_PTR;
    }
    *maxAUDataUnitSize = encodingParameters->max_au_data_unit_size;
    return SUCCESS;
}
EncodingParametersRC getPos40Bits(Encoding_ParametersType* encodingParameters, bool* pos40Bits){
    if(encodingParameters == NULL || pos40Bits == NULL){
        return NULL_PTR;
    }
    *pos40Bits = encodingParameters->pos_40_bits;
    return SUCCESS;
}
EncodingParametersRC getQVDepth(Encoding_ParametersType* encodingParameters, uint8_t* qv_depth){
    if(encodingParameters == NULL || qv_depth == NULL){
        return NULL_PTR;
    }
    *qv_depth = encodingParameters->qv_depth;
    return SUCCESS;
}
EncodingParametersRC getASDepth(Encoding_ParametersType* encodingParameters, uint8_t* as_depth){
    if(encodingParameters == NULL || as_depth == NULL){
        return NULL_PTR;
    }
    *as_depth = encodingParameters->as_depth;
    return SUCCESS;
}
EncodingParametersRC getNumClasses(Encoding_ParametersType* encodingParameters, uint8_t* numClasses){
    if(encodingParameters == NULL || numClasses == NULL){
        return NULL_PTR;
    }
    *numClasses = encodingParameters->num_classes;
    return SUCCESS;
}
EncodingParametersRC getClassIds(Encoding_ParametersType* encodingParameters, uint8_t* numClasses, uint8_t** classID){
    if(encodingParameters == NULL || numClasses == NULL || classID == NULL){
        return NULL_PTR;
    }
    *numClasses = encodingParameters->num_classes;
    *classID = encodingParameters->classID;
    return SUCCESS;
}
EncodingParametersRC getCABACDecoderConfiguration(
        Encoding_ParametersType* encodingParameters,
        uint8_t classIndex,
        uint8_t descriptorID,
        DecoderConfigurationTypeCABAC** decoder_configuration_cabac
){
    if(encodingParameters == NULL){
        return NULL_PTR;
    }
    if (classIndex >= encodingParameters->num_classes){
        return OUT_OF_BOUNDERIES;
    }
    if(descriptorID>= 18 || descriptorID==11 || descriptorID == 15){
        return OUT_OF_BOUNDERIES;
    }
    uint8_t indexToRetrieve;
    switch (descriptorID){
        case 0:
            indexToRetrieve = 0;
            break;
        case 1:
            indexToRetrieve = 1;
            break;
        case 2:
            indexToRetrieve = 2;
            break;
        case 3:
            indexToRetrieve = 3;
            break;
        case 4:
            indexToRetrieve = 4;
            break;
        case 5:
            indexToRetrieve = 5;
            break;
        case 6:
            indexToRetrieve = 6;
            break;
        case 7:
            indexToRetrieve = 7;
            break;
        case 8:
            indexToRetrieve = 8;
            break;
        case 9:
            indexToRetrieve = 9;
            break;
        case 10:
            indexToRetrieve = 10;
            break;
        case 12:
            indexToRetrieve = 11;
            break;
        case 13:
            indexToRetrieve = 12;
            break;
        case 14:
            indexToRetrieve = 13;
            break;
        case 16:
            indexToRetrieve = 15;
            break;
        case 17:
            indexToRetrieve = 16;
            break;
        default:
            return OUT_OF_BOUNDERIES;
    }
    *decoder_configuration_cabac =
            (DecoderConfigurationTypeCABAC *) (encodingParameters->decoderConfiguration + indexToRetrieve);
    return SUCCESS;
}
EncodingParametersRC getCABACDecoderConfigurationTokenType(
    Encoding_ParametersType* encodingParameters,
    uint8_t classIndex,
    uint8_t descriptorID,
    Decoder_configuration_tokentype** decoderConfigurationTokentypeCABAC
){
    if(encodingParameters == NULL || decoderConfigurationTokentypeCABAC == NULL){
        return NULL_PTR;
    }
    if(classIndex >= encodingParameters->num_classes){
        return OUT_OF_BOUNDERIES;
    }
    if(descriptorID == 11 || descriptorID == 15){
        // *decoderConfigurationTokentypeCABAC = encodingParameters->decoderConfiguration[classIndex][descriptorID];
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getNumGroups(Encoding_ParametersType* encodingParameters, uint16_t* numGroups){
    if(encodingParameters == NULL || numGroups == NULL){
        return NULL_PTR;
    }
    *numGroups = encodingParameters->num_groups;
    return SUCCESS;
}
EncodingParametersRC getReadGroupId(Encoding_ParametersType* encodingParameters, uint16_t groupIndex, char** rgroupID){
    if(encodingParameters == NULL || rgroupID == NULL){
        return NULL_PTR;
    }
    if(groupIndex < encodingParameters->num_groups){
        *rgroupID = encodingParameters->rgroup_ID[groupIndex];
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getSplicedReadsFlag(Encoding_ParametersType* encodingParameters, bool* splicedReadsFlag){
    if(encodingParameters == NULL || splicedReadsFlag == NULL){
        return NULL_PTR;
    }
    *splicedReadsFlag = encodingParameters->spliced_reads_flag;
    return SUCCESS;
}
EncodingParametersRC getMultipleSignatureFlag(
        Encoding_ParametersType* encodingParameters,
        bool* multipleSignatureFlag
){
    if(encodingParameters == NULL || multipleSignatureFlag==NULL){
        return NULL_PTR;
    }
    *multipleSignatureFlag = encodingParameters->multiple_signature_flag;
    return SUCCESS;
}
EncodingParametersRC getMultipleSignatureBaseParameters(
        Encoding_ParametersType* encodingParameters,
        uint32_t* multiple_signature_base
){
    if(encodingParameters == NULL || multiple_signature_base == NULL){
        return NULL_PTR;
    }
    *multiple_signature_base = encodingParameters->multiple_signature_base;
    return SUCCESS;
}
EncodingParametersRC getSignatureSize(
    Encoding_ParametersType* encodingParameters,
    uint8_t* u_signature_size
){
    if(encodingParameters == NULL || u_signature_size == NULL){
        return NULL_PTR;
    }
    *u_signature_size = encodingParameters->U_signature_size;
    return SUCCESS;
}
EncodingParametersRC getMultipleAlignments_flag(
        Encoding_ParametersType* encodingParameters,
        bool* multiple_alignments_flag
){
    if(encodingParameters == NULL || multiple_alignments_flag == NULL){
        return NULL_PTR;
    }
    *multiple_alignments_flag = encodingParameters->multiple_alignments_flag;
    return SUCCESS;
}
EncodingParametersRC getMscoreMantissa(
        Encoding_ParametersType* encodingParameters,
        uint8_t* mscore_mantissa
){
    if(encodingParameters == NULL || mscore_mantissa == NULL){
        return NULL_PTR;
    }
    if(encodingParameters->multiple_alignments_flag){
        *mscore_mantissa = encodingParameters->mscore_mantissa;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}
EncodingParametersRC getQVCodingMode(
        Encoding_ParametersType* encodingParameters,
        uint8_t classIndex,
        uint8_t* qv_coding_mode
){
    if(encodingParameters==NULL || qv_coding_mode==NULL){
        return NULL_PTR;
    }
    if(classIndex < encodingParameters->num_classes){
        *qv_coding_mode = encodingParameters->qv_coding_mode[classIndex];
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getQVPSflag(
    Encoding_ParametersType* encodingParameters,
    uint8_t classIndex,
    bool* qvps_flag
){
    if(encodingParameters == NULL || qvps_flag == NULL){
        return NULL_PTR;
    }
    if(classIndex < encodingParameters->num_classes){
        if(encodingParameters->qv_coding_mode[classIndex] == 1) {
            *qvps_flag = encodingParameters->qvps_flag[classIndex];
            return SUCCESS;
        }
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getParameterSetQvps(
        Encoding_ParametersType* encodingParameters,
        uint8_t classIndex,
        Parameter_set_qvpsType** pParameter_set_qvpsType
){
    if(encodingParameters == NULL || pParameter_set_qvpsType == NULL){
        return NULL_PTR;
    }
    if(classIndex < encodingParameters->num_classes){
        if(encodingParameters->qv_coding_mode[classIndex] == 1){
            if(encodingParameters->qvps_flag[classIndex]){
                *pParameter_set_qvpsType = encodingParameters->parameter_set_qvps[classIndex];
                return SUCCESS;
            }
            return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
        }
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getDefaultQvpsId(
        Encoding_ParametersType* encodingParameters,
        uint8_t classIndex,
        uint8_t* defaultQvpsId
){
    if(encodingParameters == NULL || defaultQvpsId == NULL){
        return NULL_PTR;
    }
    if(classIndex < encodingParameters->num_classes){
        if(encodingParameters->qv_coding_mode[classIndex] == 1){
            if(encodingParameters->qvps_flag[classIndex]){
                *defaultQvpsId = encodingParameters->default_qvps_ID[classIndex];
                return SUCCESS;
            }
            return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
        }
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getCrpsFlag(
        Encoding_ParametersType* encodingParameters,
        bool* crps_flag
){
    if(encodingParameters == NULL || crps_flag == NULL){
        return NULL_PTR;
    }
    *crps_flag = encodingParameters->crps_flag;
    return SUCCESS;
}
EncodingParametersRC getParameterSetCrps(
        Encoding_ParametersType* encodingParameters,
        Parameter_set_crpsType** parameterSetCrps
){
    if(encodingParameters == NULL || parameterSetCrps == NULL){
        return NULL_PTR;
    }
    if(encodingParameters->crps_flag){
        *parameterSetCrps = encodingParameters->parameter_set_crps;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getNumDescriptorSubsequenceCfgsMinus1(
        DecoderConfigurationTypeCABAC* decoderConfigurationCABAC,
        uint8_t* num_descriptor_subsequence_cfgs_minus1
){
    if(decoderConfigurationCABAC == NULL || num_descriptor_subsequence_cfgs_minus1 == NULL){
        return NULL_PTR;
    }
    *num_descriptor_subsequence_cfgs_minus1 = decoderConfigurationCABAC->num_descriptor_subsequence_cfgs_minus1;
    return SUCCESS;
}

EncodingParametersRC getDescriptorSubsequenceId(
    DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
    uint16_t desciptorSubsequence_i,
    uint16_t* desciptorSubsequenceId
){
    if(decoderConfigurationTypeCABAC == NULL || desciptorSubsequenceId == NULL){
        return NULL_PTR;
    }
    if(desciptorSubsequence_i <= decoderConfigurationTypeCABAC->num_descriptor_subsequence_cfgs_minus1){
        *desciptorSubsequenceId = decoderConfigurationTypeCABAC->descriptor_subsequence_ID[desciptorSubsequence_i];
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getTransformSubseqParameters(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequence_i,
        Transform_subseq_parametersType** transform_subseq_parameter
){
    if(decoderConfigurationTypeCABAC == NULL || transform_subseq_parameter == NULL){
        return NULL_PTR;
    }
    if(descriptorSubsequence_i <= decoderConfigurationTypeCABAC->num_descriptor_subsequence_cfgs_minus1){
        *transform_subseq_parameter = decoderConfigurationTypeCABAC->transform_subseq_parameters[descriptorSubsequence_i];
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}

EncodingParametersRC getTransformSubseqCounter(
        DecoderConfigurationTypeCABAC *decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint8_t *transformSubseqCounter
){
    if(decoderConfigurationTypeCABAC == NULL || transformSubseqCounter == NULL){
        return NULL_PTR;
    }
    if(descriptorSubsequenceId <= decoderConfigurationTypeCABAC->num_descriptor_subsequence_cfgs_minus1){
        getCorrectedTransform_subseq_counter(
            decoderConfigurationTypeCABAC->transform_subseq_parameters[descriptorSubsequenceId],
            transformSubseqCounter
        );
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}

EncodingParametersRC getTransformIdSubSym(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        TransformSubSymIdEnum* transformIdSubSym
){
    if(decoderConfigurationTypeCABAC == NULL || transformIdSubSym == NULL){
        return NULL_PTR;
    }
    uint8_t transformSubseqCounter;
    EncodingParametersRC result =
            getTransformSubseqCounter(decoderConfigurationTypeCABAC, descriptorSubsequenceId, &transformSubseqCounter);
    if(result == SUCCESS){
        if(transformSubseqIndex < transformSubseqCounter){
            *transformIdSubSym =
                    decoderConfigurationTypeCABAC->transform_id_subsym[descriptorSubsequenceId][transformSubseqIndex];
            return SUCCESS;
        }
        return OUT_OF_BOUNDERIES;
    }
    return result;
}
EncodingParametersRC getTransformSupportValues(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        Support_valuesType** supportValuesType
){
    if(decoderConfigurationTypeCABAC == NULL || supportValuesType == NULL){
        return NULL_PTR;
    }
    uint8_t transformSubseqCounter;
    EncodingParametersRC result =
            getTransformSubseqCounter(decoderConfigurationTypeCABAC, descriptorSubsequenceId, &transformSubseqCounter);
    if(result == SUCCESS){
        if(transformSubseqIndex < transformSubseqCounter){
            *supportValuesType =
                    decoderConfigurationTypeCABAC->support_values[descriptorSubsequenceId][transformSubseqIndex];
            return SUCCESS;
        }
        return OUT_OF_BOUNDERIES;
    }
    return result;
}
EncodingParametersRC getCABACBinarizations(
        DecoderConfigurationTypeCABAC *decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        Cabac_binarizationsType **cabacBinarizations
){
    if(decoderConfigurationTypeCABAC == NULL || cabacBinarizations == NULL){
        return NULL_PTR;
    }
    uint8_t transformSubseqCounter;
    EncodingParametersRC result =
            getTransformSubseqCounter(decoderConfigurationTypeCABAC, descriptorSubsequenceId, &transformSubseqCounter);
    if(result == SUCCESS){
        if(transformSubseqIndex < transformSubseqCounter){
            *cabacBinarizations =
                    decoderConfigurationTypeCABAC->cabac_binarizations[descriptorSubsequenceId][transformSubseqIndex];
            return SUCCESS;
        }
        return OUT_OF_BOUNDERIES;
    }
    return result;
}

EncodingParametersRC getSupportValueOutputSymbolSize(
    DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
    uint16_t descriptorSubsequenceId,
    uint16_t transformSubseqIndex,
    uint8_t* outputSymbolSize
){
    Support_valuesType* supportValues;
    EncodingParametersRC resultSupportValue = getTransformSupportValues(
        decoderConfigurationTypeCABAC,
        descriptorSubsequenceId,
        transformSubseqIndex,
        &supportValues
    );
    if(resultSupportValue == SUCCESS){
        return getOutputSymbolSize(supportValues, outputSymbolSize);
    }
    return resultSupportValue;
}

EncodingParametersRC getSupportValueCodingSymbolSize(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        uint8_t* codingSymbolSize
){
    Support_valuesType* supportValues;
    EncodingParametersRC resultSupportValue = getTransformSupportValues(
            decoderConfigurationTypeCABAC,
            descriptorSubsequenceId,
            transformSubseqIndex,
            &supportValues
    );
    if(resultSupportValue == SUCCESS){
        return getCodingSymbolSize(supportValues, codingSymbolSize);
    }
    return resultSupportValue;
}

EncodingParametersRC getSupportValueCodingOrder(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        uint8_t* codingOrder
){
    Support_valuesType* supportValues;
    EncodingParametersRC resultSupportValue = getTransformSupportValues(
            decoderConfigurationTypeCABAC,
            descriptorSubsequenceId,
            transformSubseqIndex,
            &supportValues
    );
    if(resultSupportValue == SUCCESS){
        return getCodingOrder(supportValues, codingOrder);
    }
    return resultSupportValue;
}

EncodingParametersRC getSupportValueShareSubsymLUTFlag(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        bool* share_subsym_lut_flag
){
    Support_valuesType* supportValues;
    EncodingParametersRC resultSupportValue = getTransformSupportValues(
            decoderConfigurationTypeCABAC,
            descriptorSubsequenceId,
            transformSubseqIndex,
            &supportValues
    );
    if(resultSupportValue == SUCCESS){
        TransformSubSymIdEnum transformSubSymIdEnum;
        EncodingParametersRC resultTransformId = getTransformIdSubSym(
                decoderConfigurationTypeCABAC,
                descriptorSubsequenceId,
                transformSubseqIndex,
                &transformSubSymIdEnum
        );
        if(resultTransformId == SUCCESS) {
            return getShareSubsymLUTFlag(supportValues, transformSubSymIdEnum, share_subsym_lut_flag);
        }
        return resultTransformId;
    }
    return resultSupportValue;
}

EncodingParametersRC getSupportValueShareSubsymPRVFlag(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        bool* share_subsym_prv_flag
){
    Support_valuesType* supportValues;
    EncodingParametersRC resultSupportValue = getTransformSupportValues(
            decoderConfigurationTypeCABAC,
            descriptorSubsequenceId,
            transformSubseqIndex,
            &supportValues
    );
    if(resultSupportValue == SUCCESS){
        return getShareSubsymPRVFlag(supportValues, share_subsym_prv_flag);
    }
    return resultSupportValue;
}

EncodingParametersRC getOutputSymbolSizeToken(
        Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac,
        uint8_t* output_symbol_size
){
    if(decoder_configuration_tokentype_cabac == NULL || output_symbol_size == NULL){
        return NULL_PTR;
    }
    *output_symbol_size = decoder_configuration_tokentype_cabac->output_symbol_size;
    return SUCCESS;
}
EncodingParametersRC getCodingSymbolsNumbers(
        Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac,
        uint8_t* coding_symbols_numbers
){
    if(decoder_configuration_tokentype_cabac == NULL || coding_symbols_numbers == NULL){
        return NULL_PTR;
    }
    *coding_symbols_numbers = decoder_configuration_tokentype_cabac->coding_symbols_numbers;
    return SUCCESS;
}
EncodingParametersRC getCodingSymbolSizes(
        Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac,
        uint8_t* coding_symbols_numbers,
        uint8_t** coding_symbol_size
){
    if(decoder_configuration_tokentype_cabac == NULL || coding_symbols_numbers == NULL || coding_symbol_size == NULL){
        return NULL_PTR;
    }
    *coding_symbols_numbers = decoder_configuration_tokentype_cabac->coding_symbols_numbers;
    *coding_symbol_size = decoder_configuration_tokentype_cabac->coding_symbol_size;
    return SUCCESS;
}
EncodingParametersRC getCabacBinarizationsToken(
        Decoder_configuration_tokentype_cabac *decoder_configuration_tokentype_cabac,
        Cabac_binarizationsType **cabacBinarizations
){
    if(decoder_configuration_tokentype_cabac == NULL || cabacBinarizations == NULL ){
        return NULL_PTR;
    }
    *cabacBinarizations = decoder_configuration_tokentype_cabac->cabac_binarizations;
    return SUCCESS;
}

EncodingParametersRC getRLEGuardTokenType(
        Decoder_configuration_tokentype* decoder_configuration_tokentype_typeCABAC,
        uint8_t* rle_guard_tokentype
){
    if(decoder_configuration_tokentype_typeCABAC == NULL || rle_guard_tokentype == NULL) {
        return NULL_PTR;
    }
    *rle_guard_tokentype = decoder_configuration_tokentype_typeCABAC->rle_guard_tokentype;
    return SUCCESS;
}
EncodingParametersRC getDecoderConfigurationTokentypeCabac_order0(
        Decoder_configuration_tokentype* decoder_configuration_tokentype_typeCABAC,
        Decoder_configuration_tokentype_cabac** decoder_configuration_tokentype_cabac
){
    if(decoder_configuration_tokentype_typeCABAC == NULL || decoder_configuration_tokentype_cabac == NULL) {
        return NULL_PTR;
    }
    *decoder_configuration_tokentype_cabac =
            decoder_configuration_tokentype_typeCABAC->decoder_configuration_tokentype_cabac0;
    return SUCCESS;
}
EncodingParametersRC getDecoderConfigurationTokentypeCabac_order1(
        Decoder_configuration_tokentype* decoder_configuration_tokentype_typeCABAC,
        Decoder_configuration_tokentype_cabac** decoder_configuration_tokentype_cabac
){
    if(decoder_configuration_tokentype_typeCABAC == NULL || decoder_configuration_tokentype_cabac == NULL) {
        return NULL_PTR;
    }
    *decoder_configuration_tokentype_cabac =
            decoder_configuration_tokentype_typeCABAC->decoder_configuration_tokentype_cabac1;
    return SUCCESS;
}
EncodingParametersRC getQVNumCodebooksTotal(
        Parameter_set_qvpsType* parameter_set_qvps,
        uint8_t* qv_num_codebooks_total
){
    if(parameter_set_qvps == NULL || qv_num_codebooks_total == NULL){
        return NULL_PTR;
    }
    *qv_num_codebooks_total = parameter_set_qvps->qv_num_codebooks_total;
    return SUCCESS;
}
EncodingParametersRC getQVNumCodebooksEntries(
        Parameter_set_qvpsType* parameter_set_qvps,
        uint8_t qv_num_codebook_index,
        uint8_t* qv_num_codebook_entries,
        uint8_t** qv_recon
){
    if(parameter_set_qvps == NULL || qv_num_codebook_entries == NULL || qv_recon == NULL){
        return NULL_PTR;
    }
    if(qv_num_codebook_index >= parameter_set_qvps->qv_num_codebooks_total){
        return OUT_OF_BOUNDERIES;
    }
    *qv_num_codebook_entries = parameter_set_qvps->qv_num_codebook_entries[qv_num_codebook_index];
    *qv_recon = parameter_set_qvps->qv_recon[qv_num_codebook_index];
}



EncodingParametersRC getCrAlgId(
        Parameter_set_crpsType* parameterSetCrps,
        CrAlgEnum* cr_alg_ID
){
    if(parameterSetCrps == NULL || cr_alg_ID == NULL){
        return NULL_PTR;
    }
    *cr_alg_ID = parameterSetCrps->cr_alg_ID;
}
EncodingParametersRC getCrPadSize(
        Parameter_set_crpsType* parameterSetCrps,
        uint8_t* cr_pad_size
){
    if(parameterSetCrps == NULL || cr_pad_size == NULL){
        return NULL_PTR;
    }
    *cr_pad_size = parameterSetCrps->cr_pad_size;
}
EncodingParametersRC getCrBufMaxSize(
        Parameter_set_crpsType* parameterSetCrps,
        uint32_t* cr_buf_max_size
){
    if(parameterSetCrps == NULL || cr_buf_max_size == NULL){
        return NULL_PTR;
    }
    *cr_buf_max_size = parameterSetCrps->cr_buf_max_size;
}


/**********************Write methods**********************************/
bool writeTransformSubseqParameters(
    Transform_subseq_parametersType* transform_subseq_parameters,
    OutputBitstream* outputBitstream
){
    bool transIdSubseqSuccWrite = true;
    bool matchCodingBufferSizeSuccWrite = true;
    bool rleCodingGuardSuccWrite = true;

    transIdSubseqSuccWrite =
            writeNBitsShift(outputBitstream, 8, (char*)&(transform_subseq_parameters->transform_ID_subseq));
    if(transform_subseq_parameters->transform_ID_subseq == SubSeq_EQUALITY_CODING){
        /*Nothing to write*/
    } else if (transform_subseq_parameters->transform_ID_subseq == SubSeq_MATCH_CODING){
        matchCodingBufferSizeSuccWrite = writeNBitsShiftAndConvertToBigEndian16(
                outputBitstream,
                16,
                transform_subseq_parameters->match_coding_buffer_size
        );
    } else if (transform_subseq_parameters->rle_coding_guard == SubSeq_RLE_CODING){
        rleCodingGuardSuccWrite = writeNBitsShift(
                outputBitstream,
                8,
                (char*)&(transform_subseq_parameters->rle_coding_guard)
        );
    }
    return transIdSubseqSuccWrite && matchCodingBufferSizeSuccWrite && rleCodingGuardSuccWrite;
};

bool writeSupportValues(
    Support_valuesType* supportValues,
    OutputBitstream* outputBitstream,
    TransformSubSymIdEnum transform_ID_subsym

){
    if(!(
        writeNBitsShift(outputBitstream, 6, (char*)&(supportValues->output_symbol_size))
        && writeNBitsShift(outputBitstream, 6, (char*)&(supportValues->coding_symbol_size))
        && writeNBitsShift(outputBitstream, 2, (char*)&(supportValues->coding_order))
    )){
      return false;
    }
    if(supportValues->coding_symbol_size < supportValues->output_symbol_size && supportValues->coding_order > 0){
        if(transform_ID_subsym == SubSym_LUT_TRANSFORM){
            if (!writeBit(outputBitstream, (uint8_t)(supportValues->share_subsym_lut_flag ? 1:0))){
                return false;
            }
        }
    }
    return writeBit(outputBitstream, (uint8_t)(supportValues->share_subsym_prv_flag ? 1:0));
}

bool writeCABACContextParameters(
    Cabac_context_parametersType* cabac_context_parameters,
    OutputBitstream* outputBitstream,
    uint8_t codingSymbolSize,
    uint8_t outputSymbolSize
){
    if( !(
        writeBit(
                outputBitstream,
                (uint8_t)(cabac_context_parameters->adaptive_mode_flag ? 1:0)
        ) && writeNBitsShiftAndConvertToBigEndian16(
                outputBitstream,
                16,
                cabac_context_parameters->num_contexts
        )
    )){
        return false;
    }
    for(int i=0; i<cabac_context_parameters->num_contexts; i++){
        if (!writeNBitsShift(outputBitstream, 7, (char*)&(cabac_context_parameters->context_initialization_value[i]))){
            return false;
        }
    }
    if(codingSymbolSize < outputSymbolSize){
        if(!writeBit(outputBitstream, (uint8_t)(cabac_context_parameters->share_sub_sym_ctx_flag? 1:0))){
            return false;
        }
    }
    if(!writeBit(outputBitstream, (uint8_t)(cabac_context_parameters->segment_ctx_flag? 1:0))){
        return false;
    }
    if(cabac_context_parameters->segment_ctx_flag){
        return
            writeNBitsShiftAndConvertToBigEndian16(outputBitstream, 16, cabac_context_parameters->segment_ctx_len)
            && writeNBitsShiftAndConvertToBigEndian16(outputBitstream, 16, cabac_context_parameters->segment_ctx_offset);
    }

}

bool writeCABACBinarizationParameters(
    Cabac_binarization_parametersType* cabac_binarization_parameters,
    OutputBitstream* outputBitstream,
    BinarizationIdEnum binarization_ID
) {
    bool biFullCtxModeSuccWrite = true;
    bool cTruncExpGolParamSuccWrite = true;
    bool cMaxDTUSuccWrite = true;
    bool splitUnitSizeSuccWrite = true;

    if (binarization_ID == BinarizationID_BinaryCoding) {
        biFullCtxModeSuccWrite =
                writeBit(outputBitstream, (uint8_t) (cabac_binarization_parameters->bIFullCtxMode ? 1 : 0));
    } else if (
        binarization_ID == BinarizationID_TruncatedExponentialGolomb
        || binarization_ID == BinarizationID_SignedTruncatedExponentialGolomb
    ) {
        cTruncExpGolParamSuccWrite = writeNBitsShift(
                outputBitstream,
                6,
                (char *) &(cabac_binarization_parameters->cTruncExpGolParam)
        );
    } else if (
        binarization_ID == BinarizationID_DoubleTruncatedUnary
        || binarization_ID == BinarizationID_SignedDoubleTruncatedUnary
    ) {
        cTruncExpGolParamSuccWrite = writeNBitsShift(
                outputBitstream,
                8,
                (char *) &(cabac_binarization_parameters->cMaxDTU)
        );
    }
    if (
        binarization_ID == BinarizationID_SplitUnitWiseTruncatedUnary
        || binarization_ID == BinarizationID_SignedSplitUnitWiseTruncatedUnary
        || binarization_ID == BinarizationID_DoubleTruncatedUnary
        || binarization_ID == BinarizationID_SignedDoubleTruncatedUnary
    ) {
        splitUnitSizeSuccWrite = writeNBitsShift(
                outputBitstream,
                6,
                (char *) &(cabac_binarization_parameters->splitUnitSize)
        );
    }
    return biFullCtxModeSuccWrite && cTruncExpGolParamSuccWrite && cMaxDTUSuccWrite && splitUnitSizeSuccWrite;
}

bool writeCABACBinarizations(
    Cabac_binarizationsType* cabac_binarizations,
    OutputBitstream* outputBitstream,
    uint8_t codingSymbolSize,
    uint8_t outputSymbolSize
){
    uint8_t binarizationIdValue = (uint8_t)cabac_binarizations->binarization_ID;
    if(!(
        writeNBitsShift(outputBitstream, 5, (char*)&binarizationIdValue)
        && writeBit(outputBitstream, (uint8_t) (cabac_binarizations->bypass_flag ? 1 : 0))
        && writeCABACBinarizationParameters(
            cabac_binarizations->cabac_binarization_parameters,
            outputBitstream,
            cabac_binarizations->binarization_ID
        )
    )){
        return false;
    }
    if(!cabac_binarizations->bypass_flag){
        return writeCABACContextParameters(
            cabac_binarizations->cabac_context_parameters,
            outputBitstream,
            codingSymbolSize,
            outputSymbolSize
        );
    }
    return true;
}

bool writeDecoderConfigurationCABAC(
    DecoderConfigurationTypeCABAC* decoderConfigurationCABAC,
    OutputBitstream* outputBitstream
){
    if(!(
        writeNBitsShift(outputBitstream, 8, (char*)&(decoderConfigurationCABAC->num_descriptor_subsequence_cfgs_minus1)))
    ){
        return false;
    }
    for(
        uint16_t i = 0;
        i <= decoderConfigurationCABAC->num_descriptor_subsequence_cfgs_minus1;
        i++
    ){
        if(
            writeNBitsShiftAndConvertToBigEndian16(outputBitstream, 10, i)
            && writeTransformSubseqParameters(
                decoderConfigurationCABAC->transform_subseq_parameters[i],
                outputBitstream
            )
        ){
            return false;
        }
        uint8_t transform_subseq_counter;
        if (getCorrectedTransform_subseq_counter(
            decoderConfigurationCABAC->transform_subseq_parameters[i],
            &transform_subseq_counter
        ) != SUCCESS){
            return false;
        }

        for(int j=0; j<transform_subseq_counter; j++){
            uint8_t transformIdSubSym = decoderConfigurationCABAC->transform_id_subsym[i][j];
            if(!(
                writeNBitsShift(outputBitstream, 3, (char*)&transformIdSubSym)
                && writeSupportValues(
                    decoderConfigurationCABAC->support_values[i][j],
                    outputBitstream,
                    decoderConfigurationCABAC->transform_id_subsym[i][j]
                )
                && writeCABACBinarizations(
                    decoderConfigurationCABAC->cabac_binarizations[i][j],
                    outputBitstream,
                    decoderConfigurationCABAC->support_values[i][j]->coding_symbol_size,
                    decoderConfigurationCABAC->support_values[i][j]->output_symbol_size
                )
            )){
                return false;
            }
        }
    }
    return true;
}

bool writeDecoderConfigurationTokentypeCabac(
        Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
        OutputBitstream* outputBitstream
){
    if(!writeNBitsShift(outputBitstream, 6, (char*)&decoderConfigurationTokentypeCabac->output_symbol_size)){
        return false;
    }
    for(int i=0; i<decoderConfigurationTokentypeCabac->coding_symbols_numbers; i++){
        if(
            !writeNBitsShift(outputBitstream, 6, (char*)(&decoderConfigurationTokentypeCabac->coding_symbol_size[i]))
        ){
            return false;
        }
    }
    if (!writeSupportValues(
            decoderConfigurationTokentypeCabac->support_values,
            outputBitstream,
            decoderConfigurationTokentypeCabac->transformSubSym)
    ){
        return false;
    }
    if (!
        writeCABACBinarizations(
                decoderConfigurationTokentypeCabac->cabac_binarizations,
                outputBitstream,
                decoderConfigurationTokentypeCabac->support_values->coding_symbol_size,
                decoderConfigurationTokentypeCabac->support_values->output_symbol_size
        )
    ){
        return false;
    }
    return true;
}

bool writeDecoder_configuration_tokentype_TypeCABAC(
    Decoder_configuration_tokentype* decoder_configuration_tokentype_typeCABAC,
    OutputBitstream* outputBitstream
){
    return writeNBitsShift(outputBitstream, 8, (char*)&(decoder_configuration_tokentype_typeCABAC->rle_guard_tokentype))
        && writeDecoderConfigurationTokentypeCabac(
            decoder_configuration_tokentype_typeCABAC->decoder_configuration_tokentype_cabac0,
            outputBitstream
        )
        && writeDecoderConfigurationTokentypeCabac(
                decoder_configuration_tokentype_typeCABAC->decoder_configuration_tokentype_cabac1,
                outputBitstream
        )
    ;
}

bool writeParameterSetQvps(
    Parameter_set_qvpsType* parameter_set_qvps,
    OutputBitstream* outputBitstream
){
    if (!writeNBitsShift(outputBitstream,8,(char*)&(parameter_set_qvps->qv_num_codebooks_total))){
        return false;
    }
    for(int i=0; i<parameter_set_qvps->qv_num_codebooks_total; i++){
        if (!writeNBitsShift(outputBitstream,8,(char*)&(parameter_set_qvps->qv_num_codebook_entries[i]))){
            return false;
        }
        for(int j=0; j<parameter_set_qvps->qv_num_codebook_entries[i]; j++){
            if (!writeNBitsShift(outputBitstream,8,(char*)&(parameter_set_qvps->qv_recon[i][j]))){
                return false;
            }
        }
    }
    return true;
}

bool writeParameterSetCrps(
       Parameter_set_crpsType* parameter_set_crps,
       OutputBitstream* outputBitstream
){
    if(!
        writeNBitsShift(outputBitstream, 8, (char*)&(parameter_set_crps->cr_alg_ID))
    ){
        return false;
    }
    if(parameter_set_crps->cr_alg_ID == CrAlg_PushIn || parameter_set_crps->cr_alg_ID == CrAlg_LocalAssembly){
        return writeNBitsShift(outputBitstream, 8, (char*)&(parameter_set_crps->cr_pad_size))
            && writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 24, parameter_set_crps->cr_buf_max_size);
    }
    return true;
}

bool writeEncoding_parameters(
        Encoding_ParametersType* encodingParameters,
        OutputBitstream* outputBitstream
){
    uint8_t reserved = 0;
    if(!(
        writeNBitsShift(outputBitstream, 4, (char*)&(encodingParameters->dataset_type))
        && writeNBitsShift(outputBitstream, 8, (char*)&(encodingParameters->alphabet_ID))
        && writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 24, encodingParameters->reads_length)
        && writeNBitsShift(outputBitstream, 2, (char*)&(encodingParameters->number_of_template_segments_minus1))
        && writeNBitsShift(outputBitstream, 6, (char*)&reserved)
        && writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 29, encodingParameters->max_au_data_unit_size)
        && writeBit(outputBitstream, (uint8_t)(encodingParameters->pos_40_bits ? 1:0))
        && writeNBitsShift(outputBitstream, 3, (char*)&(encodingParameters->qv_depth))
        && writeNBitsShift(outputBitstream, 3, (char*)&(encodingParameters->as_depth))
        && writeNBitsShift(outputBitstream, 4, (char*)&(encodingParameters->num_classes))
    )){
        return false;
    }
    for(int j=0; j<encodingParameters->num_classes; j++){
        if(!writeNBitsShift(outputBitstream, 4, (char*)&(encodingParameters->classID[j]))){
            return false;
        }
    }
    for(int desc_ID=0; desc_ID < 18; desc_ID++){

        if(!writeBit(outputBitstream, (uint8_t)(encodingParameters->class_specific_dec_cfg_flag[desc_ID]))) {
            return false;
        }
        int endLoop = encodingParameters->class_specific_dec_cfg_flag[desc_ID] ? encodingParameters->num_classes : 1;
        for(int c = 0; c < endLoop; c++){
            if(!
                writeBit(outputBitstream, (uint8_t)(encodingParameters->dec_cfg_flag[c][desc_ID] ? 1:0))
            ){
                return false;
            }
            if(encodingParameters->dec_cfg_flag[c][desc_ID]){
                if(encodingParameters->encoding_mode_id[c][desc_ID] != 0){
                    //Only supports CABAC
                    return false;
                }
                if (!writeNBitsShift(outputBitstream, 8, (char*) &(encodingParameters->encoding_mode_id[c][desc_ID]))){
                    return false;
                }
                if(desc_ID != 11 && desc_ID != 15){
                    if(!writeDecoderConfigurationCABAC(
                            (DecoderConfigurationTypeCABAC*)encodingParameters->decoderConfiguration[c]+desc_ID,
                            outputBitstream
                    )){
                        return false;
                    }
                }else if(desc_ID==11 || desc_ID==15){
                    if(!writeDecoder_configuration_tokentype_TypeCABAC(
                            (Decoder_configuration_tokentype*)encodingParameters->decoderConfiguration[c]+desc_ID,
                            outputBitstream
                    )){
                        return false;
                    }
                }
            }
        }
    }
    if(!writeNBitsShiftAndConvertToBigEndian16(outputBitstream, 16, encodingParameters->num_groups)){
        return false;
    }
    for(int j=0; j<encodingParameters->num_groups; j++){
        if(!writeCharBufferToBitstream(outputBitstream, encodingParameters->rgroup_ID[j])){
            return false;
        }
    }
    if(!(
        writeBit(outputBitstream, (uint8_t)(encodingParameters->spliced_reads_flag?1:0))
        && writeBit(outputBitstream, (uint8_t)(encodingParameters->multiple_signature_flag?1:0))
        && writeNBitsShiftAndConvertToBigEndian32(outputBitstream, 31, encodingParameters->multiple_signature_base)
        && writeNBitsShift(outputBitstream,8,(char*)&(encodingParameters->U_signature_size))
        && writeBit(outputBitstream, (uint8_t)(encodingParameters->multiple_alignments_flag))
    )){
        return false;
    }
    if(encodingParameters->multiple_alignments_flag){
        if(!writeNBitsShift(outputBitstream, 5, (char*)&(encodingParameters->mscore_mantissa))){
            return false;
        }
    }
    for(int c=0; c<encodingParameters->num_classes; c++){
        if(!writeNBitsShift(outputBitstream, 4, (char*)&(encodingParameters->qv_coding_mode[c]))){
            return false;
        }
        if(encodingParameters->qv_coding_mode[c] == 1){
            if(!writeBit(outputBitstream, (uint8_t)(encodingParameters->qvps_flag[c] ? 1:0))){
                return false;
            }
            if(encodingParameters->qvps_flag[c]){
                if(!writeParameterSetQvps(encodingParameters->parameter_set_qvps[c], outputBitstream)){
                    return false;
                }
            }else{
                if(!writeNBitsShift(outputBitstream, 4, (char*)&(encodingParameters->default_qvps_ID[c]))){
                    return false;
                }
            }
        }
    }
    if(!writeBit(outputBitstream, (uint8_t)(encodingParameters->crps_flag ? 1:0))){
        if(!writeParameterSetCrps(encodingParameters->parameter_set_crps, outputBitstream)){
            return false;
        }
    }
    return writeBuffer(outputBitstream);
}

Transform_subseq_parametersType* constructTransformSubseqParameters(
    TransformSubSeqIdEnum transform_ID_subseq,
    uint16_t match_coding_buffer_size,
    uint8_t rle_coding_guard
){
    Transform_subseq_parametersType* result =
            (Transform_subseq_parametersType*)malloc(sizeof(Transform_subseq_parametersType));
    if(result == NULL){
        return result;
    }
    result->transform_ID_subseq = transform_ID_subseq;
    result->match_coding_buffer_size = match_coding_buffer_size;
    result->rle_coding_guard = rle_coding_guard;
    return result;
}

Transform_subseq_parametersType* constructNoTransformSubseqParameters(){
    return constructTransformSubseqParameters(SubSeq_NO_TRANSFORM,0,0);
}
Transform_subseq_parametersType* constructEqualityCodingTransformSubseqParameters(){
    return constructTransformSubseqParameters(SubSeq_EQUALITY_CODING,0,0);
}
Transform_subseq_parametersType* constructMatchCodingSubseqTransformSubseqParameters(
    uint16_t match_coding_buffer_size
){
    return constructTransformSubseqParameters(SubSeq_MATCH_CODING, match_coding_buffer_size, 0);
}
Transform_subseq_parametersType* constructRLECodingSubseqTransformSubseqParameters(
        uint8_t rle_coding_guard
){
    return constructTransformSubseqParameters(SubSeq_RLE_CODING, 0, rle_coding_guard);
}
Transform_subseq_parametersType* constructRLEQVCodingSubseqTransformSubseqParameters(){
    return constructTransformSubseqParameters(SubSeq_RLE_QV_CODING, 0, 0);
}
EncodingParametersRC getTransformSubSeqId(
        Transform_subseq_parametersType* transformSubseqParameters,
        TransformSubSeqIdEnum* output
){
    *output = transformSubseqParameters->transform_ID_subseq;
    return SUCCESS;
}
EncodingParametersRC getMatchCodingBufferSize(
        Transform_subseq_parametersType* transformSubseqParameters,
        uint16_t* output
){
    if(transformSubseqParameters->transform_ID_subseq != SubSeq_MATCH_CODING){
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    *output = transformSubseqParameters->match_coding_buffer_size;
    return SUCCESS;
}
EncodingParametersRC getRLECodingGuard(
        Transform_subseq_parametersType* transformSubseqParameters,
        uint8_t* output
){
    if(transformSubseqParameters->transform_ID_subseq != SubSeq_RLE_CODING){
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    *output = transformSubseqParameters->rle_coding_guard;
    return SUCCESS;
}

Support_valuesType* constructSupportValues(
    uint8_t output_symbol_size,
    uint8_t coding_symbol_size,
    uint8_t coding_order,
    bool share_subsym_lut_flag,
    bool share_subsym_prv_flag
){
    Support_valuesType* supportValues = (Support_valuesType*)malloc(sizeof(Support_valuesType));
    if(supportValues == NULL){
        return NULL;
    }
    supportValues->output_symbol_size = output_symbol_size;
    supportValues->coding_symbol_size = coding_symbol_size;
    supportValues->coding_order = coding_order;
    supportValues->share_subsym_lut_flag = share_subsym_lut_flag;
    supportValues->share_subsym_prv_flag = share_subsym_prv_flag;
}

Cabac_binarization_parametersType* constructCABACBinarizationParameters(
    bool bIFullCtxMode,
    uint8_t cTruncExpGolParam,
    uint8_t cMaxDTU,
    uint8_t splitUnitSize
){
    Cabac_binarization_parametersType* cabac_binarization_parameters =
            (Cabac_binarization_parametersType*)malloc((sizeof(Cabac_binarization_parametersType)));
    if(cabac_binarization_parameters == NULL){
        return NULL;
    }
    cabac_binarization_parameters->bIFullCtxMode = bIFullCtxMode;
    cabac_binarization_parameters->cTruncExpGolParam = cTruncExpGolParam;
    cabac_binarization_parameters->cMaxDTU = cMaxDTU;
    cabac_binarization_parameters->splitUnitSize = splitUnitSize;
    return cabac_binarization_parameters;
}
//0
Cabac_binarization_parametersType* constructCABACBinarizationParams_BinaryCoding(
    bool BIFullCtxMode
){
    return constructCABACBinarizationParameters(BIFullCtxMode,0,0,0);
}
//1
Cabac_binarization_parametersType* constructCABACBinarizationParams_TruncatedUnary(){
    return constructCABACBinarizationParameters(false,0,0,0);
}
//2
Cabac_binarization_parametersType* constructCABACBinarizationParams_ExponentialGolomb(){
    return constructCABACBinarizationParameters(false,0,0,0);
}
//3
Cabac_binarization_parametersType* constructCABACBinarizationParams_SignedExponentialGolomb(){
    return constructCABACBinarizationParameters(false,0,0,0);
}
//4
Cabac_binarization_parametersType* constructCABACBinarizationParams_TruncatedExponentialGolomb(
    uint8_t cTruncExpGolParam
){
    return constructCABACBinarizationParameters(false,cTruncExpGolParam,0,0);
}
//5
Cabac_binarization_parametersType* constructCABACBinarizationParams_SignedTruncatedExponentialGolomb(
        uint8_t cTruncExpGolParam
){
    return constructCABACBinarizationParameters(false,cTruncExpGolParam,0,0);
}
//6
Cabac_binarization_parametersType* constructCABACBinarizationParams_SplitUnitWiseTruncatedUnary(
        uint8_t splitUnitSize
){
    return constructCABACBinarizationParameters(false,0,0,splitUnitSize);
}
//7
Cabac_binarization_parametersType* construtCABACBinarizationParams_SignedSplitUnitWiseTruncatedUnary(
    uint8_t splitUnitSize
){
    return constructCABACBinarizationParameters(false,0,0,splitUnitSize);
}
//8
Cabac_binarization_parametersType* construtCABACBinarizationParams_DoubleTruncatedUnary(
        uint8_t cMaxDTU,
        uint8_t splitUnitSize
){
    return constructCABACBinarizationParameters(false,0,cMaxDTU,splitUnitSize);
}
//9
Cabac_binarization_parametersType* construtCABACBinarizationParams_SignedDoubleTruncatedUnary(
        uint8_t cMaxDTU,
        uint8_t splitUnitSize
){
    return constructCABACBinarizationParameters(false,0,cMaxDTU,splitUnitSize);
}

Cabac_context_parametersType* constructCABACContextParameters(
    bool adaptive_mode_flag,
    uint16_t num_contexts,
    uint8_t* context_initialization_value,
    bool share_sub_sym_ctx_flag,
    bool segment_ctx_flag,
    uint16_t segment_ctx_len,
    uint16_t segment_ctx_offset
){
    Cabac_context_parametersType* result = (Cabac_context_parametersType*)malloc(sizeof(Cabac_context_parametersType));
    if(result == NULL){
        return result;
    }
    result->adaptive_mode_flag = adaptive_mode_flag;
    result->num_contexts = num_contexts;
    result->context_initialization_value = context_initialization_value;
    result->share_sub_sym_ctx_flag = share_sub_sym_ctx_flag;
    result->segment_ctx_flag = segment_ctx_flag;
    result->segment_ctx_len = segment_ctx_len;
    result->segment_ctx_offset = segment_ctx_offset;
    return result;
}

Cabac_binarizationsType* constructCabacBinarizationBinaryCoding_NotBypass(
        bool BIFullCtxMode,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_BinaryCoding;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_BinaryCoding(BIFullCtxMode);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
        adaptive_mode_flag,
        num_contexts,
        context_initialization_value,
        share_sub_sym_ctx_flag,
        segment_ctx_flag,
        segment_ctx_len,
        segment_ctx_offset
    );
    if(cabac_binarizations->cabac_context_parameters == NULL){
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
    release_cabac_binarization_parameters: free(cabac_binarizations->cabac_binarization_parameters);
    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationBinaryCoding_Bypass(
        bool BIFullCtxMode
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_BinaryCoding;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_BinaryCoding(BIFullCtxMode);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationTruncatedUnary_NotBypass(
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_TruncatedUnary;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_TruncatedUnary();
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
            adaptive_mode_flag,
            num_contexts,
            context_initialization_value,
            share_sub_sym_ctx_flag,
            segment_ctx_flag,
            segment_ctx_len,
            segment_ctx_offset
    );
    if(cabac_binarizations->cabac_context_parameters == NULL){
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
    release_cabac_binarization_parameters: free(cabac_binarizations->cabac_binarization_parameters);
    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationTruncatedUnary_Bypass(){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_TruncatedUnary;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_TruncatedUnary();
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationExponentialGolomb_NotBypass(
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_ExponentialGolomb;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_ExponentialGolomb();
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
            adaptive_mode_flag,
            num_contexts,
            context_initialization_value,
            share_sub_sym_ctx_flag,
            segment_ctx_flag,
            segment_ctx_len,
            segment_ctx_offset
    );
    if(cabac_binarizations->cabac_context_parameters == NULL){
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
    release_cabac_binarization_parameters: free(cabac_binarizations->cabac_binarization_parameters);
    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationExponentialGolomb_Bypass(){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_ExponentialGolomb;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_ExponentialGolomb();
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedExponentialGolomb_NotBypass(
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedExponentialGolomb;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_SignedExponentialGolomb();
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
            adaptive_mode_flag,
            num_contexts,
            context_initialization_value,
            share_sub_sym_ctx_flag,
            segment_ctx_flag,
            segment_ctx_len,
            segment_ctx_offset
    );
    if(cabac_binarizations->cabac_context_parameters == NULL){
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
    release_cabac_binarization_parameters: free(cabac_binarizations->cabac_binarization_parameters);
    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedExponentialGolomb_Bypass(){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedExponentialGolomb;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_SignedExponentialGolomb();
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationTruncatedExponentialGolomb_NotBypass(
        uint8_t cTruncExpGolParam,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_TruncatedExponentialGolomb;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
            constructCABACBinarizationParams_TruncatedExponentialGolomb(cTruncExpGolParam);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
            adaptive_mode_flag,
            num_contexts,
            context_initialization_value,
            share_sub_sym_ctx_flag,
            segment_ctx_flag,
            segment_ctx_len,
            segment_ctx_offset
    );
    if(cabac_binarizations->cabac_context_parameters == NULL){
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
    release_cabac_binarization_parameters: free(cabac_binarizations->cabac_binarization_parameters);
    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationTruncatedExponentialGolomb_Bypass(
        uint8_t cTruncExpGolParam
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_TruncatedExponentialGolomb;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
            constructCABACBinarizationParams_TruncatedExponentialGolomb(cTruncExpGolParam);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedTruncatedExponentialGolomb_NotBypass(
        uint8_t cTruncExpGolParam,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedTruncatedExponentialGolomb;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
            constructCABACBinarizationParams_SignedTruncatedExponentialGolomb(cTruncExpGolParam);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
            adaptive_mode_flag,
            num_contexts,
            context_initialization_value,
            share_sub_sym_ctx_flag,
            segment_ctx_flag,
            segment_ctx_len,
            segment_ctx_offset
    );
    if(cabac_binarizations->cabac_context_parameters == NULL){
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
    release_cabac_binarization_parameters: free(cabac_binarizations->cabac_binarization_parameters);
    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedTruncatedExponentialGolomb_Bypass(
        uint8_t cTruncExpGolParam
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedTruncatedExponentialGolomb;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
            constructCABACBinarizationParams_SignedTruncatedExponentialGolomb(cTruncExpGolParam);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSplitUnitwiseTruncatedUnary_NotBypass(
        uint8_t splitUnitSize,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SplitUnitWiseTruncatedUnary;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
            constructCABACBinarizationParams_SplitUnitWiseTruncatedUnary(splitUnitSize);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
            adaptive_mode_flag,
            num_contexts,
            context_initialization_value,
            share_sub_sym_ctx_flag,
            segment_ctx_flag,
            segment_ctx_len,
            segment_ctx_offset
    );
    if(cabac_binarizations->cabac_context_parameters == NULL){
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
    release_cabac_binarization_parameters: free(cabac_binarizations->cabac_binarization_parameters);
    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSplitUnitwiseTruncatedUnary_Bypass(
        uint8_t splitUnitSize
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SplitUnitWiseTruncatedUnary;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
            constructCABACBinarizationParams_SplitUnitWiseTruncatedUnary(splitUnitSize);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedSplitUnitwiseTruncatedUnary_NotBypass(
        uint8_t splitUnitSize,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedSplitUnitWiseTruncatedUnary;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
            construtCABACBinarizationParams_SignedSplitUnitWiseTruncatedUnary(splitUnitSize);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
            adaptive_mode_flag,
            num_contexts,
            context_initialization_value,
            share_sub_sym_ctx_flag,
            segment_ctx_flag,
            segment_ctx_len,
            segment_ctx_offset
    );
    if(cabac_binarizations->cabac_context_parameters == NULL){
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
    release_cabac_binarization_parameters: free(cabac_binarizations->cabac_binarization_parameters);
    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedSplitUnitwiseTruncatedUnary_Bypass(
        uint8_t splitUnitSize
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedSplitUnitWiseTruncatedUnary;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
            construtCABACBinarizationParams_SignedSplitUnitWiseTruncatedUnary(splitUnitSize);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationDoubleTruncatedUnary_NotBypass(
        uint8_t cMaxDTU,
        uint8_t splitUnitSize,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_DoubleTruncatedUnary;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
            construtCABACBinarizationParams_DoubleTruncatedUnary(cMaxDTU, splitUnitSize);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
            adaptive_mode_flag,
            num_contexts,
            context_initialization_value,
            share_sub_sym_ctx_flag,
            segment_ctx_flag,
            segment_ctx_len,
            segment_ctx_offset
    );
    if(cabac_binarizations->cabac_context_parameters == NULL){
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
    release_cabac_binarization_parameters: free(cabac_binarizations->cabac_binarization_parameters);
    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationDoubleTruncatedUnary_Bypass(
        uint8_t cMaxDTU,
        uint8_t splitUnitSize
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_DoubleTruncatedUnary;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
            construtCABACBinarizationParams_DoubleTruncatedUnary(cMaxDTU, splitUnitSize);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

    release_parent: free(cabac_binarizations);
    return NULL;
}


Cabac_binarizationsType* constructCabacBinarizationSignedDoubleTruncatedUnary_NotBypass(
        uint8_t cMaxDTU,
        uint8_t splitUnitSize,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedDoubleTruncatedUnary;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
            construtCABACBinarizationParams_SignedDoubleTruncatedUnary(cMaxDTU, splitUnitSize);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
            adaptive_mode_flag,
            num_contexts,
            context_initialization_value,
            share_sub_sym_ctx_flag,
            segment_ctx_flag,
            segment_ctx_len,
            segment_ctx_offset
    );
    if(cabac_binarizations->cabac_context_parameters == NULL){
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
    release_cabac_binarization_parameters: free(cabac_binarizations->cabac_binarization_parameters);
    release_parent: free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedDoubleTruncatedUnary_Bypass(
        uint8_t cMaxDTU,
        uint8_t splitUnitSize
){
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if(cabac_binarizations == NULL){
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedDoubleTruncatedUnary;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
            construtCABACBinarizationParams_SignedDoubleTruncatedUnary(cMaxDTU, splitUnitSize);
    if(cabac_binarizations->cabac_binarization_parameters == NULL){
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

    release_parent: free(cabac_binarizations);
    return NULL;
}

DecoderConfigurationTypeCABAC* constructDecoderConfigurationCABAC(
    uint8_t num_descriptor_subsequence_cfgs_minus1,
    uint16_t* descriptor_subsequence_ID,
    Transform_subseq_parametersType** transform_subseq_parameters,
    TransformSubSymIdEnum** transform_id_subsym,
    Support_valuesType*** support_values,
    Cabac_binarizationsType*** cabac_binarizations
){
    DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC =
            (DecoderConfigurationTypeCABAC *)malloc(sizeof(DecoderConfigurationTypeCABAC));
    if(decoderConfigurationTypeCABAC == NULL){
        return NULL;
    }

    decoderConfigurationTypeCABAC->num_descriptor_subsequence_cfgs_minus1 = num_descriptor_subsequence_cfgs_minus1;
    decoderConfigurationTypeCABAC->descriptor_subsequence_ID = descriptor_subsequence_ID;
    decoderConfigurationTypeCABAC->transform_subseq_parameters = transform_subseq_parameters;
    decoderConfigurationTypeCABAC->transform_id_subsym = transform_id_subsym;
    decoderConfigurationTypeCABAC->support_values = support_values;
    decoderConfigurationTypeCABAC->cabac_binarizations = cabac_binarizations;

    return decoderConfigurationTypeCABAC;
}

Decoder_configuration_tokentype_cabac* contructDecoder_configuration_tokentypeCABAC(
    uint8_t output_symbol_size,
    uint8_t coding_symbols_numbers,
    const uint8_t coding_symbol_size[64],
    TransformSubSymIdEnum transformSubSymId,
    Support_valuesType* support_values,
    Cabac_binarizationsType* cabac_binarizations
){
    int total_coded_symbol = 0;
    for(int i=0; i<coding_symbols_numbers; i++){
        total_coded_symbol += coding_symbol_size[i];
    }
    if(total_coded_symbol != output_symbol_size){
        return NULL;
    }
    Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac =
            (Decoder_configuration_tokentype_cabac*)malloc(sizeof(Decoder_configuration_tokentype_cabac));
    if(decoderConfigurationTokentypeCabac == NULL){
        return NULL;
    }
    decoderConfigurationTokentypeCabac->output_symbol_size = output_symbol_size;
    decoderConfigurationTokentypeCabac->coding_symbols_numbers = coding_symbols_numbers;
    for(int i=0; i<coding_symbols_numbers; i++) {
        decoderConfigurationTokentypeCabac->coding_symbol_size[i] = coding_symbol_size[i];
    }
    decoderConfigurationTokentypeCabac->transformSubSym = transformSubSymId;
    decoderConfigurationTokentypeCabac->support_values = support_values;
    decoderConfigurationTokentypeCabac->cabac_binarizations = cabac_binarizations;

    return decoderConfigurationTokentypeCabac;
}


Decoder_configuration_tokentype* constructDecoderConfigurationTokentype(
    uint8_t rle_guard_tokentype,
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac0,
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac1
){
    if(decoder_configuration_tokentype_cabac0 == NULL || decoder_configuration_tokentype_cabac1 == NULL){
        return NULL;
    }
    Decoder_configuration_tokentype* decoderConfigurationTokentypeTypeCABAC =
            (Decoder_configuration_tokentype *)malloc(sizeof(Decoder_configuration_tokentype));
    if(decoderConfigurationTokentypeTypeCABAC == NULL){
        return NULL;
    }

    decoderConfigurationTokentypeTypeCABAC->rle_guard_tokentype = rle_guard_tokentype;
    decoderConfigurationTokentypeTypeCABAC->decoder_configuration_tokentype_cabac0 =
            decoder_configuration_tokentype_cabac0;
    decoderConfigurationTokentypeTypeCABAC->decoder_configuration_tokentype_cabac1 =
            decoder_configuration_tokentype_cabac1;

    return decoderConfigurationTokentypeTypeCABAC;
}

Parameter_set_qvpsType* constructParameterSetQvpsType(
    uint8_t qv_num_codebooks_total,
    uint8_t* qv_num_codebook_entries,
    uint8_t** qv_recon
){
    Parameter_set_qvpsType* parameter_set_qvpsType = (Parameter_set_qvpsType*)malloc(sizeof(Parameter_set_qvpsType));
    if(parameter_set_qvpsType == NULL){
        return NULL;
    }
    parameter_set_qvpsType->qv_num_codebooks_total = qv_num_codebooks_total;
    parameter_set_qvpsType->qv_num_codebook_entries = qv_num_codebook_entries;
    parameter_set_qvpsType->qv_recon = qv_recon;
    return parameter_set_qvpsType;
}

Parameter_set_crpsType* constructParameterSetCRPS(CrAlgEnum cr_alg_ID, uint8_t cr_pad_size, uint32_t cr_buf_max_size){
    Parameter_set_crpsType* parameterSetCrps = (Parameter_set_crpsType*)malloc(sizeof(Parameter_set_crpsType));
    if(parameterSetCrps == NULL){
        return NULL;
    }

    parameterSetCrps->cr_alg_ID = cr_alg_ID;
    parameterSetCrps->cr_pad_size = cr_pad_size;
    parameterSetCrps->cr_buf_max_size = cr_buf_max_size;

    return parameterSetCrps;
}

Parameter_set_crpsType* constructParameterSetCRPSNoComp(){
    return constructParameterSetCRPS(CrAlg_NoComp, 0, 0);
}

Parameter_set_crpsType* constructParameterSetRefTransform(){
    return constructParameterSetCRPS(CrAlg_RefTransform, 0, 0);
}

Parameter_set_crpsType* constructParameterSetPushIn(
    uint8_t cr_pad_size,
    uint32_t cr_buf_max_size
){
    return constructParameterSetCRPS(CrAlg_PushIn, cr_pad_size, cr_buf_max_size);
}

Parameter_set_crpsType* constructParameterSetLocalAssembly(
        uint8_t cr_pad_size,
        uint32_t cr_buf_max_size
){
    return constructParameterSetCRPS(CrAlg_LocalAssembly, cr_pad_size, cr_buf_max_size);
}

Parameter_set_crpsType* constructParameterSetGlobalAssembly(
        uint8_t cr_pad_size,
        uint32_t cr_buf_max_size
){
    return constructParameterSetCRPS(CrAlg_GlobalAssembly, cr_pad_size, cr_buf_max_size);
}

Encoding_ParametersType* constructEncodingParameters(
    uint8_t datasetType,
    uint8_t alphabetId,
    uint32_t reads_length,
    uint8_t number_of_template_segments_minus1,
    uint32_t max_au_data_unit_size,
    bool pos40bits,
    uint8_t qv_depth,
    uint8_t as_depth,
    uint8_t numClasses,
    uint8_t* classID,
    uint16_t numGroups,
    char** rgroupId,
    bool splicedReadsFlag,
    bool multipleSignatureFlag,
    uint32_t multipleSignatureBase,
    uint8_t U_signature_size,
    bool multiple_alignments_flag,
    uint8_t mscore_mantissa,
    bool crps_flag,
    Parameter_set_crpsType* parameter_set_crps
){
    Encoding_ParametersType* encodingParametersType =
            (Encoding_ParametersType *)malloc(sizeof(Encoding_ParametersType));
    if(encodingParametersType == NULL){
        return NULL;
    }

    encodingParametersType->dataset_type = datasetType;
    encodingParametersType->alphabet_ID = alphabetId;
    encodingParametersType->reads_length = reads_length;
    encodingParametersType->number_of_template_segments_minus1 = number_of_template_segments_minus1;
    encodingParametersType->max_au_data_unit_size = max_au_data_unit_size;
    encodingParametersType->pos_40_bits = pos40bits;
    encodingParametersType->qv_depth = qv_depth;
    encodingParametersType->as_depth = as_depth;
    encodingParametersType->num_classes = numClasses; //todo initialize other fields
    encodingParametersType->dec_cfg_flag = (bool**)malloc(encodingParametersType->num_classes*sizeof(bool*));
    if(encodingParametersType->dec_cfg_flag == NULL){
        goto release_container;
    }
    for(int i=0; i<numClasses; i++){
        encodingParametersType->dec_cfg_flag[i]=(bool*)malloc(18*sizeof(bool));
        if(encodingParametersType->dec_cfg_flag[i] == NULL){
            goto release_dec_cfg_flag;
        }
    }
    encodingParametersType->encoding_mode_id = (uint8_t**)malloc(encodingParametersType->num_classes* sizeof(uint8_t*));
    if(encodingParametersType->encoding_mode_id == NULL){
        goto release_dec_cfg_flag;
    }
    for(int i=0; i<numClasses; i++){
        encodingParametersType->encoding_mode_id[i]=(uint8_t *)malloc(18*sizeof(uint8_t));
        if(encodingParametersType->encoding_mode_id[i] == NULL){
            goto release_encoding_mode;
        }
    }
    encodingParametersType->decoderConfiguration = (void***)malloc(encodingParametersType->num_classes* sizeof(void**));
    if(encodingParametersType->decoderConfiguration == NULL){
        goto release_encoding_mode;
    }
    for(int i=0; i<numClasses; i++){
        // encodingParametersType->decoderConfiguration[i]=malloc(18*sizeof(void*));
        if(encodingParametersType->encoding_mode_id[i] == NULL){
            goto release_decoderConfiguration;
        }
    }
    for(int classId=0; classId<numClasses; classId++){
        encodingParametersType->class_specific_dec_cfg_flag[classId] = false;
        for(int descId=0; descId<18; descId++){
            encodingParametersType->dec_cfg_flag[classId][descId] = false;
            encodingParametersType->decoderConfiguration[classId][descId] = NULL;
        }
    }

    encodingParametersType->classID = classID;
    encodingParametersType->num_groups = numGroups;
    encodingParametersType->rgroup_ID = rgroupId;
    encodingParametersType->spliced_reads_flag = splicedReadsFlag;
    encodingParametersType->multiple_signature_flag = multipleSignatureFlag;
    encodingParametersType->multiple_signature_base = multipleSignatureBase;
    encodingParametersType->U_signature_size = U_signature_size;
    encodingParametersType->multiple_alignments_flag = multiple_alignments_flag;
    encodingParametersType->mscore_mantissa = mscore_mantissa;
    encodingParametersType->qv_coding_mode = (uint8_t*)malloc(sizeof(uint8_t)*encodingParametersType->num_classes);
    if(encodingParametersType->qv_coding_mode == NULL){
        goto release_decoderConfiguration;
    }
    encodingParametersType->qvps_flag = (bool*)malloc(sizeof(bool)*encodingParametersType->num_classes);
    if(encodingParametersType->qvps_flag == NULL){
        goto release_QVCodingMode;
    }
    encodingParametersType->parameter_set_qvps =
            (Parameter_set_qvpsType**)malloc(sizeof(Parameter_set_qvpsType*)*encodingParametersType->num_classes);
    if(encodingParametersType->parameter_set_qvps == NULL){
        goto release_qvpsFlag;
    }
    encodingParametersType->default_qvps_ID =
            (uint8_t *)malloc(sizeof(uint8_t)*encodingParametersType->num_classes);
    if(encodingParametersType->default_qvps_ID == NULL){
        goto release_ParameterSetQVPS;
    }
    for(int classId = 0; classId < encodingParametersType->num_classes; classId++){
        encodingParametersType->qv_coding_mode[classId] = 1;
        encodingParametersType->parameter_set_qvps[classId] = NULL;
        encodingParametersType->default_qvps_ID = 0;
    }
    encodingParametersType->crps_flag = crps_flag;
    encodingParametersType->parameter_set_crps = parameter_set_crps;

    return encodingParametersType;

    release_QVCodingMode: free(encodingParametersType->qv_coding_mode);
    release_qvpsFlag: free(encodingParametersType->qvps_flag);
    release_ParameterSetQVPS: free(encodingParametersType->parameter_set_qvps);

release_encoding_mode: for(int i=0; i<numClasses; i++) {
        free(encodingParametersType->encoding_mode_id[i]);
    }
    free(encodingParametersType->encoding_mode_id);

release_decoderConfiguration:  for(int i=0; i<numClasses; i++) {
        free(encodingParametersType->decoderConfiguration[i]);
    }
    free(encodingParametersType->decoderConfiguration);

release_dec_cfg_flag: for(int i=0; i<numClasses; i++) {
        free(encodingParametersType->dec_cfg_flag[i]);
    }
    free(encodingParametersType->dec_cfg_flag);

release_container: free(encodingParametersType);
    return NULL;
}

Encoding_ParametersType* constructEncodingParametersSingleAlignmentNoComputed(
        uint8_t datasetType,
        uint8_t alphabetId,
        uint32_t reads_length,
        uint8_t number_of_template_segments_minus1,
        uint32_t max_au_data_unit_size,
        bool pos40bits,
        uint8_t qv_depth,
        uint8_t as_depth,
        uint8_t numClasses,
        uint8_t* classID,
        uint16_t numGroups,
        char** rgroupId,
        bool splicedReadsFlag,
        bool multipleSignatureFlag,
        uint32_t multipleSignatureBase,
        uint8_t U_signature_size
){
    return constructEncodingParameters(
            datasetType,
            alphabetId,
            reads_length,
            number_of_template_segments_minus1,
            max_au_data_unit_size,
            pos40bits,
            qv_depth,
            as_depth,
            numClasses,
            classID,
            numGroups,
            rgroupId,
            splicedReadsFlag,
            multipleSignatureFlag,
            multipleSignatureBase,
            U_signature_size,
            false,
            0,
            false,
            NULL
    );
}

Encoding_ParametersType* constructEncodingParametersMultipleAlignmentNoComputed(
        uint8_t datasetType,
        uint8_t alphabetId,
        uint32_t reads_length,
        uint8_t number_of_template_segments_minus1,
        uint32_t max_au_data_unit_size,
        bool pos40bits,
        uint8_t qv_depth,
        uint8_t as_depth,
        uint8_t numClasses,
        uint8_t* classID,
        uint16_t numGroups,
        char** rgroupId,
        bool splicedReadsFlag,
        bool multipleSignatureFlag,
        uint32_t multipleSignatureBase,
        uint8_t U_signature_size,
        uint8_t mscore_mantissa
){
    return constructEncodingParameters(
            datasetType,
            alphabetId,
            reads_length,
            number_of_template_segments_minus1,
            max_au_data_unit_size,
            pos40bits,
            qv_depth,
            as_depth,
            numClasses,
            classID,
            numGroups,
            rgroupId,
            splicedReadsFlag,
            multipleSignatureFlag,
            multipleSignatureBase,
            U_signature_size,
            true,
            mscore_mantissa,
            false,
            NULL
    );
}

Encoding_ParametersType* constructEncodingParametersSingleAlignmentComputedRef(
        uint8_t datasetType,
        uint8_t alphabetId,
        uint32_t reads_length,
        uint8_t number_of_template_segments_minus1,
        uint32_t max_au_data_unit_size,
        bool pos40bits,
        uint8_t qv_depth,
        uint8_t as_depth,
        uint8_t numClasses,
        uint8_t* classID,
        uint16_t numGroups,
        char** rgroupId,
        bool splicedReadsFlag,
        bool multipleSignatureFlag,
        uint32_t multipleSignatureBase,
        uint8_t U_signature_size,
        Parameter_set_crpsType* parameter_set_crps
){
    return constructEncodingParameters(
            datasetType,
            alphabetId,
            reads_length,
            number_of_template_segments_minus1,
            max_au_data_unit_size,
            pos40bits,
            qv_depth,
            as_depth,
            numClasses,
            classID,
            numGroups,
            rgroupId,
            splicedReadsFlag,
            multipleSignatureFlag,
            multipleSignatureBase,
            U_signature_size,
            false,
            0,
            true,
            parameter_set_crps
    );
}

Encoding_ParametersType* constructEncodingParametersMultipleAlignmentComputedRef(
        uint8_t datasetType,
        uint8_t alphabetId,
        uint32_t reads_length,
        uint8_t number_of_template_segments_minus1,
        uint32_t max_au_data_unit_size,
        bool pos40bits,
        uint8_t qv_depth,
        uint8_t as_depth,
        uint8_t numClasses,
        uint8_t* classID,
        uint16_t numGroups,
        char** rgroupId,
        bool splicedReadsFlag,
        bool multipleSignatureFlag,
        uint32_t multipleSignatureBase,
        uint8_t U_signature_size,
        uint8_t mscore_mantissa,
        Parameter_set_crpsType* parameter_set_crps
){
    return constructEncodingParameters(
            datasetType,
            alphabetId,
            reads_length,
            number_of_template_segments_minus1,
            max_au_data_unit_size,
            pos40bits,
            qv_depth,
            as_depth,
            numClasses,
            classID,
            numGroups,
            rgroupId,
            splicedReadsFlag,
            multipleSignatureFlag,
            multipleSignatureBase,
            U_signature_size,
            true,
            mscore_mantissa,
            true,
            parameter_set_crps
    );
}

EncodingParametersRC setNonClassSpecificDescriptorConfigurationAsParent(
    Encoding_ParametersType* encodingParameters,
    uint8_t descId
){
    if(descId >= 18){
        return OUT_OF_BOUNDERIES;
    }
    encodingParameters->class_specific_dec_cfg_flag[descId] = false;
    for(int c=0; c<encodingParameters->num_classes; c++){
        encodingParameters->dec_cfg_flag[c][descId]=0;
    }
    return SUCCESS;
}

EncodingParametersRC setNonClassSpecificDescriptorConfigurationAndEncodingMode(
        Encoding_ParametersType* encodingParameters,
        uint8_t descId,
        void* configuration
){
    if(descId >= 18){
        return OUT_OF_BOUNDERIES;
    }

    encodingParameters->class_specific_dec_cfg_flag[descId] = false;
    for(int c=0; c<encodingParameters->num_classes; c++){
        encodingParameters->dec_cfg_flag[c][descId]=1;
        encodingParameters->decoderConfiguration[c][descId] = configuration;
    }
    return SUCCESS;
}

EncodingParametersRC setClassSpecificDescriptorConfigurationAndEncodingMode(
        Encoding_ParametersType* encodingParameters,
        uint8_t classId,
        uint8_t descId,
        void* configuration,
        uint8_t encodingMode
){
    if(descId >= 18 || classId >= encodingParameters->num_classes){
        return OUT_OF_BOUNDERIES;
    }
    encodingParameters->class_specific_dec_cfg_flag[descId] = true;
    encodingParameters->dec_cfg_flag[classId][descId]=true;
    encodingParameters->decoderConfiguration[classId][descId] = configuration;
    encodingParameters->encoding_mode_id[classId][descId] = encodingMode;

    return SUCCESS;
}

EncodingParametersRC setDefaultQVPSId(
    Encoding_ParametersType* encodingParameters,
    uint8_t classId,
    uint8_t defaultQVPSId
){
    if(classId >= encodingParameters->num_classes){
        return OUT_OF_BOUNDERIES;
    }
    encodingParameters->qvps_flag[classId] = false;
    encodingParameters->default_qvps_ID[classId] = defaultQVPSId;

    return SUCCESS;
}

EncodingParametersRC setParameterSetQVPS(
        Encoding_ParametersType* encodingParameters,
        uint8_t classId,
        Parameter_set_qvpsType* parameterSetQvps
){
    if(classId >= encodingParameters->num_classes){
        return OUT_OF_BOUNDERIES;
    }
    encodingParameters->qvps_flag[classId] = false;
    encodingParameters->parameter_set_qvps[classId] = parameterSetQvps;

    return SUCCESS;
}


EncodingParametersRC getAdaptiveModeFlag(
        Cabac_context_parametersType* cabac_context_parameters,
        bool* adaptive_mode_flag
){
    if(cabac_context_parameters == NULL || adaptive_mode_flag == NULL){
        return NULL_PTR;
    }
    *adaptive_mode_flag = cabac_context_parameters->adaptive_mode_flag;
    return SUCCESS;
}
EncodingParametersRC getContextInitializationValues(
        Cabac_context_parametersType* cabac_context_parameters,
        uint16_t* num_contexts,
        uint8_t** context_initialization_value
){
    if(cabac_context_parameters == NULL || num_contexts == NULL || context_initialization_value == NULL){
        return NULL_PTR;
    }
    *num_contexts = cabac_context_parameters->num_contexts;
    *context_initialization_value = cabac_context_parameters->context_initialization_value;
    return SUCCESS;
}
EncodingParametersRC getShareSubSymCtxFlag(
        Cabac_context_parametersType* cabac_context_parameters,
        uint8_t codingSymbolSize,
        uint8_t outputSymbolSize,
        bool* share_sub_sym_ctx_flag
){
    if(cabac_context_parameters == NULL || share_sub_sym_ctx_flag == NULL){
        return NULL_PTR;
    }
    if(codingSymbolSize < outputSymbolSize){
        *share_sub_sym_ctx_flag = cabac_context_parameters->share_sub_sym_ctx_flag;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}
EncodingParametersRC getSegmentCtxFlag(
        Cabac_context_parametersType* cabac_context_parameters,
        bool* segment_ctx_flag
){
    if(cabac_context_parameters == NULL || segment_ctx_flag == NULL){
        return NULL_PTR;
    }
    *segment_ctx_flag = cabac_context_parameters->segment_ctx_flag;
    return SUCCESS;
}
EncodingParametersRC getSegmentCtxLen(
        Cabac_context_parametersType* cabac_context_parameters,
        uint16_t* segment_ctx_len
){
    if(cabac_context_parameters == NULL || segment_ctx_len == NULL){
        return NULL_PTR;
    }
    if(cabac_context_parameters->segment_ctx_flag){
        *segment_ctx_len = cabac_context_parameters->segment_ctx_len;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}
EncodingParametersRC getSegmentCtxOffset(
        Cabac_context_parametersType* cabac_context_parameters,
        uint16_t* segment_ctx_offset
){
    if(cabac_context_parameters == NULL || segment_ctx_offset == NULL){
        return NULL_PTR;
    }
    if(cabac_context_parameters->segment_ctx_flag){
        *segment_ctx_offset = cabac_context_parameters->segment_ctx_offset;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportOutputSymbolSize(
        Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
        uint8_t* outputSymbolSize
){
    if(decoderConfigurationTokentypeCabac == NULL || outputSymbolSize == NULL){
        return NULL_PTR;
    }
    return getOutputSymbolSize(decoderConfigurationTokentypeCabac->support_values, outputSymbolSize);
}
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportCodingSymbolSize(
        Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
        uint8_t* codingSymbolSize
){
    if(decoderConfigurationTokentypeCabac == NULL || codingSymbolSize == NULL){
        return NULL_PTR;
    }
    return getCodingSymbolSize(decoderConfigurationTokentypeCabac->support_values, codingSymbolSize);
}
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportCodingOrder(
        Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
        uint8_t* codingOrder
){
    if(decoderConfigurationTokentypeCabac == NULL || codingOrder == NULL){
        return NULL_PTR;
    }
    return getCodingOrder(decoderConfigurationTokentypeCabac->support_values, codingOrder);
}
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportShareSubsymLutFlag(
        Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
        bool* shareSubsymLutFlag
){
    if(decoderConfigurationTokentypeCabac == NULL || shareSubsymLutFlag == NULL){
        return NULL_PTR;
    }
    return getShareSubsymLUTFlag(
            decoderConfigurationTokentypeCabac->support_values,
            decoderConfigurationTokentypeCabac->transformSubSym,
            shareSubsymLutFlag
    );
}
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportShareSubsymPrvFlag(
        Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
        bool* shareSubsymPrvFlag
){
    if(decoderConfigurationTokentypeCabac == NULL || shareSubsymPrvFlag == NULL){
        return NULL_PTR;
    }
    return getShareSubsymPRVFlag(
            decoderConfigurationTokentypeCabac->support_values,
            shareSubsymPrvFlag
    );
}
