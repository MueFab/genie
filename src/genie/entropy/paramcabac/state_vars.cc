/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include "state_vars.h"
#include "binarization_parameters.h"
#include <genie/core/constants.h>
#include <genie/util/exceptions.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

StateVars::StateVars()
    : numAlphaSubsym(0),
      numSubsyms(0),
      numCtxSubsym(0),
      codingOrderCtxOffset{0,0,0},
      codingSizeCtxOffset(0),
      numCtxLuts(0),
      numCtxTotal(0){}

// ---------------------------------------------------------------------------------------------------------------------
void StateVars::populate(const SupportValues::TransformIdSubsym transform_ID_subsym,
                         const SupportValues support_values,
                         const Binarization cabac_binarization) {

    const BinarizationParameters::BinarizationId binarization_ID = cabac_binarization.getBinarizationID();
    const BinarizationParameters& cabacBinazParams = cabac_binarization.getCabacBinarizationParameters();
    const Context& cabacContextParams = cabac_binarization.getCabacContextParameters();

    core::GenDesc descriptor_ID = core::GenDesc::POS;                   // need descriptor_ID to be supplied to this function TODO
    core::GenSubIndex subsequence_ID = core::GenSub::POS_MAPPING_FIRST; // need subsequence_ID to be supplied to this function TODO
    core::AlphabetID alphabet_ID = core::AlphabetID::ACGTN;             // need alphabet_ID to be supplied to this function TODO

    const uint8_t codingSubsymSize = support_values.getCodingSubsymSize();
    const uint8_t outputSymbolSize = support_values.getOutputSymbolSize();
    const uint8_t codingOrder = support_values.getCodingOrder();
    const bool shareSubSymCtxFlag = cabacContextParams.getShareSubsymCtxFlag();
    const uint16_t numContexts = cabacContextParams.getNumContexts();


    // numSubsyms
    if(codingSubsymSize > 0) {
        numSubsyms = outputSymbolSize / codingSubsymSize;
    } else {
        UTILS_THROW_RUNTIME_EXCEPTION("coding_subsym_size = "+std::to_string(codingSubsymSize)+" not supported");
    }

    // numAlphaSubsym
    // Check for special cases for numAlphabetSymbols
    if(descriptor_ID == core::GenDesc::MMTYPE || descriptor_ID == core::GenDesc::RFTT) {
        if(subsequence_ID == core::GenSub::MMTYPE_TYPE || subsequence_ID == core::GenSub::RFTT_TYPE) { // subseq 0
            numAlphaSubsym = 3;
        } else if((subsequence_ID == core::GenSub::MMTYPE_SUBSTITUTION || subsequence_ID == core::GenSub::RFTT_SUBSTITUTION) // subseq 1
                ||(subsequence_ID == core::GenSub::MMTYPE_INSERTION || subsequence_ID == core::GenSub::RFTT_INSERTION) // subseq 2
                  ) {
            numAlphaSubsym = (alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16;
        }
    } else if(descriptor_ID == core::GenDesc::CLIPS) {
        if(subsequence_ID == core::GenSub::CLIPS_TYPE) { // subseq 1
            numAlphaSubsym = 9;
        } else if(subsequence_ID == core::GenSub::CLIPS_SOFT_STRING) { // subseq 2
            numAlphaSubsym = ((alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16) + 1;
        }
    } else if(descriptor_ID == core::GenDesc::UREADS && subsequence_ID == core::GenSub::UREADS) { // subseq 0
        numAlphaSubsym = (alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16;
    } else if(descriptor_ID == core::GenDesc::RTYPE && subsequence_ID ==  core::GenSub::RTYPE) {
        numAlphaSubsym = 6;
    }

    if(numAlphaSubsym == 0) { // 0 == not special
        numAlphaSubsym = (1<<codingSubsymSize);
    }

    // numCtxSubsym
    switch(binarization_ID) {
        case BinarizationParameters::BinarizationId::BINARY_CODING:
            numCtxSubsym = codingSubsymSize;
            cLengthBI = numCtxSubsym;
        break;
        case BinarizationParameters::BinarizationId::TRUNCATED_UNARY:
            numCtxSubsym = cabacBinazParams.getCMax(); // cmax
        break;
        case BinarizationParameters::BinarizationId::EXPONENTIAL_GOLOMB:
            numCtxSubsym = std::floor(std::log2(numAlphaSubsym+1))+1;
        break;
        case BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
            numCtxSubsym = std::floor(std::log2(numAlphaSubsym+1))+2;
        break;
        case BinarizationParameters::BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
            numCtxSubsym = cabacBinazParams.getCMaxTeg() // cmax_teg
                         + std::floor(std::log2(numAlphaSubsym+1))+1;
        break;
        case BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            numCtxSubsym = cabacBinazParams.getCMaxTeg() // cmax_teg
                         + std::floor(std::log2(numAlphaSubsym+1))+2;
        break;
        case BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
        {
            uint8_t splitUnitSize = cabacBinazParams.getSplitUnitSize();
            numCtxSubsym = (outputSymbolSize/splitUnitSize)
                         * ((1<<splitUnitSize)-1)
                         + ((1<<(outputSymbolSize%splitUnitSize))-1);
        }
        break;
        case BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
        {
            uint8_t splitUnitSize = cabacBinazParams.getSplitUnitSize();
            numCtxSubsym = (outputSymbolSize/splitUnitSize)
                         * ((1<<splitUnitSize)-1)
                         + ((1<<(outputSymbolSize%splitUnitSize))-1)
                         + 1;
        }
        break;
        case BinarizationParameters::BinarizationId::DOUBLE_TRUNCATED_UNARY:
        {
            uint8_t splitUnitSize = cabacBinazParams.getSplitUnitSize();
            numCtxSubsym = cabacBinazParams.getCMaxDtu()
                         + (outputSymbolSize/splitUnitSize)
                         * ((1<<splitUnitSize)-1)
                         + ((1<<(outputSymbolSize%splitUnitSize))-1);
        }
        break;
        case BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
        {
            uint8_t splitUnitSize = cabacBinazParams.getSplitUnitSize();
            numCtxSubsym = cabacBinazParams.getCMaxDtu()
                         + (outputSymbolSize/splitUnitSize)
                         * ((1<<splitUnitSize)-1)
                         + ((1<<(outputSymbolSize%splitUnitSize))-1)
                         + 1;
        }
        break;
    }

    // codingOrderCtxOffset[]
    codingOrderCtxOffset[0] = 0;
    if(codingOrder >= 1)
        codingOrderCtxOffset[1] = numCtxSubsym;
    if(codingOrder == 2)
        codingOrderCtxOffset[2] = numCtxSubsym * numAlphaSubsym;

    // codingSizeCtxOffset
    if(shareSubSymCtxFlag) {
        codingSizeCtxOffset = 0;
    } else if(codingOrder == 0) {
        codingSizeCtxOffset = numCtxSubsym;
    } else {
        codingSizeCtxOffset = codingOrderCtxOffset[codingOrder] *  numAlphaSubsym;
    }

    // numCtxLuts
    numCtxLuts = 0;
    if(transform_ID_subsym == SupportValues::TransformIdSubsym::LUT_TRANSFORM) {
        if((binarization_ID >= BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY &&
                binarization_ID <= BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY) ||
                codingOrder == 0 ||
                (1u<<codingSubsymSize) > MAX_LUT_SIZE) {
            UTILS_THROW_RUNTIME_EXCEPTION("LUT_TRANSFORM not supported with given configuration: coding_order = 0\
                                         , binarization_ID = "+std::to_string((uint8_t)binarization_ID)+"\
                                         , coding_subsym_size = "+std::to_string(codingSubsymSize));
        } else {
            numCtxLuts = (codingSubsymSize/2)
                       * ((1<<2)-1)
                       + ((1<<(codingSubsymSize%2))-1);
        }
    }

   // numCtxTotal
    if(numContexts != 0) {
        numCtxTotal = numContexts;
    } else {
        numCtxTotal = numCtxLuts;
        numCtxTotal += (shareSubSymCtxFlag) ?
                        1 :
                        numSubsyms *
                        ((codingOrder == 0) ?
                                numCtxSubsym :
                                codingOrderCtxOffset[codingOrder] *
                                numAlphaSubsym);
    }
}

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
