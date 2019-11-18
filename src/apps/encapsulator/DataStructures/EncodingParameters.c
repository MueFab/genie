//
// Created by daniel on 23/10/18.
//

#include "EncodingParameters.h"
#include <stdlib.h>
#include <string.h>

size_t getSizeDecoderConfigurationCABAC(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC);
EncodingParametersRC getCorrectedTransform_subseq_counter(
    Transform_subseq_parametersType* transform_subseq_parametersType, uint8_t* transform_subseq_counter);

size_t getSizeSupportValues(Support_valuesType* ptr, TransformSubSymIdEnum param);

size_t getSizeCABACBinarizations(Cabac_binarizationsType* cabac_binarizationsType, uint8_t coding_symbol_size,
                                 uint8_t output_symbol_size);

size_t getSizeCABACBinarizationParameters(Cabac_binarization_parametersType* cabac_binarizations,
                                          BinarizationIdEnum binarizationId);

size_t getSizeCABAContextParameters(Cabac_context_parametersType* cabac_context_parameters, uint8_t coding_symbol_size,
                                    uint8_t output_symbol_size);
size_t getSizeDecoderConfigurationTokenTypeCABAC(
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac);

size_t getSizeTransformSubseqParameters(Transform_subseq_parametersType* transform_subseq_parameters);

Parameter_set_qvpsType* readParameterSetQvps(InputBitstream* input);

size_t getSizeParameterSetQvps(Parameter_set_qvpsType* parameter_set_qvps);

size_t getSizeDecoderConfigurationTokenType(Decoder_configuration_tokentype* decoder_configuration_tokentype);

size_t getSizeEncodingParameters(Encoding_ParametersType* encodingParameters) {
    size_t sizeInBits = 0;
    sizeInBits += 4;   // dataset_type
    sizeInBits += 8;   // alphabet_ID
    sizeInBits += 24;  // read_length
    sizeInBits += 2;   // number_of_template_segments_minus1
    sizeInBits += 6;   // reserved
    sizeInBits += 29;  // max_au_data_unit_size
    sizeInBits += 1;   // pos_40_bits_flag
    sizeInBits += 3;   // qv_depth
    sizeInBits += 3;   // as_depth
    sizeInBits += 4;   // numClasses;

    uint8_t numClasses;
    getNumClasses(encodingParameters, &numClasses);
    sizeInBits += numClasses * 4;

    for (int desc_ID = 0; desc_ID < 18; desc_ID++) {
        sizeInBits += 1;  // class_specific_dec_cfg_flag

        int endLoop = encodingParameters->class_specific_dec_cfg_flag[desc_ID] ? encodingParameters->num_classes : 1;
        for (int c = 0; c < endLoop; c++) {
            sizeInBits += 8;  // dec_cfg_preset
            sizeInBits += 8;  // encoding_mode_id
            if (desc_ID != 11 && desc_ID != 15) {
                sizeInBits += getSizeDecoderConfigurationCABAC(
                    (DecoderConfigurationTypeCABAC*)(encodingParameters->decoderConfiguration[desc_ID][c]));
            } else {
                sizeInBits += getSizeDecoderConfigurationTokenType(
                    (Decoder_configuration_tokentype*)(encodingParameters->decoderConfiguration[desc_ID][c]));
            }
        }
    }
    sizeInBits += 16;  // num_groups
    for (int j = 0; j < encodingParameters->num_groups; j++) {
        size_t reagGroupId_len = strlen(encodingParameters->rgroup_ID[j]);
        sizeInBits += (reagGroupId_len + 1) * 8;
    }
    sizeInBits += 1;   // multiple_alignments_flag
    sizeInBits += 1;   // spliced_reads_flag
    sizeInBits += 31;  // multiple_signature_base

    if (encodingParameters->multiple_signature_base > 0) {
        sizeInBits += 6;  // u_signature_size
    }

    for (int c = 0; c < encodingParameters->num_classes; c++) {
        sizeInBits += 4;  // qv_coding_mode
        if (encodingParameters->qv_coding_mode[c] == 1) {
            sizeInBits += 1;  // qvps_flag
            if (encodingParameters->qvps_flag[c]) {
                sizeInBits += getSizeParameterSetQvps(encodingParameters->parameter_set_qvps[c]);
            } else {
                sizeInBits += 4;  // qvps_preset_ID
            }
        }
        sizeInBits += 1;  // qv_reverse_flag
    }
    sizeInBits += 1;  // crps_flag
    if (encodingParameters->crps_flag) {
        sizeInBits += getSizeParametersCrps(encodingParameters->parameter_set_crps);
    }
    return 1 + ((sizeInBits - 1) / 8);
}

size_t getSizeDecoderConfigurationTokenType(
    Decoder_configuration_tokentype* decoder_configuration_tokentype_typeCABAC) {
    size_t sizeInBits = 0;
    sizeInBits += 8;  // rle_guard_tokentype
    sizeInBits += getSizeDecoderConfigurationTokenTypeCABAC(
        decoder_configuration_tokentype_typeCABAC->decoder_configuration_tokentype_cabac0);
    sizeInBits += getSizeDecoderConfigurationTokenTypeCABAC(
        decoder_configuration_tokentype_typeCABAC->decoder_configuration_tokentype_cabac1);
    return sizeInBits;
}

size_t getSizeDecoderConfigurationTokenTypeCABAC(
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac) {
    size_t sizeInBits = 0;
    sizeInBits +=
        getSizeTransformSubseqParameters(decoder_configuration_tokentype_cabac->transform_subseq_parametersType);

    uint8_t transformSubseqCounter;

    getTransformSubseqCounterTokenType(decoder_configuration_tokentype_cabac, &transformSubseqCounter);
    for (uint8_t transformSubseq_i = 0; transformSubseq_i < transformSubseqCounter; transformSubseq_i++) {
        sizeInBits += 3;  // transformSubSym[transformSubseq_i];
        sizeInBits += getSizeSupportValues(decoder_configuration_tokentype_cabac->support_values[transformSubseq_i],
                                           decoder_configuration_tokentype_cabac->transformSubSym[transformSubseq_i]);

        sizeInBits += getSizeCABACBinarizations(
            decoder_configuration_tokentype_cabac->cabac_binarizations[transformSubseq_i],
            decoder_configuration_tokentype_cabac->support_values[transformSubseq_i]->coding_symbol_size,
            decoder_configuration_tokentype_cabac->support_values[transformSubseq_i]->output_symbol_size);
    }
    return sizeInBits;
}

size_t getSizeParameterSetQvps(Parameter_set_qvpsType* parameter_set_qvps) {
    size_t sizeInBits = 0;
    sizeInBits += 4;  // qv_num_codebooks_total
    for (uint8_t b = 0; b < parameter_set_qvps->qv_num_codebooks_total; b++) {
        sizeInBits += 8;  // qv_num_codebook_entries
        sizeInBits += parameter_set_qvps->qv_num_codebook_entries[b] * 8;
    }
    return sizeInBits;
}

size_t getSizeDecoderConfigurationCABAC(DecoderConfigurationTypeCABAC* decoderConfigurationCABAC) {
    size_t sizeInBits = 0;
    sizeInBits += 8;  // num_descriptor_subsequence_cfgs_minus1

    for (uint16_t i = 0; i <= decoderConfigurationCABAC->num_descriptor_subsequence_cfgs_minus1; i++) {
        sizeInBits += 10;  // decoderConfigurationCABAC->descriptor_subsequence_ID[i])
        sizeInBits += getSizeTransformSubseqParameters(decoderConfigurationCABAC->transform_subseq_parameters[i]);
        uint8_t transform_subseq_counter;
        if (getCorrectedTransform_subseq_counter(decoderConfigurationCABAC->transform_subseq_parameters[i],
                                                 &transform_subseq_counter) != SUCCESS) {
            return false;
        }

        for (int j = 0; j < transform_subseq_counter; j++) {
            uint8_t transformIdSubSym = decoderConfigurationCABAC->transform_id_subsym[i][j];
            sizeInBits += 3;  // transformIdSubSym
            sizeInBits += getSizeSupportValues(decoderConfigurationCABAC->support_values[i][j],
                                               decoderConfigurationCABAC->transform_id_subsym[i][j]);
            sizeInBits +=
                getSizeCABACBinarizations(decoderConfigurationCABAC->cabac_binarizations[i][j],
                                          decoderConfigurationCABAC->support_values[i][j]->coding_symbol_size,
                                          decoderConfigurationCABAC->support_values[i][j]->output_symbol_size);
        }
    }
    return sizeInBits;
}

size_t getSizeTransformSubseqParameters(Transform_subseq_parametersType* transform_subseq_parameters) {
    size_t sizeInBits = 0;

    sizeInBits += 8;  // transform_ID_subseq

    if (transform_subseq_parameters->transform_ID_subseq == SubSeq_EQUALITY_CODING) {
        /*Nothing to write*/
    } else if (transform_subseq_parameters->transform_ID_subseq == SubSeq_MATCH_CODING) {
        sizeInBits += 16;  // match_coding_buffer_size
    } else if (transform_subseq_parameters->transform_ID_subseq == SubSeq_RLE_CODING) {
        sizeInBits += 8;  // rleCodingGuard
    } else if (transform_subseq_parameters->transform_ID_subseq == SubSeq_MERGE_CODING) {
        sizeInBits += 4;  // merge_coding_subseq_count
        for (uint8_t merge_coding_i = 0; merge_coding_i < transform_subseq_parameters->merge_coding_subseq_count;
             merge_coding_i++) {
            sizeInBits += 5;  // merge_coding_shift_size[merge_coding_i]
        }
    }
    return sizeInBits;
}

size_t getSizeCABACBinarizations(Cabac_binarizationsType* cabac_binarizationsType, uint8_t coding_symbol_size,
                                 uint8_t output_symbol_size) {
    size_t sizeInBits = 0;

    sizeInBits += 5;  // binarizationId
    sizeInBits += 1;  // bypass_flag
    sizeInBits += getSizeCABACBinarizationParameters(cabac_binarizationsType->cabac_binarization_parameters,
                                                     cabac_binarizationsType->binarization_ID);
    uint8_t binarizationIdValue = (uint8_t)cabac_binarizationsType->binarization_ID;
    if (!cabac_binarizationsType->bypass_flag) {
        sizeInBits += getSizeCABAContextParameters(cabac_binarizationsType->cabac_context_parameters,
                                                   coding_symbol_size, output_symbol_size);
    }
    return sizeInBits;
}

size_t getSizeCABAContextParameters(Cabac_context_parametersType* cabac_context_parameters, uint8_t codingSubsymbolSize,
                                    uint8_t outputSubsymbolSize) {
    size_t sizeInBits = 0;
    sizeInBits += 1;   // adaptive_mode_flag
    sizeInBits += 16;  // num_contexts

    for (int i = 0; i < cabac_context_parameters->num_contexts; i++) {
        sizeInBits += 7;  // context_initialization_value;
    }
    if (codingSubsymbolSize < outputSubsymbolSize) {
        sizeInBits += 1;  // share_sub_sym_ctx_flag
    }
    return sizeInBits;
}

size_t getSizeCABACBinarizationParameters(Cabac_binarization_parametersType* cabac_binarizations,
                                          BinarizationIdEnum binarization_ID) {
    size_t sizeinBits = 0;

    if (binarization_ID == 1) {
        sizeinBits += 8;  // cmax
    } else if (binarization_ID == 4 || binarization_ID == 5) {
        sizeinBits += 8;  // cmax_teg
    } else if (binarization_ID == 8 || binarization_ID == 9) {
        sizeinBits += 8;  // cmax_dtu
    }
    if (binarization_ID == 6 || binarization_ID == 7 || binarization_ID == 8 || binarization_ID == 9) {
        sizeinBits += 4;  // splitUnitSize
    }

    return sizeinBits;
}

size_t getSizeSupportValues(Support_valuesType* supportValues, TransformSubSymIdEnum transform_ID_subsym) {
    size_t sizeInBits = 0;
    sizeInBits += 6;  // output_symbol_size
    sizeInBits += 6;  // coding_symbol_size
    sizeInBits += 2;  // coding_order

    if (supportValues->coding_symbol_size < supportValues->output_symbol_size && supportValues->coding_order > 0) {
        if (transform_ID_subsym == SubSym_LUT_TRANSFORM) {
            sizeInBits += 1;  // share_subsym_lut_flag
        }
        sizeInBits += 1;  // share_subsym_prv_flag
    }
    return sizeInBits;
}

EncodingParametersRC getCorrectedTransform_subseq_counter(
    Transform_subseq_parametersType* transform_subseq_parametersType, uint8_t* transform_subseq_counter) {
    if (transform_subseq_parametersType == NULL || transform_subseq_counter == NULL) {
        return NULL_PTR;
    }

    if (transform_subseq_parametersType->transform_ID_subseq == SubSeq_EQUALITY_CODING) {
        *transform_subseq_counter = 2;
    } else if (transform_subseq_parametersType->transform_ID_subseq == SubSeq_MATCH_CODING) {
        *transform_subseq_counter = 3;
    } else if (transform_subseq_parametersType->transform_ID_subseq == SubSeq_RLE_CODING) {
        *transform_subseq_counter = 2;
    } else if (transform_subseq_parametersType->transform_ID_subseq == SubSeq_MERGE_CODING) {
        *transform_subseq_counter = transform_subseq_parametersType->merge_coding_subseq_count;
    } else {
        /*DEFAULT*/
        *transform_subseq_counter = 1;
    }

    return SUCCESS;
}

EncodingParametersRC getOutputSymbolSize(Support_valuesType* support_values, uint8_t* outputSymbolSize) {
    if (support_values == NULL || outputSymbolSize == NULL) {
        return NULL_PTR;
    }
    *outputSymbolSize = support_values->output_symbol_size;
    return SUCCESS;
}

EncodingParametersRC getCodingSymbolSize(Support_valuesType* support_values, uint8_t* codingSymbolSize) {
    if (support_values == NULL || codingSymbolSize == NULL) {
        return NULL_PTR;
    }
    *codingSymbolSize = support_values->coding_symbol_size;
    return SUCCESS;
}
EncodingParametersRC getCodingOrder(Support_valuesType* support_values, uint8_t* codingOrder) {
    if (support_values == NULL || codingOrder == NULL) {
        return NULL_PTR;
    }
    *codingOrder = support_values->coding_order;
    return SUCCESS;
}

