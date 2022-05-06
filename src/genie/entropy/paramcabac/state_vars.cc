/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include "genie/entropy/paramcabac/state_vars.h"
#include <cmath>
#include <iostream>
#include "genie/entropy/paramcabac/binarization_parameters.h"
#include "genie/util/runtime-exception.h"

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
      codingOrderCtxOffset{0, 0, 0},
      codingSizeCtxOffset(0),
      numCtxLuts(0),
      numCtxTotal(0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t StateVars::getNumAlphaSpecial(const core::GenSubIndex subsequence_ID, const core::AlphabetID alphabet_ID) {
    uint64_t numAlphaSpecial = 0;
    if (subsequence_ID == core::GenSub::MMTYPE_TYPE) {  // mmtype subseq 0
        numAlphaSpecial = 3;
    } else if ((subsequence_ID == core::GenSub::MMTYPE_SUBSTITUTION)  // mmtype subseq 1
               || (subsequence_ID == core::GenSub::MMTYPE_INSERTION)  // mmtype subseq 2
    ) {
        numAlphaSpecial = (alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16;
    }
    if (subsequence_ID == core::GenSub::CLIPS_TYPE) {  // mmtype subseq 1
        numAlphaSpecial = 9;
    } else if (subsequence_ID == core::GenSub::CLIPS_SOFT_STRING) {  // subseq 2
        numAlphaSpecial = ((alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16) + 1;
    } else if (subsequence_ID == core::GenSub::UREADS) {  // ureads subseq 0
        numAlphaSpecial = (alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16;
    } else if (subsequence_ID == core::GenSub::RTYPE) {  // rtype subseq 0
        numAlphaSpecial = 6;
    } else if (subsequence_ID == core::GenSub::RFTT) {  // rftt subseq 0
        numAlphaSpecial = (alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16;
    }

    return numAlphaSpecial;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t StateVars::getNumLuts(const uint8_t codingOrder, const bool shareSubsymLutFlag,
                              const SupportValues::TransformIdSubsym trnsfSubsymID) const {
    return (codingOrder > 0 && trnsfSubsymID == SupportValues::TransformIdSubsym::LUT_TRANSFORM)
               ? ((shareSubsymLutFlag) ? 1 : (uint8_t)numSubsyms)
               : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t StateVars::getNumPrvs(const bool shareSubsymPrvFlag) const {
    return (shareSubsymPrvFlag) ? 1 : (uint8_t)numSubsyms;
}

// ---------------------------------------------------------------------------------------------------------------------

void StateVars::populate(const SupportValues::TransformIdSubsym transform_ID_subsym, const SupportValues support_values,
                         const Binarization cabac_binarization, const core::GenSubIndex subsequence_ID,
                         const core::AlphabetID alphabet_ID, bool original) {
    const BinarizationParameters::BinarizationId binarization_ID = cabac_binarization.getBinarizationID();
    const BinarizationParameters& cabacBinazParams = cabac_binarization.getCabacBinarizationParameters();
    const Context& cabacContextParams = cabac_binarization.getCabacContextParameters();

    const uint8_t codingSubsymSize = support_values.getCodingSubsymSize();
    const uint8_t outputSymbolSize = support_values.getOutputSymbolSize();
    const uint8_t codingOrder = support_values.getCodingOrder();
    const bool shareSubSymCtxFlag = cabacContextParams.getShareSubsymCtxFlag();
    const uint16_t numContexts = cabacContextParams.getNumContexts();
    numAlphaSubsym = 0;

    // numSubsyms
    if (codingSubsymSize > 0) {
        numSubsyms = outputSymbolSize / codingSubsymSize;
    } else {
        static bool print = false;
        if (!print) {
            std::cerr << "coding_subsym_size = " + std::to_string(codingSubsymSize) + " not supported" << std::endl;
            print = true;
        }
    }

    // numAlphaSubsym
    // Check for special cases for numAlphaSubsym
    if (original) {
        numAlphaSubsym = StateVars::getNumAlphaSpecial(subsequence_ID, alphabet_ID);
    }

    if (numAlphaSubsym == 0) {  // 0 == not special
        numAlphaSubsym = StateVars::get2PowN(codingSubsymSize);
    }

    // cLengthBI
    if (binarization_ID == BinarizationParameters::BinarizationId::BI) {
        cLengthBI = codingSubsymSize;
    }

    if (!cabac_binarization.getBypassFlag()) {
        // numCtxSubsym
        switch (binarization_ID) {
            case BinarizationParameters::BinarizationId::BI:
                numCtxSubsym = codingSubsymSize;

                break;
            case BinarizationParameters::BinarizationId::TU:
                numCtxSubsym = cabacBinazParams.getCMax();  // cmax
                break;
            case BinarizationParameters::BinarizationId::EG:
                numCtxSubsym = uint32_t(std::floor(std::log2(numAlphaSubsym + 1)) + 1);
                break;
            case BinarizationParameters::BinarizationId::SEG:
                numCtxSubsym = uint32_t(std::floor(std::log2(numAlphaSubsym + 1)) + 2);
                break;
            case BinarizationParameters::BinarizationId::TEG:
                numCtxSubsym = cabacBinazParams.getCMaxTeg()  // cmax_teg
                               + uint32_t(std::floor(std::log2(numAlphaSubsym + 1)) + 1);
                break;
            case BinarizationParameters::BinarizationId::STEG:
                numCtxSubsym = cabacBinazParams.getCMaxTeg()  // cmax_teg
                               + uint32_t(std::floor(std::log2(numAlphaSubsym + 1)) + 2);
                break;
            case BinarizationParameters::BinarizationId::SUTU: {
                uint8_t splitUnitSize = cabacBinazParams.getSplitUnitSize();
                numCtxSubsym = (outputSymbolSize / splitUnitSize) * ((1 << splitUnitSize) - 1) +
                               ((1 << (outputSymbolSize % splitUnitSize)) - 1);
            } break;
            case BinarizationParameters::BinarizationId::SSUTU: {
                uint8_t splitUnitSize = cabacBinazParams.getSplitUnitSize();
                numCtxSubsym = (outputSymbolSize / splitUnitSize) * ((1 << splitUnitSize) - 1) +
                               ((1 << (outputSymbolSize % splitUnitSize)) - 1) + 1;
            } break;
            case BinarizationParameters::BinarizationId::DTU: {
                uint8_t splitUnitSize = cabacBinazParams.getSplitUnitSize();
                numCtxSubsym = cabacBinazParams.getCMaxDtu() +
                               (outputSymbolSize / splitUnitSize) * ((1 << splitUnitSize) - 1) +
                               ((1 << (outputSymbolSize % splitUnitSize)) - 1);
            } break;
            case BinarizationParameters::BinarizationId::SDTU: {
                uint8_t splitUnitSize = cabacBinazParams.getSplitUnitSize();
                numCtxSubsym = cabacBinazParams.getCMaxDtu() +
                               (outputSymbolSize / splitUnitSize) * ((1 << splitUnitSize) - 1) +
                               ((1 << (outputSymbolSize % splitUnitSize)) - 1) + 1;
            } break;
        }

        // codingOrderCtxOffset[]
        codingOrderCtxOffset[0] = 0;
        if (codingOrder >= 1) codingOrderCtxOffset[1] = numCtxSubsym;
        if (codingOrder == 2) codingOrderCtxOffset[2] = numCtxSubsym * numAlphaSubsym;

        // codingSizeCtxOffset
        if (shareSubSymCtxFlag) {
            codingSizeCtxOffset = 0;
        } else if (codingOrder == 0) {
            codingSizeCtxOffset = numCtxSubsym;
        } else {
            codingSizeCtxOffset = codingOrderCtxOffset[codingOrder] * numAlphaSubsym;
        }

        // numCtxLuts
        numCtxLuts = 0;
        if (transform_ID_subsym == SupportValues::TransformIdSubsym::LUT_TRANSFORM) {
            if ((binarization_ID >= BinarizationParameters::BinarizationId::SUTU &&
                 binarization_ID <= BinarizationParameters::BinarizationId::SDTU) ||
                codingOrder == 0 || (1u << codingSubsymSize) > MAX_LUT_SIZE) {
                UTILS_THROW_RUNTIME_EXCEPTION(
                    "LUT_TRANSFORM not supported with given configuration: coding_order = 0, binarization_ID = " +
                    std::to_string((uint8_t)binarization_ID) +
                    ", coding_subsym_size = " + std::to_string(codingSubsymSize));
            } else {
                numCtxLuts = (codingSubsymSize / 2) * ((1 << 2) - 1) + ((1 << (codingSubsymSize % 2)) - 1);
            }
        }

        // numCtxTotal
        if (numContexts != 0) {
            numCtxTotal = numContexts;
        } else {
            numCtxTotal = numCtxLuts;
            uint32_t context_count = 1;
            if (!shareSubSymCtxFlag) {
                context_count = numSubsyms;
            }
            uint32_t context_size = numCtxSubsym;
            if (codingOrder > 0) {
                context_size = codingOrderCtxOffset[codingOrder] * numAlphaSubsym;
            }
            numCtxTotal += context_count * context_size;
        }
    }  // if (!cabac_binarization.getBypassFlag())
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t StateVars::getNumSubsymbols() const { return numSubsyms; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t StateVars::getNumAlphaSubsymbol() const { return numAlphaSubsym; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t StateVars::getNumCtxPerSubsymbol() const { return numCtxSubsym; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t StateVars::getCLengthBI() const { return cLengthBI; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t StateVars::getCodingOrderCtxOffset(uint8_t index) const { return codingOrderCtxOffset[index]; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t StateVars::getCodingSizeCtxOffset() const { return codingSizeCtxOffset; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t StateVars::getNumCtxLUTs() const { return numCtxLuts; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t StateVars::getNumCtxTotal() const { return numCtxTotal; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t StateVars::getMinimalSizeInBytes(uint8_t sizeInBits) { return (sizeInBits / 8) + ((sizeInBits % 8) ? 1 : 0); }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t StateVars::getLgWordSize(uint8_t sizeInBits) { return StateVars::getMinimalSizeInBytes(sizeInBits) >> 3; }

// ---------------------------------------------------------------------------------------------------------------------

int64_t StateVars::getSignedValue(uint64_t input, uint8_t sizeInBytes) {
    int64_t signedValue = 0;

    switch (sizeInBytes) {
        case 1:
            signedValue = static_cast<int64_t>(static_cast<int8_t>(input));
            break;
        case 2:
            signedValue = static_cast<int64_t>(static_cast<int16_t>(input));
            break;
        case 4:
            signedValue = static_cast<int64_t>(static_cast<int32_t>(input));
            break;
        case 8:
            signedValue = static_cast<int64_t>(input);
            break;
        default:
            UTILS_THROW_RUNTIME_EXCEPTION("Invalid size");
    }

    return signedValue;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t StateVars::get2PowN(uint8_t N) {
    assert(N <= 32);
    uint64_t one = 1u;
    return one << N;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
