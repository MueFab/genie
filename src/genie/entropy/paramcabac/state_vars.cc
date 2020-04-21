/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include "state_vars.h"
#include "binarization_parameters.h"
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
      cLengthBI(0),
      codingOrderCtxOffset{0,0,0},
      codingSizeCtxOffset(0),
      numCtxLuts(0),
      numCtxTotal(0) {}

// ---------------------------------------------------------------------------------------------------------------------
uint64_t StateVars::getNumAlphaSpecial(const core::GenSubIndex subsequence_ID,
                                       const core::AlphabetID alphabet_ID) {
    unsigned long numAlphaSpecial = 0;
    if(subsequence_ID == core::GenSub::MMTYPE_TYPE) { // mmtype subseq 0
            numAlphaSpecial = 3;
    } else if((subsequence_ID == core::GenSub::MMTYPE_SUBSTITUTION) // mmtype subseq 1
            ||(subsequence_ID == core::GenSub::MMTYPE_INSERTION) // mmtype subseq 2
    ) {
        numAlphaSpecial = (alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16;
    } if(subsequence_ID == core::GenSub::CLIPS_TYPE) { // mmtype subseq 1
            numAlphaSpecial = 9;
    } else if(subsequence_ID == core::GenSub::CLIPS_SOFT_STRING) { // subseq 2
        numAlphaSpecial = ((alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16) + 1;
    } else if(subsequence_ID == core::GenSub::UREADS) { // ureads subseq 0
        numAlphaSpecial = (alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16;
    } else if(subsequence_ID ==  core::GenSub::RTYPE) { // rtype subseq 0
        numAlphaSpecial = 6;
    } else if(subsequence_ID == core::GenSub::RFTT) { // rftt subseq 0
        numAlphaSpecial = (alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16;
    }

    return numAlphaSpecial;
}


// ---------------------------------------------------------------------------------------------------------------------
uint8_t StateVars::getNumLuts(const uint8_t codingOrder,
                              const bool shareSubsymLutFlag,
                              const SupportValues::TransformIdSubsym trnsfSubsymID) const {
    return (codingOrder > 0 && trnsfSubsymID == SupportValues::TransformIdSubsym::LUT_TRANSFORM)
            ? ((shareSubsymLutFlag)
                    ? 1
                    : numSubsyms)
            : 0;
}

// ---------------------------------------------------------------------------------------------------------------------
uint8_t StateVars::getNumPrvs(const bool shareSubsymPrvFlag) const {
    return (shareSubsymPrvFlag) ? 1 : numSubsyms;
}

// ---------------------------------------------------------------------------------------------------------------------
void StateVars::populate(const SupportValues::TransformIdSubsym transform_ID_subsym,
                         const SupportValues support_values,
                         const Binarization cabac_binarization,
                         const core::GenSubIndex subsequence_ID,
                         const core::AlphabetID alphabet_ID) {

    const BinarizationParameters::BinarizationId binarization_ID = cabac_binarization.getBinarizationID();
    const BinarizationParameters& cabacBinazParams = cabac_binarization.getCabacBinarizationParameters();
    const Context& cabacContextParams = cabac_binarization.getCabacContextParameters();

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
    // Check for special cases for numAlphaSubsym
    numAlphaSubsym = StateVars::getNumAlphaSpecial(subsequence_ID, alphabet_ID);

    if(numAlphaSubsym == 0) { // 0 == not special
        numAlphaSubsym = StateVars::get2PowN(codingSubsymSize);
    }

    //cLengthBI
    if(binarization_ID == BinarizationParameters::BinarizationId::BINARY_CODING) {
        cLengthBI = codingSubsymSize;
    }

    if (!cabac_binarization.getBypassFlag()) {
        // numCtxSubsym
        switch(binarization_ID) {
            case BinarizationParameters::BinarizationId::BINARY_CODING:
                numCtxSubsym = codingSubsymSize;

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
            //std::cout<<"NumContexts: "<< std::to_string(numCtxTotal) << std::endl;
        }
    } // if (!cabac_binarization.getBypassFlag())
}

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