EncodingParametersRC getShareSubsymLUTFlag(Support_valuesType* support_values, uint8_t transform_ID_subsym,
                                           bool* share_subsym_lut_flag) {
    if (support_values == NULL || share_subsym_lut_flag == NULL) {
        return NULL_PTR;
    }
    if (support_values->coding_symbol_size < support_values->output_symbol_size && support_values->coding_order &&
        transform_ID_subsym == SubSym_LUT_TRANSFORM) {
        *share_subsym_lut_flag = support_values->share_subsym_lut_flag;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getShareSubsymPRVFlag(Support_valuesType* support_values, bool* share_subsym_prv_flag) {
    if (support_values == NULL || share_subsym_prv_flag == NULL) {
        return NULL_PTR;
    }
    if (support_values->coding_symbol_size < support_values->output_symbol_size && support_values->coding_order) {
        *share_subsym_prv_flag = support_values->share_subsym_prv_flag;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getCTruncExpGolParam_BinarizationParameters(
    Cabac_binarization_parametersType* binarizationParameters, BinarizationIdEnum binarization_ID,
    uint8_t* cTruncExpGolParam) {
    if (binarizationParameters == NULL || cTruncExpGolParam == NULL) {
        return NULL_PTR;
    }
    if (binarization_ID == BinarizationID_TruncatedExponentialGolomb ||
        binarization_ID == BinarizationID_SignedTruncatedExponentialGolomb) {
        *cTruncExpGolParam = binarizationParameters->cmax_teg;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}
EncodingParametersRC getcMaxDTU_BinarizationParameters(Cabac_binarization_parametersType* binarizationParameters,
                                                       BinarizationIdEnum binarization_ID, uint8_t* cMaxDTU) {
    if (binarizationParameters == NULL || cMaxDTU == NULL) {
        return NULL_PTR;
    }
    if (binarization_ID == BinarizationID_DoubleTruncatedUnary ||
        binarization_ID == BinarizationID_SignedDoubleTruncatedUnary) {
        *cMaxDTU = binarizationParameters->cMaxDTU;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}
EncodingParametersRC getSplitUnitSize_BinarizationParameters(Cabac_binarization_parametersType* binarizationParameters,
                                                             BinarizationIdEnum binarization_ID,
                                                             uint8_t* splitUnitSize) {
    if (binarizationParameters == NULL || splitUnitSize == NULL) {
        return NULL_PTR;
    }
    if (binarization_ID == BinarizationID_SplitUnitWiseTruncatedUnary ||
        binarization_ID == BinarizationID_SignedSplitUnitWiseTruncatedUnary ||
        binarization_ID == BinarizationID_DoubleTruncatedUnary ||
        binarization_ID == BinarizationID_SignedDoubleTruncatedUnary) {
        *splitUnitSize = binarizationParameters->splitUnitSize;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getBinarizationId(Cabac_binarizationsType* cabac_binarizations, uint8_t* binarizationId) {
    if (cabac_binarizations == NULL || binarizationId == NULL) {
        return NULL_PTR;
    }
    *binarizationId = cabac_binarizations->binarization_ID;
    return SUCCESS;
}
EncodingParametersRC getBypassFlag(Cabac_binarizationsType* cabac_binarizations, bool* bypassFlag) {
    if (cabac_binarizations == NULL || bypassFlag == NULL) {
        return NULL_PTR;
    }
    *bypassFlag = cabac_binarizations->bypass_flag;
    return SUCCESS;
}

EncodingParametersRC getCTruncExpGolParam(Cabac_binarizationsType* cabac_binarizations, uint8_t* cTruncExpGolParam) {
    if (cabac_binarizations == NULL || cabac_binarizations->cabac_binarization_parameters == NULL ||
        cTruncExpGolParam == NULL) {
        return NULL_PTR;
    }
    return getCTruncExpGolParam_BinarizationParameters(cabac_binarizations->cabac_binarization_parameters,
                                                       cabac_binarizations->binarization_ID, cTruncExpGolParam);
}

EncodingParametersRC getcMax_BinarizationParameters(Cabac_binarization_parametersType* binarizationParameters,
                                                    BinarizationIdEnum binarization_ID, uint8_t* cMax) {
    if (binarizationParameters == NULL || cMax == NULL) {
        return NULL_PTR;
    }
    if (binarization_ID == BinarizationID_TruncatedUnary) {
        *cMax = binarizationParameters->cmax_teg;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getcMax(Cabac_binarizationsType* cabac_binarizations, uint8_t* cMax) {
    if (cabac_binarizations == NULL || cabac_binarizations->cabac_binarization_parameters == NULL || cMax == NULL) {
        return NULL_PTR;
    }
    return getcMax_BinarizationParameters(cabac_binarizations->cabac_binarization_parameters,
                                          cabac_binarizations->binarization_ID, cMax);
}

EncodingParametersRC getcMaxTeg_BinarizationParameters(Cabac_binarization_parametersType* binarizationParameters,
                                                       BinarizationIdEnum binarization_ID, uint8_t* cMaxTeg) {
    if (binarizationParameters == NULL || cMaxTeg == NULL) {
        return NULL_PTR;
    }
    if (binarization_ID == BinarizationID_TruncatedExponentialGolomb ||
        binarization_ID == BinarizationID_SignedTruncatedExponentialGolomb) {
        *cMaxTeg = binarizationParameters->cmax_teg;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getcMaxTeg(Cabac_binarizationsType* cabac_binarizations, uint8_t* cMaxTeg) {
    if (cabac_binarizations == NULL || cabac_binarizations->cabac_binarization_parameters == NULL || cMaxTeg == NULL) {
        return NULL_PTR;
    }
    return getcMaxTeg_BinarizationParameters(cabac_binarizations->cabac_binarization_parameters,
                                             cabac_binarizations->binarization_ID, cMaxTeg);
}

EncodingParametersRC getcMaxDTU(Cabac_binarizationsType* cabac_binarizations, uint8_t* cMaxDTU) {
    if (cabac_binarizations == NULL || cabac_binarizations->cabac_binarization_parameters == NULL || cMaxDTU == NULL) {
        return NULL_PTR;
    }
    return getcMaxDTU_BinarizationParameters(cabac_binarizations->cabac_binarization_parameters,
                                             cabac_binarizations->binarization_ID, cMaxDTU);
}
EncodingParametersRC getSplitUnitSize(Cabac_binarizationsType* cabac_binarizations, uint8_t* splitUnitSize) {
    if (cabac_binarizations == NULL || cabac_binarizations->cabac_binarization_parameters == NULL ||
        splitUnitSize == NULL) {
        return NULL_PTR;
    }
    return getSplitUnitSize_BinarizationParameters(cabac_binarizations->cabac_binarization_parameters,
                                                   cabac_binarizations->binarization_ID, splitUnitSize);
}
EncodingParametersRC getCabacContextParameters(Cabac_binarizationsType* cabac_binarizations,
                                               Cabac_context_parametersType** cabac_context_parameters) {
    if (cabac_binarizations == NULL || cabac_context_parameters == NULL) {
        return NULL_PTR;
    }
    if (cabac_binarizations->bypass_flag) {
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    *cabac_context_parameters = cabac_binarizations->cabac_context_parameters;
    return SUCCESS;
}
EncodingParametersRC getCabacBinarizationParameters(
    Cabac_binarizationsType* cabac_binarizations,
    Cabac_binarization_parametersType** cabac_binarization_parametersType) {
    if (cabac_binarizations == NULL) {
        return NULL_PTR;
    }
    *cabac_binarization_parametersType = cabac_binarizations->cabac_binarization_parameters;
    return SUCCESS;
}

EncodingParametersRC getDatasetTypeParameters(Encoding_ParametersType* encodingParameters, uint8_t* datasetType) {
    if (encodingParameters == NULL || datasetType == NULL) {
        return NULL_PTR;
    }
    *datasetType = encodingParameters->dataset_type;
    return SUCCESS;
}
EncodingParametersRC getAlphabetID(Encoding_ParametersType* encodingParameters, uint8_t* alphabetId) {
    if (encodingParameters == NULL || alphabetId == NULL) {
        return NULL_PTR;
    }
    *alphabetId = encodingParameters->alphabet_ID;
    return SUCCESS;
}
EncodingParametersRC getReadsLength(Encoding_ParametersType* encodingParameters, uint32_t* readsLength) {
    if (encodingParameters == NULL || readsLength == NULL) {
        return NULL_PTR;
    }
    *readsLength = encodingParameters->read_length;
    return SUCCESS;
}
EncodingParametersRC getNumberOfTemplateSegmentsMinus1(Encoding_ParametersType* encodingParameters,
                                                       uint8_t* numberOfTemplateSegmentsMinus1) {
    if (encodingParameters == NULL || numberOfTemplateSegmentsMinus1 == NULL) {
        return NULL_PTR;
    }
    *numberOfTemplateSegmentsMinus1 = encodingParameters->number_of_template_segments_minus1;
    return SUCCESS;
}
EncodingParametersRC getMaxAUDataUnitSize(Encoding_ParametersType* encodingParameters, uint32_t* maxAUDataUnitSize) {
    if (encodingParameters == NULL || maxAUDataUnitSize == NULL) {
        return NULL_PTR;
    }
    *maxAUDataUnitSize = encodingParameters->max_au_data_unit_size;
    return SUCCESS;
}
EncodingParametersRC getPos40Bits(Encoding_ParametersType* encodingParameters, bool* pos40Bits) {
    if (encodingParameters == NULL || pos40Bits == NULL) {
        return NULL_PTR;
    }
    *pos40Bits = encodingParameters->pos_40_bits;
    return SUCCESS;
}
EncodingParametersRC getQVDepth(Encoding_ParametersType* encodingParameters, uint8_t* qv_depth) {
    if (encodingParameters == NULL || qv_depth == NULL) {
        return NULL_PTR;
    }
    *qv_depth = encodingParameters->qv_depth;
    return SUCCESS;
}
EncodingParametersRC getASDepth(Encoding_ParametersType* encodingParameters, uint8_t* as_depth) {
    if (encodingParameters == NULL || as_depth == NULL) {
        return NULL_PTR;
    }
    *as_depth = encodingParameters->as_depth;
    return SUCCESS;
}
EncodingParametersRC getNumClasses(Encoding_ParametersType* encodingParameters, uint8_t* numClasses) {
    if (encodingParameters == NULL || numClasses == NULL) {
        return NULL_PTR;
    }
    *numClasses = encodingParameters->num_classes;
    return SUCCESS;
}
EncodingParametersRC getClassIds(Encoding_ParametersType* encodingParameters, uint8_t* numClasses, uint8_t** classID) {
    if (encodingParameters == NULL || numClasses == NULL || classID == NULL) {
        return NULL_PTR;
    }
    *numClasses = encodingParameters->num_classes;
    *classID = encodingParameters->classID;
    return SUCCESS;
}

Support_valuesType* copySupportValues(Support_valuesType* supportValuesType) {
    Support_valuesType* result = malloc(sizeof(Support_valuesType));
    result->coding_order = supportValuesType->coding_order;
    result->coding_symbol_size = supportValuesType->coding_symbol_size;
    result->output_symbol_size = supportValuesType->output_symbol_size;
    result->share_subsym_lut_flag = supportValuesType->share_subsym_lut_flag;
    result->share_subsym_prv_flag = supportValuesType->share_subsym_prv_flag;
    return result;
}

Parameter_set_crpsType* copyParameterSetCrps(Parameter_set_crpsType* parameterSetCrps) {
    Parameter_set_crpsType* result = malloc(sizeof(Parameter_set_crpsType));
    result->cr_alg_ID = parameterSetCrps->cr_alg_ID;
    result->cr_buf_max_size = parameterSetCrps->cr_buf_max_size;
    result->cr_pad_size = parameterSetCrps->cr_pad_size;
    return result;
}

Cabac_binarization_parametersType* copyCabac_binarization_parametersType(Cabac_binarization_parametersType* input) {
    Cabac_binarization_parametersType* result = malloc(sizeof(Cabac_binarization_parametersType));
    result->cmax = input->cmax;
    result->cmax_teg = input->cmax_teg;
    result->cMaxDTU = input->cMaxDTU;
    result->splitUnitSize = input->splitUnitSize;
    return result;
}

Cabac_context_parametersType* copyCabac_context_parametersType(Cabac_context_parametersType* input) {
    Cabac_context_parametersType* result = malloc(sizeof(Cabac_context_parametersType));
    result->adaptive_mode_flag = input->adaptive_mode_flag;
    result->share_sub_sym_ctx_flag = input->share_sub_sym_ctx_flag;
    result->num_contexts = input->num_contexts;
    if (result->num_contexts == 0) {
        result->context_initialization_value = NULL;
    } else {
        result->context_initialization_value = malloc(sizeof(uint8_t) * result->num_contexts);
        memccpy(result->context_initialization_value, input->context_initialization_value, result->num_contexts,
                sizeof(uint8_t));
    }
    return result;
}

Cabac_binarizationsType* copyCabacBinarizations(Cabac_binarizationsType* input) {
    Cabac_binarizationsType* result = malloc(sizeof(Cabac_binarizationsType));
    result->binarization_ID = input->binarization_ID;
    result->bypass_flag = input->bypass_flag;
    result->cabac_binarization_parameters = copyCabac_binarization_parametersType(input->cabac_binarization_parameters);
    result->cabac_context_parameters = copyCabac_context_parametersType(input->cabac_context_parameters);
    return result;
}

Transform_subseq_parametersType* copySubseqParameters(Transform_subseq_parametersType* input) {
    Transform_subseq_parametersType* result = malloc(sizeof(Transform_subseq_parametersType));
    result->match_coding_buffer_size = input->match_coding_buffer_size;
    result->merge_coding_subseq_count = input->merge_coding_subseq_count;
    result->rle_coding_guard = input->rle_coding_guard;
    result->transform_ID_subseq = input->transform_ID_subseq;
    result->merge_coding_shift_size = malloc(sizeof(uint8_t) * input->merge_coding_subseq_count);
    if (input->merge_coding_shift_size != NULL) {
        memccpy(result->merge_coding_shift_size, input->merge_coding_shift_size, input->merge_coding_subseq_count,
                sizeof(uint8_t));
    }
    return result;
}

DecoderConfigurationTypeCABAC* copyDecoderConfigurationTypeCABAC(DecoderConfigurationTypeCABAC* input) {
    DecoderConfigurationTypeCABAC* result = malloc(sizeof(DecoderConfigurationTypeCABAC));
    result->num_descriptor_subsequence_cfgs_minus1 = input->num_descriptor_subsequence_cfgs_minus1;
    result->descriptor_subsequence_ID = malloc(sizeof(uint16_t) * (input->num_descriptor_subsequence_cfgs_minus1 + 1));
    result->transform_subseq_parameters =
        malloc(sizeof(Transform_subseq_parametersType*) * (input->num_descriptor_subsequence_cfgs_minus1 + 1));
    result->transform_id_subsym =
        malloc(sizeof(TransformSubSymIdEnum*) * (input->num_descriptor_subsequence_cfgs_minus1 + 1));
    result->support_values = malloc(sizeof(Support_valuesType**) * (input->num_descriptor_subsequence_cfgs_minus1 + 1));
    result->cabac_binarizations =
        malloc(sizeof(Cabac_binarizationsType**) * (input->num_descriptor_subsequence_cfgs_minus1 + 1));

    for (uint8_t subseq_i = 0; subseq_i < (input->num_descriptor_subsequence_cfgs_minus1 + 1); subseq_i++) {
        result->descriptor_subsequence_ID[subseq_i] = input->descriptor_subsequence_ID[subseq_i];
        result->transform_subseq_parameters[subseq_i] =
            copySubseqParameters(input->transform_subseq_parameters[subseq_i]);

        uint8_t transformSubseqCounter;
        getTransformSubseqCounter(input, subseq_i, &transformSubseqCounter);

        result->transform_id_subsym[subseq_i] =
            (TransformSubSymIdEnum*)malloc(sizeof(TransformSubSymIdEnum) * transformSubseqCounter);
        result->support_values[subseq_i] =
            (Support_valuesType**)malloc(sizeof(Support_valuesType*) * transformSubseqCounter);
        result->cabac_binarizations[subseq_i] =
            (Cabac_binarizationsType**)malloc(sizeof(Cabac_binarizationsType*) * transformSubseqCounter);

        for (uint8_t j = 0; j < transformSubseqCounter; j++) {
            result->transform_id_subsym[subseq_i][j] = input->transform_id_subsym[subseq_i][j];
            result->support_values[subseq_i][j] = copySupportValues(input->support_values[subseq_i][j]);
            result->cabac_binarizations[subseq_i][j] = copyCabacBinarizations(input->cabac_binarizations[subseq_i][j]);
        }
    }
    return result;
}

Decoder_configuration_tokentype_cabac* copyDecoderConfigurationTokenTypeCABAC(
    Decoder_configuration_tokentype_cabac* input) {
    Decoder_configuration_tokentype_cabac* result = malloc(sizeof(Decoder_configuration_tokentype_cabac));
    if (result == NULL) {
        return NULL;
    }

    result->transform_subseq_parametersType = input->transform_subseq_parametersType;

    uint8_t transformSubseqCounter;
    getTransformSubseqCounterTokenType(result, &transformSubseqCounter);

    result->transformSubSym = (TransformSubSymIdEnum*)malloc(sizeof(TransformSubSymIdEnum) * transformSubseqCounter);
    result->support_values = (Support_valuesType**)malloc(sizeof(Support_valuesType*) * transformSubseqCounter);
    result->cabac_binarizations =
        (Cabac_binarizationsType**)malloc(sizeof(Cabac_binarizationsType*) * transformSubseqCounter);

    for (uint8_t j = 0; j < transformSubseqCounter; j++) {
        result->transformSubSym[j] = input->transformSubSym[j];
        result->support_values[j] = copySupportValues(input->support_values[j]);
        result->cabac_binarizations[j] = copyCabacBinarizations(input->cabac_binarizations[j]);
    }

    return result;
}

Decoder_configuration_tokentype* copyDecoder_configuration_tokentypeCABAC(Decoder_configuration_tokentype* input) {
    Decoder_configuration_tokentype* decoderConfigurationTokentype = malloc(sizeof(Decoder_configuration_tokentype));
    decoderConfigurationTokentype->rle_guard_tokentype = input->rle_guard_tokentype;
    decoderConfigurationTokentype->decoder_configuration_tokentype_cabac0 =
        copyDecoderConfigurationTokenTypeCABAC(input->decoder_configuration_tokentype_cabac0);
    decoderConfigurationTokentype->decoder_configuration_tokentype_cabac1 =
        copyDecoderConfigurationTokenTypeCABAC(input->decoder_configuration_tokentype_cabac1);
    return decoderConfigurationTokentype;
}

EncodingParametersRC getCABACDecoderConfiguration(Encoding_ParametersType* encodingParameters, uint8_t classIndex,
                                                  uint8_t descriptorID,
                                                  DecoderConfigurationTypeCABAC** decoder_configuration_cabac) {
    if (encodingParameters == NULL) {
        return NULL_PTR;
    }
    if (classIndex >= encodingParameters->num_classes) {
        return OUT_OF_BOUNDERIES;
    }
    if (descriptorID >= 18 || descriptorID == 11 || descriptorID == 15) {
        return OUT_OF_BOUNDERIES;
    }

    if (!encodingParameters->class_specific_dec_cfg_flag[descriptorID]) {
        classIndex = 0;
    }

    *decoder_configuration_cabac = encodingParameters->decoderConfiguration[descriptorID][classIndex];
    return SUCCESS;
}
EncodingParametersRC getCABACDecoderConfigurationTokenType(
    Encoding_ParametersType* encodingParameters, uint8_t classIndex, uint8_t descriptorID,
    Decoder_configuration_tokentype** decoderConfigurationTokentypeCABAC) {
    if (encodingParameters == NULL || decoderConfigurationTokentypeCABAC == NULL) {
        return NULL_PTR;
    }
    if (classIndex >= encodingParameters->num_classes) {
        return OUT_OF_BOUNDERIES;
    }
    if (descriptorID == 11 || descriptorID == 15) {
        *decoderConfigurationTokentypeCABAC = encodingParameters->decoderConfiguration[classIndex][descriptorID];
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getNumGroups(Encoding_ParametersType* encodingParameters, uint16_t* numGroups) {
    if (encodingParameters == NULL || numGroups == NULL) {
        return NULL_PTR;
    }
    *numGroups = encodingParameters->num_groups;
    return SUCCESS;
}
EncodingParametersRC getReadGroupId(Encoding_ParametersType* encodingParameters, uint16_t groupIndex, char** rgroupID) {
    if (encodingParameters == NULL || rgroupID == NULL) {
        return NULL_PTR;
    }
    if (groupIndex < encodingParameters->num_groups) {
        *rgroupID = encodingParameters->rgroup_ID[groupIndex];
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getSplicedReadsFlag(Encoding_ParametersType* encodingParameters, bool* splicedReadsFlag) {
    if (encodingParameters == NULL || splicedReadsFlag == NULL) {
        return NULL_PTR;
    }
    *splicedReadsFlag = encodingParameters->spliced_reads_flag;
    return SUCCESS;
}

EncodingParametersRC getMultipleSignatureBaseParameters(Encoding_ParametersType* encodingParameters,
                                                        uint32_t* multiple_signature_base) {
    if (encodingParameters == NULL || multiple_signature_base == NULL) {
        return NULL_PTR;
    }
    *multiple_signature_base = encodingParameters->multiple_signature_base;
    return SUCCESS;
}
EncodingParametersRC getSignatureSize(Encoding_ParametersType* encodingParameters, uint8_t* u_signature_size) {
    if (encodingParameters == NULL || u_signature_size == NULL) {
        return NULL_PTR;
    }
    *u_signature_size = encodingParameters->U_signature_size;
    return SUCCESS;
}
EncodingParametersRC getMultipleAlignments_flag(Encoding_ParametersType* encodingParameters,
                                                bool* multiple_alignments_flag) {
    if (encodingParameters == NULL || multiple_alignments_flag == NULL) {
        return NULL_PTR;
    }
    *multiple_alignments_flag = encodingParameters->multiple_alignments_flag;
    return SUCCESS;
}
EncodingParametersRC getQVCodingMode(Encoding_ParametersType* encodingParameters, uint8_t classIndex,
                                     uint8_t* qv_coding_mode) {
    if (encodingParameters == NULL || qv_coding_mode == NULL) {
        return NULL_PTR;
    }
    if (classIndex < encodingParameters->num_classes) {
        *qv_coding_mode = encodingParameters->qv_coding_mode[classIndex];
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getQVPSflag(Encoding_ParametersType* encodingParameters, uint8_t classIndex, bool* qvps_flag) {
    if (encodingParameters == NULL || qvps_flag == NULL) {
        return NULL_PTR;
    }
    if (classIndex < encodingParameters->num_classes) {
        if (encodingParameters->qv_coding_mode[classIndex] == 1) {
            *qvps_flag = encodingParameters->qvps_flag[classIndex];
            return SUCCESS;
        }
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getParameterSetQvps(Encoding_ParametersType* encodingParameters, uint8_t classIndex,
                                         Parameter_set_qvpsType** pParameter_set_qvpsType) {
    if (encodingParameters == NULL || pParameter_set_qvpsType == NULL) {
        return NULL_PTR;
    }
    if (classIndex < encodingParameters->num_classes) {
        if (encodingParameters->qv_coding_mode[classIndex] == 1) {
            if (encodingParameters->qvps_flag[classIndex]) {
                *pParameter_set_qvpsType = encodingParameters->parameter_set_qvps[classIndex];
                return SUCCESS;
            }
            return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
        }
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getQvps_preset_id(Encoding_ParametersType* encodingParameters, uint8_t classIndex,
                                       uint8_t* qvps_preset_ID) {
    if (encodingParameters == NULL || qvps_preset_ID == NULL) {
        return NULL_PTR;
    }
    if (classIndex < encodingParameters->num_classes) {
        if (encodingParameters->qv_coding_mode[classIndex] == 1) {
            if (encodingParameters->qvps_flag[classIndex]) {
                *qvps_preset_ID = encodingParameters->qvps_preset_ID[classIndex];
                return SUCCESS;
            }
            return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
        }
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getCrpsFlag(Encoding_ParametersType* encodingParameters, bool* crps_flag) {
    if (encodingParameters == NULL || crps_flag == NULL) {
        return NULL_PTR;
    }
    *crps_flag = encodingParameters->crps_flag;
    return SUCCESS;
}
EncodingParametersRC getParameterSetCrps(Encoding_ParametersType* encodingParameters,
                                         Parameter_set_crpsType** parameterSetCrps) {
    if (encodingParameters == NULL || parameterSetCrps == NULL) {
        return NULL_PTR;
    }
    if (encodingParameters->crps_flag) {
        *parameterSetCrps = encodingParameters->parameter_set_crps;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getNumDescriptorSubsequenceCfgsMinus1(DecoderConfigurationTypeCABAC* decoderConfigurationCABAC,
                                                           uint8_t* num_descriptor_subsequence_cfgs_minus1) {
    if (decoderConfigurationCABAC == NULL || num_descriptor_subsequence_cfgs_minus1 == NULL) {
        return NULL_PTR;
    }
    *num_descriptor_subsequence_cfgs_minus1 = decoderConfigurationCABAC->num_descriptor_subsequence_cfgs_minus1;
    return SUCCESS;
}

EncodingParametersRC getDescriptorSubsequenceId(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                                uint16_t desciptorSubsequence_i, uint16_t* desciptorSubsequenceId) {
    if (decoderConfigurationTypeCABAC == NULL || desciptorSubsequenceId == NULL) {
        return NULL_PTR;
    }
    if (desciptorSubsequence_i <= decoderConfigurationTypeCABAC->num_descriptor_subsequence_cfgs_minus1) {
        *desciptorSubsequenceId = decoderConfigurationTypeCABAC->descriptor_subsequence_ID[desciptorSubsequence_i];
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}
EncodingParametersRC getTransformSubseqParameters(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                                  uint16_t descriptorSubsequence_i,
                                                  Transform_subseq_parametersType** transform_subseq_parameter) {
    if (decoderConfigurationTypeCABAC == NULL || transform_subseq_parameter == NULL) {
        return NULL_PTR;
    }
    if (descriptorSubsequence_i <= decoderConfigurationTypeCABAC->num_descriptor_subsequence_cfgs_minus1) {
        *transform_subseq_parameter =
            decoderConfigurationTypeCABAC->transform_subseq_parameters[descriptorSubsequence_i];
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}

Decoder_configuration_tokentype_cabac* constructDecoder_configuration_tokentype_cabac(
    Transform_subseq_parametersType* transform_subseq_parametersType, TransformSubSymIdEnum* transformSubSym,
    Support_valuesType** support_values, Cabac_binarizationsType** cabac_binarizations) {
    Decoder_configuration_tokentype_cabac* result = malloc(sizeof(Decoder_configuration_tokentype_cabac));
    result->transform_subseq_parametersType = transform_subseq_parametersType;
    result->transformSubSym = transformSubSym;
    result->support_values = support_values;
    result->cabac_binarizations = cabac_binarizations;
    return result;
}

Decoder_configuration_tokentype_cabac* readDecoder_configuration_tokentype_cabac(InputBitstream* inputBitstream) {
    Decoder_configuration_tokentype_cabac* result = malloc(sizeof(Decoder_configuration_tokentype_cabac));
    if (result == NULL) {
        return NULL;
    }

    Transform_subseq_parametersType* transform_subseq_parametersType = parseTransformSubseqParameters(inputBitstream);
    result->transform_subseq_parametersType = transform_subseq_parametersType;

    uint8_t transformSubseqCounter;
    getTransformSubseqCounterTokenType(result, &transformSubseqCounter);

    result->transformSubSym = (TransformSubSymIdEnum*)malloc(sizeof(TransformSubSymIdEnum) * transformSubseqCounter);
    result->support_values = (Support_valuesType**)malloc(sizeof(Support_valuesType*) * transformSubseqCounter);
    result->cabac_binarizations =
        (Cabac_binarizationsType**)malloc(sizeof(Cabac_binarizationsType*) * transformSubseqCounter);

    for (uint8_t j = 0; j < transformSubseqCounter; j++) {
        uint8_t transform_ID_subsym;
        readNBits8(inputBitstream, 3, &transform_ID_subsym);
        switch (transform_ID_subsym) {
            case 0:
                result->transformSubSym[j] = SubSym_NO_TRANSFORM;
                break;
            case 1:
                result->transformSubSym[j] = SubSym_LUT_TRANSFORM;
                break;
            case 2:
                result->transformSubSym[j] = SubSym_DIFF_CODING;
                break;
            default:
                return NULL;
        }
        result->support_values[j] = readSupportValuesType(inputBitstream, result->transformSubSym[j]);
        result->cabac_binarizations[j] =
            readCabacBinarization(inputBitstream, result->support_values[j]->coding_symbol_size,
                                  result->support_values[j]->output_symbol_size);
    }

    return result;
}

EncodingParametersRC getTransformSubseqCounter(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                               uint16_t descriptorSubsequenceId, uint8_t* transformSubseqCounter) {
    if (decoderConfigurationTypeCABAC == NULL || transformSubseqCounter == NULL) {
        return NULL_PTR;
    }
    if (descriptorSubsequenceId <= decoderConfigurationTypeCABAC->num_descriptor_subsequence_cfgs_minus1) {
        getCorrectedTransform_subseq_counter(
            decoderConfigurationTypeCABAC->transform_subseq_parameters[descriptorSubsequenceId],
            transformSubseqCounter);
        return SUCCESS;
    }
    return OUT_OF_BOUNDERIES;
}

EncodingParametersRC getTransformSubseqCounterTokenType(
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac, uint8_t* transformSubseqCounter) {
    if (decoder_configuration_tokentype_cabac == NULL || transformSubseqCounter == NULL) {
        return NULL_PTR;
    }
    getCorrectedTransform_subseq_counter(decoder_configuration_tokentype_cabac->transform_subseq_parametersType,
                                         transformSubseqCounter);
    return SUCCESS;
}

EncodingParametersRC getTransformIdSubSym(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                          uint16_t descriptorSubsequenceId, uint16_t transformSubseqIndex,
                                          TransformSubSymIdEnum* transformIdSubSym) {
    if (decoderConfigurationTypeCABAC == NULL || transformIdSubSym == NULL) {
        return NULL_PTR;
    }
    uint8_t transformSubseqCounter;
    EncodingParametersRC result =
        getTransformSubseqCounter(decoderConfigurationTypeCABAC, descriptorSubsequenceId, &transformSubseqCounter);
    if (result == SUCCESS) {
        if (transformSubseqIndex < transformSubseqCounter) {
            *transformIdSubSym =
                decoderConfigurationTypeCABAC->transform_id_subsym[descriptorSubsequenceId][transformSubseqIndex];
            return SUCCESS;
        }
        return OUT_OF_BOUNDERIES;
    }
    return result;
}
EncodingParametersRC getTransformSupportValues(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                               uint16_t descriptorSubsequenceId, uint16_t transformSubseqIndex,
                                               Support_valuesType** supportValuesType) {
    if (decoderConfigurationTypeCABAC == NULL || supportValuesType == NULL) {
        return NULL_PTR;
    }
    uint8_t transformSubseqCounter;
    EncodingParametersRC result =
        getTransformSubseqCounter(decoderConfigurationTypeCABAC, descriptorSubsequenceId, &transformSubseqCounter);
    if (result == SUCCESS) {
        if (transformSubseqIndex < transformSubseqCounter) {
            *supportValuesType =
                decoderConfigurationTypeCABAC->support_values[descriptorSubsequenceId][transformSubseqIndex];
            return SUCCESS;
        }
        return OUT_OF_BOUNDERIES;
    }
    return result;
}
EncodingParametersRC getCABACBinarizations(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                           uint16_t descriptorSubsequenceId, uint16_t transformSubseqIndex,
                                           Cabac_binarizationsType** cabacBinarizations) {
    if (decoderConfigurationTypeCABAC == NULL || cabacBinarizations == NULL) {
        return NULL_PTR;
    }
    uint8_t transformSubseqCounter;
    EncodingParametersRC result =
        getTransformSubseqCounter(decoderConfigurationTypeCABAC, descriptorSubsequenceId, &transformSubseqCounter);
    if (result == SUCCESS) {
        if (transformSubseqIndex < transformSubseqCounter) {
            *cabacBinarizations =
                decoderConfigurationTypeCABAC->cabac_binarizations[descriptorSubsequenceId][transformSubseqIndex];
            return SUCCESS;
        }
        return OUT_OF_BOUNDERIES;
    }
    return result;
}

EncodingParametersRC getSupportValueOutputSymbolSize(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                                     uint16_t descriptorSubsequenceId, uint16_t transformSubseqIndex,
                                                     uint8_t* outputSymbolSize) {
    Support_valuesType* supportValues;
    EncodingParametersRC resultSupportValue = getTransformSupportValues(
        decoderConfigurationTypeCABAC, descriptorSubsequenceId, transformSubseqIndex, &supportValues);
    if (resultSupportValue == SUCCESS) {
        return getOutputSymbolSize(supportValues, outputSymbolSize);
    }
    return resultSupportValue;
}

EncodingParametersRC getSupportValueCodingSymbolSize(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                                     uint16_t descriptorSubsequenceId, uint16_t transformSubseqIndex,
                                                     uint8_t* codingSymbolSize) {
    Support_valuesType* supportValues;
    EncodingParametersRC resultSupportValue = getTransformSupportValues(
        decoderConfigurationTypeCABAC, descriptorSubsequenceId, transformSubseqIndex, &supportValues);
    if (resultSupportValue == SUCCESS) {
        return getCodingSymbolSize(supportValues, codingSymbolSize);
    }
    return resultSupportValue;
}

EncodingParametersRC getSupportValueCodingOrder(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                                uint16_t descriptorSubsequenceId, uint16_t transformSubseqIndex,
                                                uint8_t* codingOrder) {
    Support_valuesType* supportValues;
    EncodingParametersRC resultSupportValue = getTransformSupportValues(
        decoderConfigurationTypeCABAC, descriptorSubsequenceId, transformSubseqIndex, &supportValues);
    if (resultSupportValue == SUCCESS) {
        return getCodingOrder(supportValues, codingOrder);
    }
    return resultSupportValue;
}

EncodingParametersRC getSupportValueShareSubsymLUTFlag(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                                       uint16_t descriptorSubsequenceId, uint16_t transformSubseqIndex,
                                                       bool* share_subsym_lut_flag) {
    Support_valuesType* supportValues;
    EncodingParametersRC resultSupportValue = getTransformSupportValues(
        decoderConfigurationTypeCABAC, descriptorSubsequenceId, transformSubseqIndex, &supportValues);
    if (resultSupportValue == SUCCESS) {
        TransformSubSymIdEnum transformSubSymIdEnum;
        EncodingParametersRC resultTransformId = getTransformIdSubSym(
            decoderConfigurationTypeCABAC, descriptorSubsequenceId, transformSubseqIndex, &transformSubSymIdEnum);
        if (resultTransformId == SUCCESS) {
            return getShareSubsymLUTFlag(supportValues, transformSubSymIdEnum, share_subsym_lut_flag);
        }
        return resultTransformId;
    }
    return resultSupportValue;
}

EncodingParametersRC getSupportValueShareSubsymPRVFlag(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                                       uint16_t descriptorSubsequenceId, uint16_t transformSubseqIndex,
                                                       bool* share_subsym_prv_flag) {
    Support_valuesType* supportValues;
    EncodingParametersRC resultSupportValue = getTransformSupportValues(
        decoderConfigurationTypeCABAC, descriptorSubsequenceId, transformSubseqIndex, &supportValues);
    if (resultSupportValue == SUCCESS) {
        return getShareSubsymPRVFlag(supportValues, share_subsym_prv_flag);
    }
    return resultSupportValue;
}

DecoderConfigurationTypeCABAC* readDecoderConfigurationTypeCABAC(InputBitstream* inputBitstream) {
    uint8_t num_descriptor_subsequence_cfgs_minus1;
    uint16_t* descriptor_subsequence_ID;
    Transform_subseq_parametersType** transformSubseqParameters;
    TransformSubSymIdEnum** transformSubSymIdEnum;
    Support_valuesType*** supportValues;
    Cabac_binarizationsType*** cabacBinarizations;

    readNBits8(inputBitstream, 8, &num_descriptor_subsequence_cfgs_minus1);
    descriptor_subsequence_ID = (uint16_t*)malloc(sizeof(uint16_t) * (num_descriptor_subsequence_cfgs_minus1 + 1));
    transformSubseqParameters = (Transform_subseq_parametersType**)malloc(sizeof(Transform_subseq_parametersType*) *
                                                                          (num_descriptor_subsequence_cfgs_minus1 + 1));
    transformSubSymIdEnum =
        (TransformSubSymIdEnum**)malloc(sizeof(TransformSubSymIdEnum*) * (num_descriptor_subsequence_cfgs_minus1 + 1));
    supportValues =
        (Support_valuesType***)malloc(sizeof(Support_valuesType**) * (num_descriptor_subsequence_cfgs_minus1 + 1));
    cabacBinarizations = (Cabac_binarizationsType***)malloc(sizeof(Cabac_binarizationsType**) *
                                                            (num_descriptor_subsequence_cfgs_minus1 + 1));

    for (uint8_t subsequence_i = 0; subsequence_i <= num_descriptor_subsequence_cfgs_minus1; subsequence_i++) {
        readNBitsBigToNativeEndian16(inputBitstream, 10, &(descriptor_subsequence_ID[subsequence_i]));
        Transform_subseq_parametersType* transform_subseq_parametersType =
            parseTransformSubseqParameters(inputBitstream);
        transformSubseqParameters[subsequence_i] = transform_subseq_parametersType;
        uint8_t transform_subseq_counter;
        getCorrectedTransform_subseq_counter(transform_subseq_parametersType, &transform_subseq_counter);

        transformSubSymIdEnum[subsequence_i] =
            (TransformSubSymIdEnum*)malloc(sizeof(TransformSubSymIdEnum) * (transform_subseq_counter));
        supportValues[subsequence_i] =
            (Support_valuesType**)malloc(sizeof(Support_valuesType*) * (transform_subseq_counter));
        cabacBinarizations[subsequence_i] =
            (Cabac_binarizationsType**)malloc(sizeof(Cabac_binarizationsType*) * (transform_subseq_counter));

        for (uint8_t transform_subseq_i = 0; transform_subseq_i < transform_subseq_counter; transform_subseq_i++) {
            uint8_t transform_ID_subsym;
            readNBits8(inputBitstream, 3, &transform_ID_subsym);
            transformSubSymIdEnum[subsequence_i][transform_subseq_i] = transform_ID_subsym;
            supportValues[subsequence_i][transform_subseq_i] =
                readSupportValuesType(inputBitstream, transform_ID_subsym);
            cabacBinarizations[subsequence_i][transform_subseq_i] = readCabacBinarization(
                inputBitstream, supportValues[subsequence_i][transform_subseq_i]->coding_symbol_size,
                supportValues[subsequence_i][transform_subseq_i]->output_symbol_size);
        }
    }
    return constructDecoderConfigurationCABAC(num_descriptor_subsequence_cfgs_minus1, descriptor_subsequence_ID,
                                              transformSubseqParameters, transformSubSymIdEnum, supportValues,
                                              cabacBinarizations);
}

EncodingParametersRC getCabacBinarizationsToken(
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac,
    Cabac_binarizationsType** cabacBinarizations) {
    if (decoder_configuration_tokentype_cabac == NULL || cabacBinarizations == NULL) {
        return NULL_PTR;
    }
    *cabacBinarizations = decoder_configuration_tokentype_cabac->cabac_binarizations[0];
    return SUCCESS;
}

EncodingParametersRC getRLEGuardTokenType(Decoder_configuration_tokentype* decoder_configuration_tokentype_typeCABAC,
                                          uint8_t* rle_guard_tokentype) {
    if (decoder_configuration_tokentype_typeCABAC == NULL || rle_guard_tokentype == NULL) {
        return NULL_PTR;
    }
    *rle_guard_tokentype = decoder_configuration_tokentype_typeCABAC->rle_guard_tokentype;
    return SUCCESS;
}
EncodingParametersRC getDecoderConfigurationTokentypeCabac_order0(
    Decoder_configuration_tokentype* decoder_configuration_tokentype_typeCABAC,
    Decoder_configuration_tokentype_cabac** decoder_configuration_tokentype_cabac) {
    if (decoder_configuration_tokentype_typeCABAC == NULL || decoder_configuration_tokentype_cabac == NULL) {
        return NULL_PTR;
    }
    *decoder_configuration_tokentype_cabac =
        decoder_configuration_tokentype_typeCABAC->decoder_configuration_tokentype_cabac0;
    return SUCCESS;
}
EncodingParametersRC getDecoderConfigurationTokentypeCabac_order1(
    Decoder_configuration_tokentype* decoder_configuration_tokentype_typeCABAC,
    Decoder_configuration_tokentype_cabac** decoder_configuration_tokentype_cabac) {
    if (decoder_configuration_tokentype_typeCABAC == NULL || decoder_configuration_tokentype_cabac == NULL) {
        return NULL_PTR;
    }
    *decoder_configuration_tokentype_cabac =
        decoder_configuration_tokentype_typeCABAC->decoder_configuration_tokentype_cabac1;
    return SUCCESS;
}
EncodingParametersRC getQVNumCodebooksTotal(Parameter_set_qvpsType* parameter_set_qvps,
                                            uint8_t* qv_num_codebooks_total) {
    if (parameter_set_qvps == NULL || qv_num_codebooks_total == NULL) {
        return NULL_PTR;
    }
    *qv_num_codebooks_total = parameter_set_qvps->qv_num_codebooks_total;
    return SUCCESS;
}
EncodingParametersRC getQVNumCodebooksEntries(Parameter_set_qvpsType* parameter_set_qvps, uint8_t qv_num_codebook_index,
                                              uint8_t* qv_num_codebook_entries, uint8_t** qv_recon) {
    if (parameter_set_qvps == NULL || qv_num_codebook_entries == NULL || qv_recon == NULL) {
        return NULL_PTR;
    }
    if (qv_num_codebook_index >= parameter_set_qvps->qv_num_codebooks_total) {
        return OUT_OF_BOUNDERIES;
    }
    *qv_num_codebook_entries = parameter_set_qvps->qv_num_codebook_entries[qv_num_codebook_index];
    *qv_recon = parameter_set_qvps->qv_recon[qv_num_codebook_index];
    return SUCCESS;
}

EncodingParametersRC getCrAlgId(Parameter_set_crpsType* parameterSetCrps, CrAlgEnum* cr_alg_ID) {
    if (parameterSetCrps == NULL || cr_alg_ID == NULL) {
        return NULL_PTR;
    }
    *cr_alg_ID = parameterSetCrps->cr_alg_ID;
    return SUCCESS;
}
EncodingParametersRC getCrPadSize(Parameter_set_crpsType* parameterSetCrps, uint8_t* cr_pad_size) {
    if (parameterSetCrps == NULL || cr_pad_size == NULL) {
        return NULL_PTR;
    }
    *cr_pad_size = parameterSetCrps->cr_pad_size;
    return SUCCESS;
}
EncodingParametersRC getCrBufMaxSize(Parameter_set_crpsType* parameterSetCrps, uint32_t* cr_buf_max_size) {
    if (parameterSetCrps == NULL || cr_buf_max_size == NULL) {
        return NULL_PTR;
    }
    *cr_buf_max_size = parameterSetCrps->cr_buf_max_size;
    return SUCCESS;
}
Parameter_set_crpsType* constructParameterSetCrps(CrAlgEnum cr_alg_id, uint8_t cr_pad_size, uint32_t cr_buf_max_size) {
    Parameter_set_crpsType* parameter_set_crps = (Parameter_set_crpsType*)malloc(sizeof(Parameter_set_crpsType));
    if (parameter_set_crps == NULL) {
        return NULL;
    }
    parameter_set_crps->cr_alg_ID = cr_alg_id;
    parameter_set_crps->cr_pad_size = cr_pad_size;
    parameter_set_crps->cr_buf_max_size = cr_buf_max_size;
    return parameter_set_crps;
}
void freeParameterSetCrps(Parameter_set_crpsType* parameterSetCrps) { free(parameterSetCrps); }

/**********************Write methods**********************************/
bool writeTransformSubseqParameters(Transform_subseq_parametersType* transform_subseq_parameters,
                                    OutputBitstream* outputBitstream) {
    bool transIdSubseqSuccWrite = true;
    bool matchCodingBufferSizeSuccWrite = true;
    bool rleCodingGuardSuccWrite = true;

    transIdSubseqSuccWrite =
        writeNBitsShift(outputBitstream, 8, (char*)&(transform_subseq_parameters->transform_ID_subseq));
    if (transform_subseq_parameters->transform_ID_subseq == SubSeq_EQUALITY_CODING) {
        /*Nothing to write*/
    } else if (transform_subseq_parameters->transform_ID_subseq == SubSeq_MATCH_CODING) {
        matchCodingBufferSizeSuccWrite = writeNBitsShiftAndConvertToBigEndian16(
            outputBitstream, 16, transform_subseq_parameters->match_coding_buffer_size);
    } else if (transform_subseq_parameters->transform_ID_subseq == SubSeq_RLE_CODING) {
        rleCodingGuardSuccWrite =
            writeNBitsShift(outputBitstream, 8, (char*)&(transform_subseq_parameters->rle_coding_guard));
    } else if (transform_subseq_parameters->transform_ID_subseq == SubSeq_MERGE_CODING) {
        writeNBitsShift(outputBitstream, 4, (char*)&(transform_subseq_parameters->merge_coding_subseq_count));
        for (uint8_t merge_coding_i = 0; merge_coding_i < transform_subseq_parameters->merge_coding_subseq_count;
             merge_coding_i++) {
            writeNBitsShift(outputBitstream, 5,
                            (char*)&(transform_subseq_parameters->merge_coding_shift_size[merge_coding_i]));
        }
    }
    return transIdSubseqSuccWrite && matchCodingBufferSizeSuccWrite && rleCodingGuardSuccWrite;
};

bool writeSupportValues(Support_valuesType* supportValues, OutputBitstream* outputBitstream,
                        TransformSubSymIdEnum transform_ID_subsym

) {
    if (!(writeNBitsShift(outputBitstream, 6, (char*)&(supportValues->output_symbol_size)) &&
          writeNBitsShift(outputBitstream, 6, (char*)&(supportValues->coding_symbol_size)) &&
          writeNBitsShift(outputBitstream, 2, (char*)&(supportValues->coding_order)))) {
        return false;
    }
    if (supportValues->coding_symbol_size < supportValues->output_symbol_size && supportValues->coding_order > 0) {
        if (transform_ID_subsym == SubSym_LUT_TRANSFORM) {
            if (!writeBit(outputBitstream, (uint8_t)(supportValues->share_subsym_lut_flag ? 1 : 0))) {
                return false;
            }
        }
        if (!writeBit(outputBitstream, (uint8_t)(supportValues->share_subsym_prv_flag ? 1 : 0))) {
            return false;
        }
    }
    return true;
}

bool writeCABACContextParameters(Cabac_context_parametersType* cabac_context_parameters,
                                 OutputBitstream* outputBitstream, uint8_t codingSubsymbolSize,
                                 uint8_t outputSubsymbolSize) {
    if (!(writeBit(outputBitstream, (uint8_t)(cabac_context_parameters->adaptive_mode_flag ? 1 : 0)) &&
          writeNBitsShiftAndConvertToBigEndian16(outputBitstream, 16, cabac_context_parameters->num_contexts))) {
        return false;
    }
    for (int i = 0; i < cabac_context_parameters->num_contexts; i++) {
        if (!writeNBitsShift(outputBitstream, 7, (char*)&(cabac_context_parameters->context_initialization_value[i]))) {
            return false;
        }
    }
    if (codingSubsymbolSize < outputSubsymbolSize) {
        if (!writeBit(outputBitstream, (uint8_t)(cabac_context_parameters->share_sub_sym_ctx_flag ? 1 : 0))) {
            return false;
        }
    }
    return true;
}

bool writeCABACBinarizationParameters(Cabac_binarization_parametersType* cabac_binarization_parameters,
                                      OutputBitstream* outputBitstream, BinarizationIdEnum binarization_ID) {
    bool cmaxSuccWrite = true;
    bool cmax_tegSuccWrite = true;
    bool cmax_dtuSuccWrite = true;
    bool splitUnitSizeSuccWrite = true;

    if (binarization_ID == 1) {
        cmaxSuccWrite = writeNBitsShift(outputBitstream, 8, (char*)&(cabac_binarization_parameters->cmax));
    } else if (binarization_ID == 4 || binarization_ID == 5) {
        cmax_tegSuccWrite = writeNBitsShift(outputBitstream, 8, (char*)&(cabac_binarization_parameters->cmax_teg));
    } else if (binarization_ID == 8 || binarization_ID == 9) {
        cmax_dtuSuccWrite = writeNBitsShift(outputBitstream, 8, (char*)&(cabac_binarization_parameters->cMaxDTU));
    }
    if (binarization_ID == 6 || binarization_ID == 7 || binarization_ID == 8 || binarization_ID == 9) {
        splitUnitSizeSuccWrite =
            writeNBitsShift(outputBitstream, 4, (char*)&(cabac_binarization_parameters->splitUnitSize));
    }

    return cmaxSuccWrite && cmax_tegSuccWrite && cmax_dtuSuccWrite && splitUnitSizeSuccWrite;
}

bool writeCABACBinarizations(Cabac_binarizationsType* cabac_binarizations, OutputBitstream* outputBitstream,
                             uint8_t codingSymbolSize, uint8_t outputSymbolSize) {
    uint8_t binarizationIdValue = (uint8_t)cabac_binarizations->binarization_ID;
    if (!(writeNBitsShift(outputBitstream, 5, (char*)&binarizationIdValue) &&
          writeBit(outputBitstream, (uint8_t)(cabac_binarizations->bypass_flag ? 1 : 0)) &&
          writeCABACBinarizationParameters(cabac_binarizations->cabac_binarization_parameters, outputBitstream,
                                           cabac_binarizations->binarization_ID))) {
        return false;
    }
    if (!cabac_binarizations->bypass_flag) {
        return writeCABACContextParameters(cabac_binarizations->cabac_context_parameters, outputBitstream,
                                           codingSymbolSize, outputSymbolSize);
    }
    return true;
}

bool writeDecoderConfigurationCABAC(DecoderConfigurationTypeCABAC* decoderConfigurationCABAC,
                                    OutputBitstream* outputBitstream) {
    if (!(writeNBitsShift(outputBitstream, 8,
                          (char*)&(decoderConfigurationCABAC->num_descriptor_subsequence_cfgs_minus1)))) {
        return false;
    }
    for (uint16_t i = 0; i <= decoderConfigurationCABAC->num_descriptor_subsequence_cfgs_minus1; i++) {
        if (!writeNBitsShiftAndConvertToBigEndian16(outputBitstream, 10,
                                                    decoderConfigurationCABAC->descriptor_subsequence_ID[i])) {
            return false;
        }
        if (!writeTransformSubseqParameters(decoderConfigurationCABAC->transform_subseq_parameters[i],
                                            outputBitstream)) {
            return false;
        }
        uint8_t transform_subseq_counter;
        if (getCorrectedTransform_subseq_counter(decoderConfigurationCABAC->transform_subseq_parameters[i],
                                                 &transform_subseq_counter) != SUCCESS) {
            return false;
        }

        for (int j = 0; j < transform_subseq_counter; j++) {
            uint8_t transformIdSubSym = decoderConfigurationCABAC->transform_id_subsym[i][j];
            if (!(writeNBitsShift(outputBitstream, 3, (char*)&transformIdSubSym) &&
                  writeSupportValues(decoderConfigurationCABAC->support_values[i][j], outputBitstream,
                                     decoderConfigurationCABAC->transform_id_subsym[i][j]) &&
                  writeCABACBinarizations(decoderConfigurationCABAC->cabac_binarizations[i][j], outputBitstream,
                                          decoderConfigurationCABAC->support_values[i][j]->coding_symbol_size,
                                          decoderConfigurationCABAC->support_values[i][j]->output_symbol_size))) {
                return false;
            }
        }
    }
    return true;
}

bool writeDecoderConfigurationTokentypeCabac(Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
                                             OutputBitstream* outputBitstream) {
    writeTransformSubseqParameters(decoderConfigurationTokentypeCabac->transform_subseq_parametersType,
                                   outputBitstream);
    uint8_t transformSubseqCounter;
    getTransformSubseqCounterTokenType(decoderConfigurationTokentypeCabac, &transformSubseqCounter);
    for (uint8_t transformSubseq_i = 0; transformSubseq_i < transformSubseqCounter; transformSubseq_i++) {
        writeNBitsShift(outputBitstream, 3,
                        (char*)&(decoderConfigurationTokentypeCabac->transformSubSym[transformSubseq_i]));
        writeSupportValues(decoderConfigurationTokentypeCabac->support_values[transformSubseq_i], outputBitstream,
                           decoderConfigurationTokentypeCabac->transformSubSym[transformSubseq_i]);
        writeCABACBinarizations(
            decoderConfigurationTokentypeCabac->cabac_binarizations[transformSubseq_i], outputBitstream,
            decoderConfigurationTokentypeCabac->support_values[transformSubseq_i]->coding_symbol_size,
            decoderConfigurationTokentypeCabac->support_values[transformSubseq_i]->output_symbol_size);
    }
    return true;
}

bool writeDecoder_configuration_tokentype_TypeCABAC(
    Decoder_configuration_tokentype* decoder_configuration_tokentype_typeCABAC, OutputBitstream* outputBitstream) {
    return writeNBitsShift(outputBitstream, 8,
                           (char*)&(decoder_configuration_tokentype_typeCABAC->rle_guard_tokentype)) &&
           writeDecoderConfigurationTokentypeCabac(
               decoder_configuration_tokentype_typeCABAC->decoder_configuration_tokentype_cabac0, outputBitstream) &&
           writeDecoderConfigurationTokentypeCabac(
               decoder_configuration_tokentype_typeCABAC->decoder_configuration_tokentype_cabac1, outputBitstream);
}

Decoder_configuration_tokentype* readDecoder_configuration_tokentypeCABAC(InputBitstream* inputBitstream) {
    uint8_t rle_guard;
    if (!readNBits8(inputBitstream, 8, &rle_guard)) {
        return NULL;
    }
    Decoder_configuration_tokentype_cabac* cabac0 = readDecoder_configuration_tokentype_cabac(inputBitstream);
    Decoder_configuration_tokentype_cabac* cabac1 = readDecoder_configuration_tokentype_cabac(inputBitstream);

    return constructDecoderConfigurationTokentype(rle_guard, cabac0, cabac1);
}

bool writeParameterSetQvps(Parameter_set_qvpsType* parameter_set_qvps, OutputBitstream* outputBitstream) {
    if (!writeNBitsShift(outputBitstream, 8, (char*)&(parameter_set_qvps->qv_num_codebooks_total))) {
        return false;
    }
    for (int i = 0; i < parameter_set_qvps->qv_num_codebooks_total; i++) {
        if (!writeNBitsShift(outputBitstream, 8, (char*)&(parameter_set_qvps->qv_num_codebook_entries[i]))) {
            return false;
        }
        for (int j = 0; j < parameter_set_qvps->qv_num_codebook_entries[i]; j++) {
            if (!writeNBitsShift(outputBitstream, 8, (char*)&(parameter_set_qvps->qv_recon[i][j]))) {
                return false;
            }
        }
    }
    return true;
}

bool writeParameterSetCrps(Parameter_set_crpsType* parameter_set_crps, OutputBitstream* outputBitstream) {
    if (!writeNBitsShift(outputBitstream, 8, (char*)&(parameter_set_crps->cr_alg_ID))) {
        return false;
    }
    if (parameter_set_crps->cr_alg_ID == CrAlg_PushIn || parameter_set_crps->cr_alg_ID == CrAlg_LocalAssembly) {
        return writeNBitsShift(outputBitstream, 8, (char*)&(parameter_set_crps->cr_pad_size)) &&
               writeNBitsShiftAndConvertToBigEndian32_new(outputBitstream, 24, parameter_set_crps->cr_buf_max_size);
    }
    return true;
}

bool writeEncoding_parameters(Encoding_ParametersType* encodingParameters, OutputBitstream* outputBitstream) {
    uint8_t reserved = 0;
    if (!(writeNBitsShift(outputBitstream, 4, (char*)&(encodingParameters->dataset_type)) &&
          writeNBitsShift(outputBitstream, 8, (char*)&(encodingParameters->alphabet_ID)) &&
          writeNBitsShiftAndConvertToBigEndian32_new(outputBitstream, 24, encodingParameters->read_length) &&
          writeNBitsShift(outputBitstream, 2, (char*)&(encodingParameters->number_of_template_segments_minus1)) &&
          writeNBitsShift(outputBitstream, 6, (char*)&reserved) &&
          writeNBitsShiftAndConvertToBigEndian32_new(outputBitstream, 29, encodingParameters->max_au_data_unit_size) &&
          writeBit(outputBitstream, (uint8_t)(encodingParameters->pos_40_bits ? 1 : 0)) &&
          writeNBitsShift(outputBitstream, 3, (char*)&(encodingParameters->qv_depth)) &&
          writeNBitsShift(outputBitstream, 3, (char*)&(encodingParameters->as_depth)) &&
          writeNBitsShift(outputBitstream, 4, (char*)&(encodingParameters->num_classes)))) {
        return false;
    }
    for (int j = 0; j < encodingParameters->num_classes; j++) {
        if (!writeNBitsShift(outputBitstream, 4, (char*)&(encodingParameters->classID[j]))) {
            return false;
        }
    }
    for (int desc_ID = 0; desc_ID < 18; desc_ID++) {
        if (!writeBit(outputBitstream, (uint8_t)(encodingParameters->class_specific_dec_cfg_flag[desc_ID]))) {
            return false;
        }
        int endLoop = encodingParameters->class_specific_dec_cfg_flag[desc_ID] ? encodingParameters->num_classes : 1;
        for (int c = 0; c < endLoop; c++) {
            writeNBitsShift(outputBitstream, 8, (char*)&(encodingParameters->dec_cfg_preset[desc_ID][c]));
            if (encodingParameters->encoding_mode_id[desc_ID][c] != 0) {
                // Only supports CABAC
                return false;
            }
            if (!writeNBitsShift(outputBitstream, 8, (char*)&(encodingParameters->encoding_mode_id[desc_ID][c]))) {
                return false;
            }
            if (desc_ID != 11 && desc_ID != 15) {
                if (!writeDecoderConfigurationCABAC(
                        (DecoderConfigurationTypeCABAC*)(encodingParameters->decoderConfiguration[desc_ID][c]),
                        outputBitstream)) {
                    return false;
                }
            } else {
                if (!writeDecoder_configuration_tokentype_TypeCABAC(
                        (Decoder_configuration_tokentype*)(encodingParameters->decoderConfiguration[desc_ID][c]),
                        outputBitstream)) {
                    return false;
                }
            }
        }
    }
    if (!writeNBitsShiftAndConvertToBigEndian16(outputBitstream, 16, encodingParameters->num_groups)) {
        return false;
    }
    for (int j = 0; j < encodingParameters->num_groups; j++) {
        size_t reagGroupId_len = strlen(encodingParameters->rgroup_ID[j]);
        if (!writeBytes(outputBitstream, reagGroupId_len, encodingParameters->rgroup_ID[j])) {
            return false;
        }
        if (!writeToBitstream(outputBitstream, 0)) {
            return false;
        }
    }
    if (!(writeBit(outputBitstream, (uint8_t)(encodingParameters->multiple_alignments_flag)) &&
          writeBit(outputBitstream, (uint8_t)(encodingParameters->spliced_reads_flag ? 1 : 0)) &&
          writeNBitsShiftAndConvertToBigEndian32_new(outputBitstream, 31,
                                                     encodingParameters->multiple_signature_base))) {
        return false;
    }

    if (encodingParameters->multiple_signature_base > 0) {
        if (!writeNBitsShift(outputBitstream, 6, (char*)&encodingParameters->U_signature_size)) {
            return false;
        }
    }

    for (int c = 0; c < encodingParameters->num_classes; c++) {
        if (!writeNBitsShift(outputBitstream, 4, (char*)&(encodingParameters->qv_coding_mode[c]))) {
            return false;
        }
        if (encodingParameters->qv_coding_mode[c] == 1) {
            if (!writeBit(outputBitstream, (uint8_t)(encodingParameters->qvps_flag[c] ? 1 : 0))) {
                return false;
            }
            if (encodingParameters->qvps_flag[c]) {
                if (!writeParameterSetQvps(encodingParameters->parameter_set_qvps[c], outputBitstream)) {
                    return false;
                }
            } else {
                if (!writeNBitsShift(outputBitstream, 4, (char*)&(encodingParameters->qvps_preset_ID[c]))) {
                    return false;
                }
            }
        }
        if (!writeBit(outputBitstream, (uint8_t)(encodingParameters->qv_reverse_flag[c] ? 1 : 0))) {
            return false;
        }
    }
    if (!writeBit(outputBitstream, (uint8_t)(encodingParameters->crps_flag ? 1 : 0))) {
        return false;
    }

    if (encodingParameters->crps_flag) {
        if (!writeParameterSetCrps(encodingParameters->parameter_set_crps, outputBitstream)) {
            return false;
        }
    }

    return writeBuffer(outputBitstream);
}

Transform_subseq_parametersType* constructTransformSubseqParameters(TransformSubSeqIdEnum transform_ID_subseq,
                                                                    uint16_t match_coding_buffer_size,
                                                                    uint8_t merge_coding_subseq_count,
                                                                    uint8_t* merge_coding_shift_size,
                                                                    uint8_t rle_coding_guard) {
    Transform_subseq_parametersType* result =
        (Transform_subseq_parametersType*)malloc(sizeof(Transform_subseq_parametersType));
    if (result == NULL) {
        return result;
    }
    result->transform_ID_subseq = transform_ID_subseq;
    result->match_coding_buffer_size = match_coding_buffer_size;
    result->rle_coding_guard = rle_coding_guard;
    result->merge_coding_subseq_count = merge_coding_subseq_count;
    result->merge_coding_shift_size = merge_coding_shift_size;
    return result;
}

Transform_subseq_parametersType* constructNoTransformSubseqParameters() {
    TransformSubSeqIdEnum transform_ID_subseq = SubSeq_NO_TRANSFORM;
    uint16_t match_coding_buffer_size = 0;
    uint8_t merge_coding_subseq_count = 0;
    uint8_t* merge_coding_shift_size = NULL;
    uint8_t rle_coding_guard = 0;
    return constructTransformSubseqParameters(transform_ID_subseq, match_coding_buffer_size, merge_coding_subseq_count,
                                              merge_coding_shift_size, rle_coding_guard);
}
Transform_subseq_parametersType* constructEqualityCodingTransformSubseqParameters() {
    TransformSubSeqIdEnum transform_ID_subseq = SubSeq_EQUALITY_CODING;
    uint16_t match_coding_buffer_size = 0;
    uint8_t merge_coding_subseq_count = 0;
    uint8_t* merge_coding_shift_size = NULL;
    uint8_t rle_coding_guard = 0;
    return constructTransformSubseqParameters(transform_ID_subseq, match_coding_buffer_size, merge_coding_subseq_count,
                                              merge_coding_shift_size, rle_coding_guard);
}
Transform_subseq_parametersType* constructMatchCodingSubseqTransformSubseqParameters(
    uint16_t match_coding_buffer_size) {
    TransformSubSeqIdEnum transform_ID_subseq = SubSeq_MATCH_CODING;
    uint8_t merge_coding_subseq_count = 0;
    uint8_t* merge_coding_shift_size = NULL;
    uint8_t rle_coding_guard = 0;
    return constructTransformSubseqParameters(transform_ID_subseq, match_coding_buffer_size, merge_coding_subseq_count,
                                              merge_coding_shift_size, rle_coding_guard);
}
Transform_subseq_parametersType* constructRLECodingSubseqTransformSubseqParameters(uint8_t rle_coding_guard) {
    TransformSubSeqIdEnum transform_ID_subseq = SubSeq_RLE_CODING;
    uint16_t match_coding_buffer_size = 0;
    uint8_t merge_coding_subseq_count = 0;
    uint8_t* merge_coding_shift_size = NULL;
    return constructTransformSubseqParameters(transform_ID_subseq, match_coding_buffer_size, merge_coding_subseq_count,
                                              merge_coding_shift_size, rle_coding_guard);
}
Transform_subseq_parametersType* constructMergeCodingSubseqTransformSubseqParameters(uint8_t merge_coding_subseq_count,
                                                                                     uint8_t* merge_coding_shift_size) {
    TransformSubSeqIdEnum transform_ID_subseq = SubSeq_MERGE_CODING;
    uint16_t match_coding_buffer_size = 0;
    uint8_t rle_coding_guard = 0;
    return constructTransformSubseqParameters(transform_ID_subseq, match_coding_buffer_size, merge_coding_subseq_count,
                                              merge_coding_shift_size, rle_coding_guard);
}
EncodingParametersRC getTransformSubSeqId(Transform_subseq_parametersType* transformSubseqParameters,
                                          TransformSubSeqIdEnum* output) {
    *output = transformSubseqParameters->transform_ID_subseq;
    return SUCCESS;
}
EncodingParametersRC getMatchCodingBufferSize(Transform_subseq_parametersType* transformSubseqParameters,
                                              uint16_t* output) {
    if (transformSubseqParameters->transform_ID_subseq != SubSeq_MATCH_CODING) {
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    *output = transformSubseqParameters->match_coding_buffer_size;
    return SUCCESS;
}
EncodingParametersRC getRLECodingGuard(Transform_subseq_parametersType* transformSubseqParameters, uint8_t* output) {
    if (transformSubseqParameters->transform_ID_subseq != SubSeq_RLE_CODING) {
        return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
    }
    *output = transformSubseqParameters->rle_coding_guard;
    return SUCCESS;
}

Transform_subseq_parametersType* parseTransformSubseqParameters(InputBitstream* input) {
    TransformSubSeqIdEnum transform_ID_subseq;
    uint16_t match_coding_buffer_size;
    uint8_t merge_coding_subseq_count = 0;
    uint8_t* merge_coding_shift_size = NULL;
    uint8_t rle_coding_guard;

    uint8_t buffer;

    if (!readNBits8(input, 8, &buffer)) {
        fprintf(stderr, "Error reading transform ID subseq\n");
        return NULL;
    }
    switch (buffer) {
        case SubSeq_NO_TRANSFORM:
            transform_ID_subseq = SubSeq_NO_TRANSFORM;
            break;
        case SubSeq_EQUALITY_CODING:
            transform_ID_subseq = SubSeq_EQUALITY_CODING;
            break;
        case SubSeq_MATCH_CODING:
            transform_ID_subseq = SubSeq_MATCH_CODING;
            readNBitsBigToNativeEndian16(input, 16, &match_coding_buffer_size);
            break;
        case SubSeq_RLE_CODING:
            transform_ID_subseq = SubSeq_RLE_CODING;
            readNBits8(input, 8, &rle_coding_guard);
            break;
        case SubSeq_MERGE_CODING:
            transform_ID_subseq = SubSeq_MERGE_CODING;
            readNBits8(input, 4, &merge_coding_subseq_count);
            merge_coding_shift_size = malloc(sizeof(uint8_t) * merge_coding_subseq_count);
            for (uint8_t i = 0; i < merge_coding_subseq_count; i++) {
                readNBits8(input, 5, merge_coding_shift_size + i);
            }
            break;
        default:
            fprintf(stderr, "Unknown transform id subseq\n");
            return NULL;
    }

    return constructTransformSubseqParameters(transform_ID_subseq, match_coding_buffer_size, merge_coding_subseq_count,
                                              merge_coding_shift_size, rle_coding_guard);
}
void freeTransformSubseqParameters(Transform_subseq_parametersType* parameters) {
    free(parameters->merge_coding_shift_size);
    free(parameters);
}
Support_valuesType* constructSupportValues(uint8_t output_symbol_size, uint8_t coding_symbol_size, uint8_t coding_order,
                                           bool share_subsym_lut_flag, bool share_subsym_prv_flag) {
    Support_valuesType* supportValues = (Support_valuesType*)malloc(sizeof(Support_valuesType));
    if (supportValues == NULL) {
        return NULL;
    }
    supportValues->output_symbol_size = output_symbol_size;
    supportValues->coding_symbol_size = coding_symbol_size;
    supportValues->coding_order = coding_order;
    supportValues->share_subsym_lut_flag = share_subsym_lut_flag;
    supportValues->share_subsym_prv_flag = share_subsym_prv_flag;
    return supportValues;
}

Cabac_binarization_parametersType* constructCABACBinarizationParameters(uint8_t cmax, uint8_t cmax_teg,
                                                                        uint8_t cmax_dtu, uint8_t splitUnitSize) {
    Cabac_binarization_parametersType* cabac_binarization_parameters =
        (Cabac_binarization_parametersType*)malloc((sizeof(Cabac_binarization_parametersType)));
    if (cabac_binarization_parameters == NULL) {
        return NULL;
    }
    cabac_binarization_parameters->cmax = cmax;
    cabac_binarization_parameters->cmax_teg = cmax_teg;
    cabac_binarization_parameters->cMaxDTU = cmax_dtu;
    cabac_binarization_parameters->splitUnitSize = splitUnitSize;
    return cabac_binarization_parameters;
}
// 0
Cabac_binarization_parametersType* constructCABACBinarizationParams_BinaryCoding() {
    uint8_t cmax = 0;
    uint8_t cmax_teg = 0;
    uint8_t cmax_dtu = 0;
    uint8_t splitUnitSize = 0;
    return constructCABACBinarizationParameters(cmax, cmax_teg, cmax_dtu, splitUnitSize);
}
// 1
Cabac_binarization_parametersType* constructCABACBinarizationParams_TruncatedUnary(uint8_t cmax) {
    uint8_t cmax_teg = 0;
    uint8_t cmax_dtu = 0;
    uint8_t splitUnitSize = 0;
    return constructCABACBinarizationParameters(cmax, cmax_teg, cmax_dtu, splitUnitSize);
}
// 2
Cabac_binarization_parametersType* constructCABACBinarizationParams_ExponentialGolomb() {
    uint8_t cmax = 0;
    uint8_t cmax_teg = 0;
    uint8_t cmax_dtu = 0;
    uint8_t splitUnitSize = 0;
    return constructCABACBinarizationParameters(cmax, cmax_teg, cmax_dtu, splitUnitSize);
}
// 3
Cabac_binarization_parametersType* constructCABACBinarizationParams_SignedExponentialGolomb() {
    uint8_t cmax = 0;
    uint8_t cmax_teg = 0;
    uint8_t cmax_dtu = 0;
    uint8_t splitUnitSize = 0;
    return constructCABACBinarizationParameters(cmax, cmax_teg, cmax_dtu, splitUnitSize);
}
// 4
Cabac_binarization_parametersType* constructCABACBinarizationParams_TruncatedExponentialGolomb(uint8_t cmax_teg) {
    uint8_t cmax = 0;
    uint8_t cmax_dtu = 0;
    uint8_t splitUnitSize = 0;
    return constructCABACBinarizationParameters(cmax, cmax_teg, cmax_dtu, splitUnitSize);
}
// 5
Cabac_binarization_parametersType* constructCABACBinarizationParams_SignedTruncatedExponentialGolomb(uint8_t cmax_teg) {
    uint8_t cmax = 0;
    uint8_t cmax_dtu = 0;
    uint8_t splitUnitSize = 0;
    return constructCABACBinarizationParameters(cmax, cmax_teg, cmax_dtu, splitUnitSize);
}
// 6
Cabac_binarization_parametersType* constructCABACBinarizationParams_SplitUnitWiseTruncatedUnary(uint8_t splitUnitSize) {
    uint8_t cmax = 0;
    uint8_t cmax_teg = 0;
    uint8_t cmax_dtu = 0;
    return constructCABACBinarizationParameters(cmax, cmax_teg, cmax_dtu, splitUnitSize);
}
// 7
Cabac_binarization_parametersType* construtCABACBinarizationParams_SignedSplitUnitWiseTruncatedUnary(
    uint8_t splitUnitSize) {
    uint8_t cmax = 0;
    uint8_t cmax_teg = 0;
    uint8_t cmax_dtu = 0;
    return constructCABACBinarizationParameters(cmax, cmax_teg, cmax_dtu, splitUnitSize);
}
// 8
Cabac_binarization_parametersType* construtCABACBinarizationParams_DoubleTruncatedUnary(uint8_t cmax_dtu,
                                                                                        uint8_t splitUnitSize) {
    uint8_t cmax = 0;
    uint8_t cmax_teg = 0;
    return constructCABACBinarizationParameters(cmax, cmax_teg, cmax_dtu, splitUnitSize);
}
// 9
Cabac_binarization_parametersType* construtCABACBinarizationParams_SignedDoubleTruncatedUnary(uint8_t cmax_dtu,
                                                                                              uint8_t splitUnitSize) {
    uint8_t cmax = 0;
    uint8_t cmax_teg = 0;
    return constructCABACBinarizationParameters(cmax, cmax_teg, cmax_dtu, splitUnitSize);
}

Cabac_context_parametersType* constructCABACContextParameters(bool adaptive_mode_flag, uint16_t num_contexts,
                                                              uint8_t* context_initialization_value,
                                                              bool share_sub_sym_ctx_flag) {
    Cabac_context_parametersType* result = (Cabac_context_parametersType*)malloc(sizeof(Cabac_context_parametersType));
    if (result == NULL) {
        return result;
    }
    result->adaptive_mode_flag = adaptive_mode_flag;
    result->num_contexts = num_contexts;
    result->context_initialization_value = context_initialization_value;
    result->share_sub_sym_ctx_flag = share_sub_sym_ctx_flag;
    return result;
}

Cabac_binarizationsType* readCabacBinarization(InputBitstream* input, uint8_t coding_subsym_size,
                                               uint8_t output_symbol_size) {
    uint8_t buffer;

    uint8_t binarization_ID;
    bool bypass_flag;
    Cabac_binarization_parametersType* cabacBinarizationParameters = NULL;
    Cabac_context_parametersType* cabacContextParameters = NULL;

    readNBits8(input, 5, &binarization_ID);
    readBit(input, &buffer);
    bypass_flag = buffer != 0;

    cabacBinarizationParameters = readCabacBinarizationParameters(input, binarization_ID);
    if (!bypass_flag) {
        cabacContextParameters = readCabacContextParameters(input, coding_subsym_size, output_symbol_size);
    }

    Cabac_binarizationsType* cabac_binarizations = malloc(sizeof(Cabac_binarizationsType));
    cabac_binarizations->binarization_ID = binarization_ID;
    cabac_binarizations->bypass_flag = bypass_flag;
    cabac_binarizations->cabac_binarization_parameters = cabacBinarizationParameters;
    cabac_binarizations->cabac_context_parameters = cabacContextParameters;

    return cabac_binarizations;
}

Cabac_context_parametersType* readCabacContextParameters(InputBitstream* input, uint8_t coding_subsym_size,
                                                         uint8_t output_subsym_size) {
    uint8_t buffer;

    bool adaptive_mode_flag;
    uint16_t num_contexts;
    uint8_t* context_initialization_value;
    bool share_sub_sym_ctx_flag;

    readBit(input, &buffer);
    adaptive_mode_flag = buffer != 0;
    readNBitsBigToNativeEndian16(input, 16, &num_contexts);
    context_initialization_value = malloc(sizeof(uint8_t) * num_contexts);
    for (uint16_t context_i = 0; context_i < num_contexts; context_i++) {
        readNBits8(input, 7, context_initialization_value + context_i);
    }
    if (coding_subsym_size < output_subsym_size) {
        readBit(input, &buffer);
        share_sub_sym_ctx_flag = buffer != 0;
    }

    return constructCABACContextParameters(adaptive_mode_flag, num_contexts, context_initialization_value,
                                           share_sub_sym_ctx_flag);
}

Cabac_binarization_parametersType* readCabacBinarizationParameters(InputBitstream* input, uint8_t binarization_ID) {
    uint8_t cmax = 0;
    uint8_t cmax_teg = 0;
    uint8_t cMaxDTU = 0;
    uint8_t splitUnitSize = 0;

    if (binarization_ID == 1) {
        readNBits8(input, 8, &cmax);
    } else if (binarization_ID == 4 || binarization_ID == 5) {
        readNBits8(input, 8, &cmax_teg);
    } else if (binarization_ID == 8 || binarization_ID == 9) {
        readNBits8(input, 8, &cMaxDTU);
    }
    if (binarization_ID == 6 || binarization_ID == 7 || binarization_ID == 8 || binarization_ID == 9) {
        readNBits8(input, 4, &splitUnitSize);
    }

    return constructCABACBinarizationParameters(cmax, cmax_teg, cMaxDTU, splitUnitSize);
}

void freeCabac_binarization(Cabac_binarizationsType* cabac_binarization) {
    if (cabac_binarization == NULL) {
        return;
    }
    freeCabacContextParameters(cabac_binarization->cabac_context_parameters);
    freeCabac_binarization_parameters(cabac_binarization->cabac_binarization_parameters);
    free(cabac_binarization);
}

void freeCabac_binarization_parameters(Cabac_binarization_parametersType* parameters) {
    if (parameters == NULL) {
        return;
    }
    free(parameters);
}

void freeCabacContextParameters(Cabac_context_parametersType* parameters) {
    if (parameters == NULL) {
        return;
    }
    free(parameters->context_initialization_value);
    free(parameters);
}

void freeDecoderConfigurationTypeCABAC(DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
                                       uint16_t numTransformSubsym, uint16_t numSupportValues) {
    for (int descriptor_i = 0; descriptor_i < numTransformSubsym; descriptor_i++) {
        free(decoderConfigurationTypeCABAC->transform_id_subsym[descriptor_i]);
    }
    free(decoderConfigurationTypeCABAC->transform_id_subsym);

    for (int descriptor_i = 0; descriptor_i < numSupportValues; descriptor_i++) {
        free(decoderConfigurationTypeCABAC->support_values[descriptor_i]);
    }
    free(decoderConfigurationTypeCABAC->support_values);

    free(decoderConfigurationTypeCABAC);
}

Cabac_binarizationsType* constructCabacBinarizationBinaryCoding_NotBypass(bool adaptive_mode_flag,
                                                                          uint16_t num_contexts,
                                                                          uint8_t* context_initialization_value,
                                                                          bool share_sub_sym_ctx_flag) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_BinaryCoding;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_BinaryCoding();
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
        adaptive_mode_flag, num_contexts, context_initialization_value, share_sub_sym_ctx_flag);
    if (cabac_binarizations->cabac_context_parameters == NULL) {
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
release_cabac_binarization_parameters:
    free(cabac_binarizations->cabac_binarization_parameters);
release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationBinaryCoding_Bypass() {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_BinaryCoding;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_BinaryCoding();
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationTruncatedUnary_NotBypass(uint8_t cmax, bool adaptive_mode_flag,
                                                                            uint16_t num_contexts,
                                                                            uint8_t* context_initialization_value,
                                                                            bool share_sub_sym_ctx_flag) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_TruncatedUnary;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_TruncatedUnary(cmax);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
        adaptive_mode_flag, num_contexts, context_initialization_value, share_sub_sym_ctx_flag);
    if (cabac_binarizations->cabac_context_parameters == NULL) {
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
release_cabac_binarization_parameters:
    free(cabac_binarizations->cabac_binarization_parameters);
release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationTruncatedUnary_Bypass(uint8_t cmax) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_TruncatedUnary;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_TruncatedUnary(cmax);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationExponentialGolomb_NotBypass(bool adaptive_mode_flag,
                                                                               uint16_t num_contexts,
                                                                               uint8_t* context_initialization_value,
                                                                               bool share_sub_sym_ctx_flag) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_ExponentialGolomb;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_ExponentialGolomb();
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
        adaptive_mode_flag, num_contexts, context_initialization_value, share_sub_sym_ctx_flag);
    if (cabac_binarizations->cabac_context_parameters == NULL) {
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
release_cabac_binarization_parameters:
    free(cabac_binarizations->cabac_binarization_parameters);
release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationExponentialGolomb_Bypass() {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_ExponentialGolomb;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_ExponentialGolomb();
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedExponentialGolomb_NotBypass(
    bool adaptive_mode_flag, uint16_t num_contexts, uint8_t* context_initialization_value,
    bool share_sub_sym_ctx_flag) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedExponentialGolomb;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_SignedExponentialGolomb();
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
        adaptive_mode_flag, num_contexts, context_initialization_value, share_sub_sym_ctx_flag);
    if (cabac_binarizations->cabac_context_parameters == NULL) {
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
release_cabac_binarization_parameters:
    free(cabac_binarizations->cabac_binarization_parameters);
release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedExponentialGolomb_Bypass() {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedExponentialGolomb;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters = constructCABACBinarizationParams_SignedExponentialGolomb();
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationTruncatedExponentialGolomb_NotBypass(
    uint8_t cTruncExpGolParam, bool adaptive_mode_flag, uint16_t num_contexts, uint8_t* context_initialization_value,
    bool share_sub_sym_ctx_flag) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_TruncatedExponentialGolomb;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
        constructCABACBinarizationParams_TruncatedExponentialGolomb(cTruncExpGolParam);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
        adaptive_mode_flag, num_contexts, context_initialization_value, share_sub_sym_ctx_flag);
    if (cabac_binarizations->cabac_context_parameters == NULL) {
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
release_cabac_binarization_parameters:
    free(cabac_binarizations->cabac_binarization_parameters);
release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationTruncatedExponentialGolomb_Bypass(uint8_t cTruncExpGolParam) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_TruncatedExponentialGolomb;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
        constructCABACBinarizationParams_TruncatedExponentialGolomb(cTruncExpGolParam);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedTruncatedExponentialGolomb_NotBypass(
    uint8_t cTruncExpGolParam, bool adaptive_mode_flag, uint16_t num_contexts, uint8_t* context_initialization_value,
    bool share_sub_sym_ctx_flag) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedTruncatedExponentialGolomb;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
        constructCABACBinarizationParams_SignedTruncatedExponentialGolomb(cTruncExpGolParam);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
        adaptive_mode_flag, num_contexts, context_initialization_value, share_sub_sym_ctx_flag);
    if (cabac_binarizations->cabac_context_parameters == NULL) {
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
release_cabac_binarization_parameters:
    free(cabac_binarizations->cabac_binarization_parameters);
release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedTruncatedExponentialGolomb_Bypass(uint8_t cTruncExpGolParam) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedTruncatedExponentialGolomb;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
        constructCABACBinarizationParams_SignedTruncatedExponentialGolomb(cTruncExpGolParam);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSplitUnitwiseTruncatedUnary_NotBypass(
    uint8_t splitUnitSize, bool adaptive_mode_flag, uint16_t num_contexts, uint8_t* context_initialization_value,
    bool share_sub_sym_ctx_flag) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SplitUnitWiseTruncatedUnary;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
        constructCABACBinarizationParams_SplitUnitWiseTruncatedUnary(splitUnitSize);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
        adaptive_mode_flag, num_contexts, context_initialization_value, share_sub_sym_ctx_flag);
    if (cabac_binarizations->cabac_context_parameters == NULL) {
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
release_cabac_binarization_parameters:
    free(cabac_binarizations->cabac_binarization_parameters);
release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSplitUnitwiseTruncatedUnary_Bypass(uint8_t splitUnitSize) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SplitUnitWiseTruncatedUnary;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
        constructCABACBinarizationParams_SplitUnitWiseTruncatedUnary(splitUnitSize);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedSplitUnitwiseTruncatedUnary_NotBypass(
    uint8_t splitUnitSize, bool adaptive_mode_flag, uint16_t num_contexts, uint8_t* context_initialization_value,
    bool share_sub_sym_ctx_flag) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedSplitUnitWiseTruncatedUnary;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
        construtCABACBinarizationParams_SignedSplitUnitWiseTruncatedUnary(splitUnitSize);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
        adaptive_mode_flag, num_contexts, context_initialization_value, share_sub_sym_ctx_flag);
    if (cabac_binarizations->cabac_context_parameters == NULL) {
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
release_cabac_binarization_parameters:
    free(cabac_binarizations->cabac_binarization_parameters);
release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedSplitUnitwiseTruncatedUnary_Bypass(uint8_t splitUnitSize) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedSplitUnitWiseTruncatedUnary;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
        construtCABACBinarizationParams_SignedSplitUnitWiseTruncatedUnary(splitUnitSize);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationDoubleTruncatedUnary_NotBypass(
    uint8_t cMaxDTU, uint8_t splitUnitSize, bool adaptive_mode_flag, uint16_t num_contexts,
    uint8_t* context_initialization_value, bool share_sub_sym_ctx_flag) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_DoubleTruncatedUnary;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
        construtCABACBinarizationParams_DoubleTruncatedUnary(cMaxDTU, splitUnitSize);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
        adaptive_mode_flag, num_contexts, context_initialization_value, share_sub_sym_ctx_flag);
    if (cabac_binarizations->cabac_context_parameters == NULL) {
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
release_cabac_binarization_parameters:
    free(cabac_binarizations->cabac_binarization_parameters);
release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationDoubleTruncatedUnary_Bypass(uint8_t cMaxDTU, uint8_t splitUnitSize) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_DoubleTruncatedUnary;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
        construtCABACBinarizationParams_DoubleTruncatedUnary(cMaxDTU, splitUnitSize);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedDoubleTruncatedUnary_NotBypass(
    uint8_t cMaxDTU, uint8_t splitUnitSize, bool adaptive_mode_flag, uint16_t num_contexts,
    uint8_t* context_initialization_value, bool share_sub_sym_ctx_flag) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedDoubleTruncatedUnary;
    cabac_binarizations->bypass_flag = false;
    cabac_binarizations->cabac_binarization_parameters =
        construtCABACBinarizationParams_SignedDoubleTruncatedUnary(cMaxDTU, splitUnitSize);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = constructCABACContextParameters(
        adaptive_mode_flag, num_contexts, context_initialization_value, share_sub_sym_ctx_flag);
    if (cabac_binarizations->cabac_context_parameters == NULL) {
        goto release_cabac_binarization_parameters;
    }
    return cabac_binarizations;
release_cabac_binarization_parameters:
    free(cabac_binarizations->cabac_binarization_parameters);
release_parent:
    free(cabac_binarizations);
    return NULL;
}

Cabac_binarizationsType* constructCabacBinarizationSignedDoubleTruncatedUnary_Bypass(uint8_t cMaxDTU,
                                                                                     uint8_t splitUnitSize) {
    Cabac_binarizationsType* cabac_binarizations = (Cabac_binarizationsType*)malloc(sizeof(Cabac_binarizationsType));
    if (cabac_binarizations == NULL) {
        return NULL;
    }
    cabac_binarizations->binarization_ID = BinarizationID_SignedDoubleTruncatedUnary;
    cabac_binarizations->bypass_flag = true;
    cabac_binarizations->cabac_binarization_parameters =
        construtCABACBinarizationParams_SignedDoubleTruncatedUnary(cMaxDTU, splitUnitSize);
    if (cabac_binarizations->cabac_binarization_parameters == NULL) {
        goto release_parent;
    }
    cabac_binarizations->cabac_context_parameters = NULL;
    return cabac_binarizations;

release_parent:
    free(cabac_binarizations);
    return NULL;
}

Support_valuesType* readSupportValuesType(InputBitstream* input, uint8_t transform_ID_subsym) {
    uint8_t output_symbol_size;
    uint8_t coding_symbol_size;
    uint8_t coding_order;
    bool share_subsym_lut_flag = false;
    bool share_subsym_prv_flag = false;

    uint8_t buffer;

    readNBits8(input, 6, &output_symbol_size);
    readNBits8(input, 6, &coding_symbol_size);
    readNBits8(input, 2, &coding_order);
    if (coding_symbol_size < output_symbol_size && coding_order > 0) {
        if (transform_ID_subsym == 1) {
            readBit(input, &buffer);
            share_subsym_lut_flag = buffer != 0;
        }
        readBit(input, &buffer);
        share_subsym_prv_flag = buffer != 0;
    }

    return constructSupportValues(output_symbol_size, coding_symbol_size, coding_order, share_subsym_lut_flag,
                                  share_subsym_prv_flag);
}

DecoderConfigurationTypeCABAC* constructDecoderConfigurationCABAC(
    uint8_t num_descriptor_subsequence_cfgs_minus1, uint16_t* descriptor_subsequence_ID,
    Transform_subseq_parametersType** transform_subseq_parameters, TransformSubSymIdEnum** transform_id_subsym,
    Support_valuesType*** support_values, Cabac_binarizationsType*** cabac_binarizations) {
    DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC =
        (DecoderConfigurationTypeCABAC*)malloc(sizeof(DecoderConfigurationTypeCABAC));
    if (decoderConfigurationTypeCABAC == NULL) {
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

Decoder_configuration_tokentype* constructDecoderConfigurationTokentype(
    uint8_t rle_guard_tokentype, Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac0,
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac1) {
    if (decoder_configuration_tokentype_cabac0 == NULL || decoder_configuration_tokentype_cabac1 == NULL) {
        return NULL;
    }
    Decoder_configuration_tokentype* decoderConfigurationTokentypeTypeCABAC =
        (Decoder_configuration_tokentype*)malloc(sizeof(Decoder_configuration_tokentype));
    if (decoderConfigurationTokentypeTypeCABAC == NULL) {
        return NULL;
    }

    decoderConfigurationTokentypeTypeCABAC->rle_guard_tokentype = rle_guard_tokentype;
    decoderConfigurationTokentypeTypeCABAC->decoder_configuration_tokentype_cabac0 =
        decoder_configuration_tokentype_cabac0;
    decoderConfigurationTokentypeTypeCABAC->decoder_configuration_tokentype_cabac1 =
        decoder_configuration_tokentype_cabac1;

    return decoderConfigurationTokentypeTypeCABAC;
}

Parameter_set_qvpsType* constructParameterSetQvpsType(uint8_t qv_num_codebooks_total, uint8_t* qv_num_codebook_entries,
                                                      uint8_t** qv_recon) {
    Parameter_set_qvpsType* parameter_set_qvpsType = (Parameter_set_qvpsType*)malloc(sizeof(Parameter_set_qvpsType));
    if (parameter_set_qvpsType == NULL) {
        return NULL;
    }
    parameter_set_qvpsType->qv_num_codebooks_total = qv_num_codebooks_total;
    parameter_set_qvpsType->qv_num_codebook_entries = qv_num_codebook_entries;
    parameter_set_qvpsType->qv_recon = qv_recon;
    return parameter_set_qvpsType;
}

Parameter_set_crpsType* constructParameterSetCRPSNoComp() { return constructParameterSetCrps(CrAlg_NoComp, 0, 0); }

Parameter_set_crpsType* constructParameterSetRefTransform() {
    return constructParameterSetCrps(CrAlg_RefTransform, 0, 0);
}

Parameter_set_crpsType* constructParameterSetPushIn(uint8_t cr_pad_size, uint32_t cr_buf_max_size) {
    return constructParameterSetCrps(CrAlg_PushIn, cr_pad_size, cr_buf_max_size);
}

Parameter_set_crpsType* constructParameterSetLocalAssembly(uint8_t cr_pad_size, uint32_t cr_buf_max_size) {
    return constructParameterSetCrps(CrAlg_LocalAssembly, cr_pad_size, cr_buf_max_size);
}

Parameter_set_crpsType* constructParameterSetGlobalAssembly(uint8_t cr_pad_size, uint32_t cr_buf_max_size) {
    return constructParameterSetCrps(CrAlg_GlobalAssembly, cr_pad_size, cr_buf_max_size);
}

Encoding_ParametersType* constructEncodingParameters(
    uint8_t datasetType, uint8_t alphabetId, uint32_t read_length, uint8_t number_of_template_segments_minus1,
    uint32_t max_au_data_unit_size, bool pos40bits, uint8_t qv_depth, uint8_t as_depth, uint8_t numClasses,
    uint8_t* classID, uint16_t numGroups, char** rgroupId, bool multiple_alignments_flag, bool splicedReadsFlag,
    uint32_t multipleSignatureBase, uint8_t U_signature_size, uint8_t* qv_coding_mode, bool* qvps_flag,
    Parameter_set_qvpsType** parameter_set_qvps, uint8_t* qvps_preset_ID, bool* qv_reverse_flag, bool crps_flag,
    Parameter_set_crpsType* parameter_set_crps) {
    Encoding_ParametersType* encodingParametersType = (Encoding_ParametersType*)malloc(sizeof(Encoding_ParametersType));
    if (encodingParametersType == NULL) {
        return NULL;
    }

    encodingParametersType->dataset_type = datasetType;
    encodingParametersType->alphabet_ID = alphabetId;
    encodingParametersType->read_length = read_length;
    encodingParametersType->number_of_template_segments_minus1 = number_of_template_segments_minus1;
    encodingParametersType->max_au_data_unit_size = max_au_data_unit_size;
    encodingParametersType->pos_40_bits = pos40bits;
    encodingParametersType->qv_depth = qv_depth;
    encodingParametersType->as_depth = as_depth;
    encodingParametersType->num_classes = numClasses;
    encodingParametersType->classID = classID;
    encodingParametersType->dec_cfg_preset = (uint8_t**)malloc(sizeof(uint8_t*) * 18);
    for (uint8_t descriptor_i = 0; descriptor_i < 18; descriptor_i++) {
        encodingParametersType->dec_cfg_preset[descriptor_i] = (uint8_t*)calloc(numClasses, sizeof(uint8_t));
    }
    encodingParametersType->decoderConfiguration = (void***)malloc(sizeof(void**) * 18);
    for (uint8_t descriptor_i = 0; descriptor_i < 18; descriptor_i++) {
        encodingParametersType->decoderConfiguration[descriptor_i] = (void**)malloc(sizeof(void*) * numClasses);
    }
    encodingParametersType->encoding_mode_id = (uint8_t**)malloc(sizeof(uint8_t*) * 18);
    for (uint8_t descriptor_i = 0; descriptor_i < 18; descriptor_i++) {
        encodingParametersType->encoding_mode_id[descriptor_i] = (uint8_t*)malloc(sizeof(uint8_t) * numClasses);
    }
    encodingParametersType->num_groups = numGroups;
    encodingParametersType->rgroup_ID = rgroupId;
    encodingParametersType->multiple_alignments_flag = multiple_alignments_flag;
    encodingParametersType->spliced_reads_flag = splicedReadsFlag;
    encodingParametersType->multiple_signature_base = multipleSignatureBase;
    encodingParametersType->U_signature_size = U_signature_size;

    encodingParametersType->qv_coding_mode = qv_coding_mode;
    encodingParametersType->qvps_flag = qvps_flag;
    encodingParametersType->parameter_set_qvps = parameter_set_qvps;
    encodingParametersType->qvps_preset_ID = qvps_preset_ID;

    encodingParametersType->crps_flag = crps_flag;
    encodingParametersType->parameter_set_crps = parameter_set_crps;

    return encodingParametersType;
}

Encoding_ParametersType* constructEncodingParametersSingleAlignmentNoComputed(
    uint8_t datasetType, uint8_t alphabetId, uint32_t reads_length, uint8_t number_of_template_segments_minus1,
    uint32_t max_au_data_unit_size, bool pos40bits, uint8_t qv_depth, uint8_t as_depth, uint8_t numClasses,
    uint8_t* classID, uint16_t numGroups, char** rgroupId, bool splicedReadsFlag, bool multipleSignatureFlag,
    uint32_t multipleSignatureBase, uint8_t U_signature_size, uint8_t* qv_coding_mode, bool* qvps_flag,
    Parameter_set_qvpsType** parameter_set_qvps, uint8_t* qvps_preset_ID, bool* qv_reverse_flag) {
    return constructEncodingParameters(
        datasetType, alphabetId, reads_length, number_of_template_segments_minus1, max_au_data_unit_size, pos40bits,
        qv_depth, as_depth, numClasses, classID, numGroups, rgroupId, false, splicedReadsFlag, multipleSignatureBase,
        U_signature_size, qv_coding_mode, qvps_flag, parameter_set_qvps, qvps_preset_ID, qv_reverse_flag, false, NULL);
}

Encoding_ParametersType* constructEncodingParametersMultipleAlignmentNoComputed(
    uint8_t datasetType, uint8_t alphabetId, uint32_t reads_length, uint8_t number_of_template_segments_minus1,
    uint32_t max_au_data_unit_size, bool pos40bits, uint8_t qv_depth, uint8_t as_depth, uint8_t numClasses,
    uint8_t* classID, uint16_t numGroups, char** rgroupId, bool splicedReadsFlag, uint32_t multipleSignatureBase,
    uint8_t U_signature_size, uint8_t* qv_coding_mode, bool* qvps_flag, Parameter_set_qvpsType** parameter_set_qvps,
    uint8_t* qvps_preset_ID, bool* qv_reverse_flag) {
    return constructEncodingParameters(
        datasetType, alphabetId, reads_length, number_of_template_segments_minus1, max_au_data_unit_size, pos40bits,
        qv_depth, as_depth, numClasses, classID, numGroups, rgroupId, true, splicedReadsFlag, multipleSignatureBase,
        U_signature_size, qv_coding_mode, qvps_flag, parameter_set_qvps, qvps_preset_ID, qv_reverse_flag, false, NULL);
}

Encoding_ParametersType* constructEncodingParametersSingleAlignmentComputedRef(
    uint8_t datasetType, uint8_t alphabetId, uint32_t reads_length, uint8_t number_of_template_segments_minus1,
    uint32_t max_au_data_unit_size, bool pos40bits, uint8_t qv_depth, uint8_t as_depth, uint8_t numClasses,
    uint8_t* classID, uint16_t numGroups, char** rgroupId, bool splicedReadsFlag, uint32_t multipleSignatureBase,
    uint8_t U_signature_size, uint8_t* qv_coding_mode, bool* qvps_flag, Parameter_set_qvpsType** parameter_set_qvps,
    uint8_t* qvps_preset_ID, bool* qv_reverse_flag, Parameter_set_crpsType* parameter_set_crps) {
    return constructEncodingParameters(datasetType, alphabetId, reads_length, number_of_template_segments_minus1,
                                       max_au_data_unit_size, pos40bits, qv_depth, as_depth, numClasses, classID,
                                       numGroups, rgroupId, false, splicedReadsFlag, multipleSignatureBase,
                                       U_signature_size, qv_coding_mode, qvps_flag, parameter_set_qvps, qvps_preset_ID,
                                       qv_reverse_flag, true, parameter_set_crps);
}

Encoding_ParametersType* constructEncodingParametersMultipleAlignmentComputedRef(
    uint8_t datasetType, uint8_t alphabetId, uint32_t reads_length, uint8_t number_of_template_segments_minus1,
    uint32_t max_au_data_unit_size, bool pos40bits, uint8_t qv_depth, uint8_t as_depth, uint8_t numClasses,
    uint8_t* classID, uint16_t numGroups, char** rgroupId, bool splicedReadsFlag, uint32_t multipleSignatureBase,
    uint8_t U_signature_size, uint8_t* qv_coding_mode, bool* qvps_flag, Parameter_set_qvpsType** parameter_set_qvps,
    uint8_t* qvps_preset_ID, bool* qv_reverse_flag, Parameter_set_crpsType* parameter_set_crps) {
    return constructEncodingParameters(datasetType, alphabetId, reads_length, number_of_template_segments_minus1,
                                       max_au_data_unit_size, pos40bits, qv_depth, as_depth, numClasses, classID,
                                       numGroups, rgroupId, true, splicedReadsFlag, multipleSignatureBase,
                                       U_signature_size, qv_coding_mode, qvps_flag, parameter_set_qvps, qvps_preset_ID,
                                       qv_reverse_flag, true, parameter_set_crps);
}

EncodingParametersRC setNonClassSpecificDescriptorConfigurationAndEncodingMode(
    Encoding_ParametersType* encodingParameters, uint8_t descId, void* configuration, uint8_t encodingMode) {
    if (descId >= 18) {
        return OUT_OF_BOUNDERIES;
    }

    encodingParameters->class_specific_dec_cfg_flag[descId] = false;
    for (int c = 0; c < encodingParameters->num_classes; c++) {
        encodingParameters->decoderConfiguration[descId][c] = configuration;
        encodingParameters->encoding_mode_id[descId][c] = encodingMode;
    }
    return SUCCESS;
}

EncodingParametersRC setClassSpecificDescriptorConfigurationAndEncodingMode(Encoding_ParametersType* encodingParameters,
                                                                            uint8_t classId, uint8_t descId,
                                                                            void* configuration, uint8_t encodingMode) {
    if (descId >= 18 || classId >= encodingParameters->num_classes) {
        return OUT_OF_BOUNDERIES;
    }
    encodingParameters->class_specific_dec_cfg_flag[descId] = true;
    encodingParameters->decoderConfiguration[descId][classId] = configuration;
    encodingParameters->encoding_mode_id[descId][classId] = encodingMode;

    return SUCCESS;
}

EncodingParametersRC setDefaultQVPSId(Encoding_ParametersType* encodingParameters, uint8_t classId,
                                      uint8_t defaultQVPSId) {
    if (classId >= encodingParameters->num_classes) {
        return OUT_OF_BOUNDERIES;
    }
    encodingParameters->qvps_flag[classId] = false;
    encodingParameters->qvps_preset_ID[classId] = defaultQVPSId;

    return SUCCESS;
}

EncodingParametersRC setParameterSetQVPS(Encoding_ParametersType* encodingParameters, uint8_t classId,
                                         Parameter_set_qvpsType* parameterSetQvps) {
    if (classId >= encodingParameters->num_classes) {
        return OUT_OF_BOUNDERIES;
    }
    encodingParameters->qvps_flag[classId] = true;
    encodingParameters->parameter_set_qvps[classId] = parameterSetQvps;

    return SUCCESS;
}

EncodingParametersRC getAdaptiveModeFlag(Cabac_context_parametersType* cabac_context_parameters,
                                         bool* adaptive_mode_flag) {
    if (cabac_context_parameters == NULL || adaptive_mode_flag == NULL) {
        return NULL_PTR;
    }
    *adaptive_mode_flag = cabac_context_parameters->adaptive_mode_flag;
    return SUCCESS;
}
EncodingParametersRC getContextInitializationValues(Cabac_context_parametersType* cabac_context_parameters,
                                                    uint16_t* num_contexts, uint8_t** context_initialization_value) {
    if (cabac_context_parameters == NULL || num_contexts == NULL || context_initialization_value == NULL) {
        return NULL_PTR;
    }
    *num_contexts = cabac_context_parameters->num_contexts;
    *context_initialization_value = cabac_context_parameters->context_initialization_value;
    return SUCCESS;
}
EncodingParametersRC getShareSubSymCtxFlag(Cabac_context_parametersType* cabac_context_parameters,
                                           uint8_t codingSymbolSize, uint8_t outputSymbolSize,
                                           bool* share_sub_sym_ctx_flag) {
    if (cabac_context_parameters == NULL || share_sub_sym_ctx_flag == NULL) {
        return NULL_PTR;
    }
    if (codingSymbolSize < outputSymbolSize) {
        *share_sub_sym_ctx_flag = cabac_context_parameters->share_sub_sym_ctx_flag;
        return SUCCESS;
    }
    return FIELDs_EXISTANCE_CONDITIONS_NOT_MET;
}

EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportOutputSymbolSize(
    Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac, uint8_t* outputSymbolSize) {
    if (decoderConfigurationTokentypeCabac == NULL || outputSymbolSize == NULL) {
        return NULL_PTR;
    }
    return getOutputSymbolSize(decoderConfigurationTokentypeCabac->support_values, outputSymbolSize);
}
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportCodingSymbolSize(
    Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac, uint8_t* codingSymbolSize) {
    if (decoderConfigurationTokentypeCabac == NULL || codingSymbolSize == NULL) {
        return NULL_PTR;
    }
    return getCodingSymbolSize(decoderConfigurationTokentypeCabac->support_values, codingSymbolSize);
}
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportCodingOrder(
    Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac, uint8_t* codingOrder) {
    if (decoderConfigurationTokentypeCabac == NULL || codingOrder == NULL) {
        return NULL_PTR;
    }
    return getCodingOrder(decoderConfigurationTokentypeCabac->support_values, codingOrder);
}
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportShareSubsymLutFlag(
    Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac, bool* shareSubsymLutFlag) {
    if (decoderConfigurationTokentypeCabac == NULL || shareSubsymLutFlag == NULL) {
        return NULL_PTR;
    }
    return getShareSubsymLUTFlag(decoderConfigurationTokentypeCabac->support_values,
                                 decoderConfigurationTokentypeCabac->transformSubSym, shareSubsymLutFlag);
}
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportShareSubsymPrvFlag(
    Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac, bool* shareSubsymPrvFlag) {
    if (decoderConfigurationTokentypeCabac == NULL || shareSubsymPrvFlag == NULL) {
        return NULL_PTR;
    }
    return getShareSubsymPRVFlag(decoderConfigurationTokentypeCabac->support_values, shareSubsymPrvFlag);
}

Encoding_ParametersType* readEncodingParameters(InputBitstream* input) {
    uint8_t dataset_type;
    uint8_t alphabet_ID;
    uint32_t read_length;
    uint8_t number_of_template_segments_minus1;
    uint32_t max_au_data_unit_size;
    bool pos_40_bits_flag;
    uint8_t qv_depth;
    uint8_t as_depth;
    uint8_t num_classes;
    uint8_t* classIDs;
    bool class_specific_dec_cfg_flag[18];
    uint8_t** dec_cfg_preset;
    uint8_t** encoding_mode_id;
    void*** decoderConfiguration;  // 13.1.2 //todo correct getter

    uint16_t num_groups;
    char** rgroup_ID;
    bool multiple_alignments_flag;
    bool spliced_reads_flag;
    uint32_t multiple_signature_base;
    uint8_t u_signature_size = 0;
    uint8_t* qv_coding_mode;
    bool* qvps_flag;
    Parameter_set_qvpsType** parameter_set_qvps;  // 8.1
    uint8_t* default_qvps_ID;
    bool* qv_reverse_flag;
    bool crps_flag;
    Parameter_set_crpsType* parameter_set_crps = NULL;  // 8.2

    bool datasetTypeSuccessfulRead = readNBits8(input, 4, &dataset_type);
    bool alphabetIdSuccessfulRead = readNBits8(input, 8, &alphabet_ID);
    bool readLengthSuccessfulRead = readNBitsBigToNativeEndian32(input, 24, &read_length);
    bool numberTemplateSegmentsSuccessfulRead = readNBits8(input, 2, &number_of_template_segments_minus1);
    uint8_t reserved;
    readNBits8(input, 6, &reserved);
    bool maxAuDataUnitSuccessfulRead = readNBitsBigToNativeEndian32(input, 29, &max_au_data_unit_size);
    uint8_t buffer;
    bool pos40BitsSuccessfulRead = readBit(input, &buffer);
    pos_40_bits_flag = buffer != 0;
    bool qvDepthSuccessfulRead = readNBits8(input, 3, &qv_depth);
    bool asDepthSuccessfulRead = readNBits8(input, 3, &as_depth);
    bool numClassesSuccessfulRead = readNBits8(input, 4, &num_classes);

    if (!datasetTypeSuccessfulRead || !alphabetIdSuccessfulRead || !readLengthSuccessfulRead ||
        !numberTemplateSegmentsSuccessfulRead || !maxAuDataUnitSuccessfulRead || !pos40BitsSuccessfulRead ||
        !qvDepthSuccessfulRead || !asDepthSuccessfulRead || !numClassesSuccessfulRead) {
        fprintf(stderr, "Error reading encoding parameters.\n");
        return NULL;
    }

    classIDs = malloc(sizeof(uint8_t) * num_classes);
    if (classIDs == NULL) {
        fprintf(stderr, "Could not allocate class ids.\n");
        return NULL;
    }

    for (uint8_t class_i = 0; class_i < num_classes; class_i++) {
        buffer = 0;
        if (!readNBits8(input, 4, &buffer)) {
            fprintf(stderr, "Error reading class id.\n");
            free(classIDs);
            return NULL;
        }
        classIDs[class_i] = buffer;
    }

    dec_cfg_preset = malloc(sizeof(uint8_t*) * 18);
    encoding_mode_id = malloc(sizeof(uint8_t*) * 18);
    decoderConfiguration = malloc(sizeof(void**) * 18);

    for (uint8_t descriptor_i = 0; descriptor_i < 18; descriptor_i++) {
        buffer = 0;
        bool class_specific_dec_cfg_flagSuccessfulRead = readBit(input, &buffer);
        if (!class_specific_dec_cfg_flagSuccessfulRead) {
            fprintf(stderr, "Error reading class specific flag");
            free(classIDs);
            return NULL;
        }

        // fprintf(stderr, "BufferContent:%u\n", buffer);

        class_specific_dec_cfg_flag[descriptor_i] = buffer != 0;

        if (!class_specific_dec_cfg_flag[descriptor_i]) {
            dec_cfg_preset[descriptor_i] = malloc(sizeof(bool));  // TODO:bool?
            encoding_mode_id[descriptor_i] = malloc(sizeof(uint8_t));
            decoderConfiguration[descriptor_i] = malloc(sizeof(void*));

            readNBits8(input, 8, &(dec_cfg_preset[descriptor_i][0]));
            if (dec_cfg_preset[descriptor_i][0] != 0) {
                fprintf(stderr, "dec_cfg_preset must be equal to 0.\n");
                return NULL;
            }

            readNBits8(input, 8, &(encoding_mode_id[descriptor_i][0]));
            if (encoding_mode_id[descriptor_i][0] != 0) {
                fprintf(stderr, "encoding mode must be equal to 0.\n");
                return NULL;
            }

            if (descriptor_i != 11 && descriptor_i != 15) {
                decoderConfiguration[descriptor_i][0] = readDecoderConfigurationTypeCABAC(input);
            } else {
                decoderConfiguration[descriptor_i][0] = readDecoder_configuration_tokentypeCABAC(input);
            }

        } else {
            dec_cfg_preset[descriptor_i] = malloc(sizeof(bool) * num_classes);
            encoding_mode_id[descriptor_i] = malloc(sizeof(uint8_t) * num_classes);
            decoderConfiguration[descriptor_i] = malloc(sizeof(void*) * num_classes);
            for (uint8_t class_i = 0; class_i < num_classes; class_i++) {
                readNBits8(input, 8, dec_cfg_preset[descriptor_i] + class_i);
                if (dec_cfg_preset[descriptor_i][class_i] != 0) {
                    return NULL;
                }

                readNBits8(input, 8, encoding_mode_id[descriptor_i] + class_i);
                if (descriptor_i != 11 && descriptor_i != 15) {
                    decoderConfiguration[descriptor_i][class_i] = readDecoderConfigurationTypeCABAC(input);
                } else {
                    decoderConfiguration[descriptor_i][class_i] = readDecoder_configuration_tokentypeCABAC(input);
                }
            }
        }
    }

    if (!readNBitsBigToNativeEndian16(input, 16, &num_groups)) {
        fprintf(stderr, "Error reading num_groups");
        free(classIDs);
    }

    rgroup_ID = malloc(sizeof(char*) * num_groups);
    for (uint16_t group_i = 0; group_i < num_groups; group_i++) {
        uint16_t allocatedLength = 1024;
        rgroup_ID[group_i] = calloc(allocatedLength, sizeof(char));
        uint16_t currentLength = 0;

        char currentChar;
        readNBits8(input, 8, (uint8_t*)&currentChar);
        while (currentChar != '\0') {
            rgroup_ID[group_i][currentLength] = currentChar;
            readNBits8(input, 8, (uint8_t*)&currentChar);
            currentLength++;
            if (currentLength == allocatedLength) {
                rgroup_ID[group_i] = realloc(rgroup_ID[group_i], sizeof(char) * (allocatedLength + 1024));
                allocatedLength += 1024;
            }
        }
        rgroup_ID[group_i][currentLength] = '\0';
    }

    if (!readBit(input, &buffer)) {
        fprintf(stderr, "Could not read multiple alignments flag.\n");
        for (uint16_t group_i = 0; group_i < num_groups; group_i++) {
            free(rgroup_ID[group_i]);
        }
        free(rgroup_ID);
        free(classIDs);
        return NULL;
    }
    multiple_alignments_flag = buffer != 0;

    if (!readBit(input, &buffer)) {
        fprintf(stderr, "Could not read spliced_reads_flag\n");
        for (uint16_t group_i = 0; group_i < num_groups; group_i++) {
            free(rgroup_ID[group_i]);
        }
        free(rgroup_ID);
        free(classIDs);
        return NULL;
    }

    spliced_reads_flag = buffer != 0;

    readNBitsBigToNativeEndian32(input, 31, &multiple_signature_base);
    if (multiple_signature_base > 0) {
        if (!readNBits8(input, 6, &u_signature_size)) {
            fprintf(stderr, "Error reading u_signature_size.\n");
            for (uint16_t group_i = 0; group_i < num_groups; group_i++) {
                free(rgroup_ID[group_i]);
            }
            free(rgroup_ID);
            free(classIDs);
            return NULL;
        }
    }

    qv_coding_mode = malloc(sizeof(uint8_t) * num_classes);
    qvps_flag = malloc(sizeof(bool) * num_classes);
    parameter_set_qvps = malloc(sizeof(Parameter_set_qvpsType*) * num_classes);
    default_qvps_ID = malloc(sizeof(uint8_t) * num_classes);
    qv_reverse_flag = malloc(sizeof(bool) * num_classes);
    if (qv_coding_mode == NULL) {
        fprintf(stderr, "could not allocate qv coding mode.\n");
        for (uint16_t group_i = 0; group_i < num_groups; group_i++) {
            free(rgroup_ID[group_i]);
        }
        free(rgroup_ID);
        free(classIDs);
        return NULL;
    }

    for (uint16_t class_i = 0; class_i < num_classes; class_i++) {
        readNBits8(input, 4, &buffer);

        qv_coding_mode[class_i] = buffer;
        if (qv_coding_mode[class_i] == 1) {
            readBit(input, &buffer);
            qvps_flag[class_i] = buffer != 0;

            if (qvps_flag[class_i]) {
                parameter_set_qvps[class_i] = readParameterSetQvps(input);
            } else {
                readNBits8(input, 4, &buffer);
                default_qvps_ID[class_i] = buffer;
            }
        }

        readBit(input, &buffer);
        qv_reverse_flag[class_i] = buffer != 0;  // TODO:?
    }

    readBit(input, &buffer);
    crps_flag = buffer != 0;
    if (crps_flag) {
        parameter_set_crps = readParameterSetCrps(input);
    }  // TODO:byte_aligned?

    Encoding_ParametersType* encodingParameters = malloc(sizeof(Encoding_ParametersType));

    encodingParameters->dataset_type = dataset_type;
    encodingParameters->alphabet_ID = alphabet_ID;
    encodingParameters->read_length = read_length;
    encodingParameters->number_of_template_segments_minus1 = number_of_template_segments_minus1;
    encodingParameters->max_au_data_unit_size = max_au_data_unit_size;
    encodingParameters->pos_40_bits = pos_40_bits_flag;
    encodingParameters->qv_depth = qv_depth;
    encodingParameters->as_depth = as_depth;
    encodingParameters->num_classes = num_classes;
    encodingParameters->classID = classIDs;
    for (uint8_t descriptor_i = 0; descriptor_i < 18; descriptor_i++) {
        encodingParameters->class_specific_dec_cfg_flag[descriptor_i] = class_specific_dec_cfg_flag[descriptor_i];
    }
    encodingParameters->dec_cfg_preset = dec_cfg_preset;
    encodingParameters->encoding_mode_id = encoding_mode_id;
    encodingParameters->decoderConfiguration = decoderConfiguration;
    encodingParameters->num_groups = num_groups;
    encodingParameters->rgroup_ID = rgroup_ID;
    encodingParameters->multiple_alignments_flag = multiple_alignments_flag;
    encodingParameters->spliced_reads_flag = spliced_reads_flag;
    encodingParameters->multiple_signature_base = multiple_signature_base;
    encodingParameters->U_signature_size = u_signature_size;
    encodingParameters->qv_coding_mode = qv_coding_mode;
    encodingParameters->qvps_flag = qvps_flag;
    encodingParameters->parameter_set_qvps = parameter_set_qvps;
    encodingParameters->qvps_preset_ID = default_qvps_ID;
    encodingParameters->qv_reverse_flag = qv_reverse_flag;
    encodingParameters->crps_flag = crps_flag;
    encodingParameters->parameter_set_crps = parameter_set_crps;

    return encodingParameters;
}

Parameter_set_qvpsType* readParameterSetQvps(InputBitstream* input) {
    uint8_t qv_num_codebooks_total;
    if (!readNBits8(input, 4, &qv_num_codebooks_total)) {
        fprintf(stderr, "Error reading number qv_num_codebooks_total");
    };
    uint8_t* qv_num_codebook_entries = (uint8_t*)malloc(qv_num_codebooks_total * sizeof(uint8_t));
    uint8_t** qv_recon = (uint8_t**)malloc(qv_num_codebooks_total * sizeof(uint8_t*));

    bool allCorrect = true;
    for (uint8_t b = 0; b < qv_num_codebooks_total; b++) {
        if (!readNBits8(input, 8, qv_num_codebook_entries + b)) {
            allCorrect = false;
            break;
        }
        qv_recon[b] = (uint8_t*)malloc(qv_num_codebook_entries[b] * sizeof(uint8_t));
        if (qv_recon[b] == NULL) {
            allCorrect = false;
            break;
        }
        for (uint8_t e = 0; e < qv_num_codebook_entries[b]; e++) {
            if (!readNBits8(input, 8, &(qv_recon[b][e]))) {
                allCorrect = false;
                break;
            }
        }
    }
    if (!allCorrect) {
        for (uint8_t b = 0; b < qv_num_codebooks_total; b++) {
            free(qv_recon[b]);
        }
        free(qv_recon);
        free(qv_num_codebook_entries);

        return NULL;
    }

    return constructParameterSetQvpsType(qv_num_codebooks_total, qv_num_codebook_entries, qv_recon);
}

void freeEncodingParameters(Encoding_ParametersType* encodingParametersType) {
    free(encodingParametersType->decoderConfiguration);

    for (uint16_t group_i = 0; group_i < encodingParametersType->num_groups; group_i++) {
        free(encodingParametersType->rgroup_ID[group_i]);
    }
    free(encodingParametersType->rgroup_ID);
    free(encodingParametersType->classID);
    if (encodingParametersType->crps_flag) {
        freeParameterSetCrps(encodingParametersType->parameter_set_crps);
    }
    free(encodingParametersType);
}

Parameter_set_crpsType* readParameterSetCrps(InputBitstream* inputBitstream) {
    uint8_t cr_alg_ID;
    if (!readNBits8(inputBitstream, 8, &cr_alg_ID)) {
        fprintf(stderr, "Could not read cr_alg_ID.\n");
        return NULL;
    }
    uint8_t cr_pad_size = 0;
    uint32_t cr_buf_max_size = 0;
    if (cr_alg_ID == 2 || cr_alg_ID == 3) {
        if (!readNBits8(inputBitstream, 8, &cr_pad_size) ||
            !readNBitsBigToNativeEndian32(inputBitstream, 32, &cr_buf_max_size)) {
            fprintf(stderr, "Could not read cr_pad_size or cr_buf_max_size.\n");
            return NULL;
        }
    }
    return constructParameterSetCrps(cr_alg_ID, cr_pad_size, cr_buf_max_size);
}

size_t getSizeParametersCrps(Parameter_set_crpsType* parameter_set_crpsType) {
    size_t sizeInBits = 0;
    sizeInBits += 8;  // cr_alg_ID
    if (parameter_set_crpsType->cr_alg_ID == 2 || parameter_set_crpsType->cr_alg_ID == 3) {
        sizeInBits += 8;   // cr_pad_size
        sizeInBits += 24;  // cr_buf_max_size
    }
    return sizeInBits;
}

Parameter_set_qvpsType* copyParameter_set_qvps(Parameter_set_qvpsType* input) {
    Parameter_set_qvpsType* result = malloc(sizeof(Parameter_set_qvpsType));
    result->qv_num_codebooks_total = input->qv_num_codebooks_total;
    result->qv_num_codebook_entries = malloc(sizeof(uint8_t) * input->qv_num_codebooks_total);
    result->qv_recon = malloc(sizeof(uint8_t*) * input->qv_num_codebooks_total);
    for (uint8_t qv_codebook_i = 0; qv_codebook_i < input->qv_num_codebooks_total; qv_codebook_i++) {
        result->qv_num_codebook_entries[qv_codebook_i] = input->qv_num_codebook_entries[qv_codebook_i];
        result->qv_recon[qv_codebook_i] = malloc(sizeof(uint8_t) * result->qv_num_codebook_entries[qv_codebook_i]);
        for (uint8_t entry_i = 0; entry_i < input->qv_num_codebook_entries[qv_codebook_i]; entry_i++) {
            result->qv_recon[qv_codebook_i][entry_i] = input->qv_recon[qv_codebook_i][entry_i];
        }
    }
    return result;
}

Encoding_ParametersType* copyEncodingParameters(Encoding_ParametersType* encodingParametersType) {
    Encoding_ParametersType* encodingParametersTypeCopy = malloc(sizeof(Encoding_ParametersType));
    uint8_t dataset_type;
    encodingParametersTypeCopy->dataset_type = encodingParametersType->dataset_type;
    encodingParametersTypeCopy->alphabet_ID = encodingParametersType->alphabet_ID;
    encodingParametersTypeCopy->read_length = encodingParametersType->read_length;
    encodingParametersTypeCopy->number_of_template_segments_minus1 =
        encodingParametersType->number_of_template_segments_minus1;

    encodingParametersTypeCopy->max_au_data_unit_size = encodingParametersType->max_au_data_unit_size;
    encodingParametersTypeCopy->pos_40_bits = encodingParametersType->pos_40_bits;
    encodingParametersTypeCopy->qv_depth = encodingParametersType->qv_depth;
    encodingParametersTypeCopy->as_depth = encodingParametersType->as_depth;
    encodingParametersTypeCopy->num_classes = encodingParametersType->num_classes;
    encodingParametersTypeCopy->classID = malloc((sizeof(uint8_t) * encodingParametersType->num_classes));
    for (size_t class_i = 0; class_i < encodingParametersType->num_classes; class_i++) {
        encodingParametersTypeCopy->classID[class_i] = encodingParametersType->classID[class_i];
    }

    encodingParametersTypeCopy->dec_cfg_preset = malloc(sizeof(uint8_t*) * 18);
    encodingParametersTypeCopy->encoding_mode_id = malloc(sizeof(uint8_t*) * 18);
    encodingParametersTypeCopy->decoderConfiguration = malloc(sizeof(void**) * 18);
    for (size_t descriptor_i = 0; descriptor_i < 18; descriptor_i++) {
        encodingParametersTypeCopy->class_specific_dec_cfg_flag[descriptor_i] =
            encodingParametersType->class_specific_dec_cfg_flag[descriptor_i];
        size_t endLoop =
            encodingParametersType->class_specific_dec_cfg_flag[descriptor_i] ? encodingParametersType->num_classes : 1;
        encodingParametersTypeCopy->dec_cfg_preset[descriptor_i] = malloc(sizeof(uint8_t) * endLoop);
        encodingParametersTypeCopy->encoding_mode_id[descriptor_i] = malloc(sizeof(uint8_t) * endLoop);
        encodingParametersTypeCopy->decoderConfiguration[descriptor_i] = malloc(sizeof(void*) * endLoop);
        for (size_t class_i = 0; class_i < endLoop; class_i++) {
            encodingParametersTypeCopy->dec_cfg_preset[descriptor_i][class_i] =
                encodingParametersType->dec_cfg_preset[descriptor_i][class_i];
            encodingParametersTypeCopy->encoding_mode_id[descriptor_i][class_i] =
                encodingParametersType->encoding_mode_id[descriptor_i][class_i];
            if (descriptor_i != 11 && descriptor_i != 15) {
                encodingParametersTypeCopy->decoderConfiguration[descriptor_i][class_i] =
                    copyDecoderConfigurationTypeCABAC(
                        encodingParametersType->decoderConfiguration[descriptor_i][class_i]);
            } else {
                encodingParametersTypeCopy->decoderConfiguration[descriptor_i][class_i] =
                    copyDecoder_configuration_tokentypeCABAC(
                        encodingParametersType->decoderConfiguration[descriptor_i][class_i]);
            }
        }
    }

    encodingParametersTypeCopy->num_groups = encodingParametersType->num_groups;
    encodingParametersTypeCopy->rgroup_ID = malloc(sizeof(char*) * encodingParametersType->num_groups);
    for (size_t group_i = 0; group_i < encodingParametersType->num_groups; group_i++) {
        encodingParametersTypeCopy->rgroup_ID[group_i] = strdup(encodingParametersType->rgroup_ID[group_i]);
    }
    encodingParametersTypeCopy->multiple_alignments_flag = encodingParametersType->multiple_alignments_flag;
    encodingParametersTypeCopy->spliced_reads_flag = encodingParametersType->spliced_reads_flag;
    encodingParametersTypeCopy->multiple_signature_base = encodingParametersType->multiple_signature_base;
    encodingParametersTypeCopy->U_signature_size = encodingParametersType->U_signature_size;

    encodingParametersTypeCopy->qv_coding_mode = malloc(sizeof(uint8_t) * encodingParametersType->num_classes);
    encodingParametersTypeCopy->qvps_flag = malloc(sizeof(bool) * encodingParametersType->num_classes);
    encodingParametersTypeCopy->parameter_set_qvps =
        malloc(sizeof(Parameter_set_qvpsType*) * encodingParametersType->num_classes);
    encodingParametersTypeCopy->qvps_preset_ID = malloc(sizeof(uint8_t*) * encodingParametersType->num_classes);
    encodingParametersTypeCopy->qv_reverse_flag = malloc(sizeof(bool) * encodingParametersType->num_classes);

    for (size_t class_i = 0; class_i < encodingParametersType->num_classes; class_i++) {
        encodingParametersTypeCopy->qv_coding_mode[class_i] = encodingParametersType->qv_coding_mode[class_i];
        if (encodingParametersTypeCopy->qv_coding_mode[class_i] == 1) {
            encodingParametersTypeCopy->qvps_flag[class_i] = encodingParametersType->qvps_flag[class_i];
            if (encodingParametersTypeCopy->qvps_flag[class_i]) {
                encodingParametersTypeCopy->parameter_set_qvps[class_i] =
                    copyParameter_set_qvps(encodingParametersType->parameter_set_qvps[class_i]);
            } else {
                encodingParametersTypeCopy->qvps_preset_ID[class_i] = encodingParametersType->qvps_preset_ID[class_i];
                encodingParametersTypeCopy->parameter_set_qvps[class_i] = NULL;
            }
        }
        encodingParametersTypeCopy->qv_reverse_flag[class_i] = encodingParametersType->qv_reverse_flag[class_i];
    }

    encodingParametersTypeCopy->crps_flag = encodingParametersType->crps_flag;
    if (encodingParametersType->crps_flag) {
        encodingParametersTypeCopy->parameter_set_crps =
            copyParameterSetCrps(encodingParametersType->parameter_set_crps);
    } else {
        encodingParametersTypeCopy->parameter_set_crps = NULL;
    }

    return encodingParametersTypeCopy;
}
