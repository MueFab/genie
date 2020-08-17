/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decode-cabac.h"

#include <cassert>
#include <limits>

#include <genie/entropy/paramcabac/subsequence.h>
#include <genie/util/data-block.h>
#include "reader.h"

#include "context-selector.h"
#include "luts-subsymbol-transform.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

typedef uint64_t (Reader::*binFunc)(const std::vector<unsigned int>);

// ---------------------------------------------------------------------------------------------------------------------

static inline void decodeSignFlag(Reader &reader, const paramcabac::BinarizationParameters::BinarizationId binID,
                                  uint64_t &symbolValue) {
    if (symbolValue != 0) {
        switch (binID) {
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
                if (reader.readSignFlag()) {
                    int64_t symbolValueSigned = -(symbolValue);
                    symbolValue = static_cast<uint64_t>(symbolValueSigned);
                }
                break;
            default:
                break;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

static inline binFunc getBinarizor(const uint8_t outputSymbolSize, const bool bypassFlag,
                                   const paramcabac::BinarizationParameters::BinarizationId binID,
                                   const paramcabac::BinarizationParameters &binarzationParams,
                                   const paramcabac::StateVars &stateVars, std::vector<unsigned int> &binParams) {
    binFunc func = nullptr;
    if (bypassFlag) {
        switch (binID) {
            case paramcabac::BinarizationParameters::BinarizationId::BINARY_CODING:
                func = &Reader::readAsBIbypass;
                binParams[0] = stateVars.getCLengthBI();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY:
                func = &Reader::readAsTUbypass;
                binParams[0] = binarzationParams.getCMax();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::EXPONENTIAL_GOLOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
                func = &Reader::readAsEGbypass;
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
                func = &Reader::readAsTEGbypass;
                binParams[0] = binarzationParams.getCMaxTeg();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
                func = &Reader::readAsSUTUbypass;
                binParams[0] = outputSymbolSize;
                binParams[1] = binarzationParams.getSplitUnitSize();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::DOUBLE_TRUNCATED_UNARY:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
                func = &Reader::readAsDTUbypass;
                binParams[0] = outputSymbolSize;
                binParams[1] = binarzationParams.getSplitUnitSize();
                binParams[2] = binarzationParams.getCMaxDtu();
                break;
            default:
                UTILS_DIE("Unknown Binarization");
        }
    } else {
        switch (binID) {
            case paramcabac::BinarizationParameters::BinarizationId::BINARY_CODING:
                func = &Reader::readAsBIcabac;
                binParams[0] = stateVars.getCLengthBI();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY:
                func = &Reader::readAsTUcabac;
                binParams[0] = binarzationParams.getCMax();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::EXPONENTIAL_GOLOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
                func = &Reader::readAsEGcabac;
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
                func = &Reader::readAsTEGcabac;
                binParams[0] = binarzationParams.getCMaxTeg();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
                func = &Reader::readAsSUTUcabac;
                binParams[0] = outputSymbolSize;
                binParams[1] = binarzationParams.getSplitUnitSize();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::DOUBLE_TRUNCATED_UNARY:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
                func = &Reader::readAsDTUcabac;
                binParams[0] = outputSymbolSize;
                binParams[1] = binarzationParams.getSplitUnitSize();
                binParams[2] = binarzationParams.getCMaxDtu();
                break;
            default:
                UTILS_DIE("Unknown Binarization");
        }
    }

    return func;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t decodeTransformSubseqOrder0(const paramcabac::TransformedSubSeq &trnsfSubseqConf,
                                   const unsigned int numEncodedSymbols, util::DataBlock *bitstream) {
    if (bitstream == nullptr) {
        UTILS_DIE("Bitstream is null");
    }

    if (numEncodedSymbols <= 0) return 0;

    const paramcabac::SupportValues &supportVals = trnsfSubseqConf.getSupportValues();
    const paramcabac::Binarization &binarzation = trnsfSubseqConf.getBinarization();
    const paramcabac::BinarizationParameters &binarzationParams = binarzation.getCabacBinarizationParameters();
    const paramcabac::StateVars &stateVars = trnsfSubseqConf.getStateVars();
    const paramcabac::BinarizationParameters::BinarizationId binID = binarzation.getBinarizationID();

    const uint8_t outputSymbolSize = supportVals.getOutputSymbolSize();
    const uint8_t codingSubsymSize = supportVals.getCodingSubsymSize();
    const bool bypassFlag = binarzation.getBypassFlag();
    size_t payloadSizeUsed = 0;

    Reader reader(bitstream, bypassFlag, stateVars.getNumCtxTotal());
    reader.start();

    std::vector<unsigned int> binParams(4,
                                        0);  // first three elements are for binarization params, last one is for ctxIdx

    util::DataBlock decodedSymbols(numEncodedSymbols, 4);
    util::BlockStepper r = decodedSymbols.getReader();
    std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());

    ContextSelector ctxSelector(stateVars);
    const bool diffEnabled =
        (trnsfSubseqConf.getTransformIDSubsym() == paramcabac::SupportValues::TransformIdSubsym::DIFF_CODING);

    binFunc func = getBinarizor(outputSymbolSize, bypassFlag, binID, binarzationParams, stateVars, binParams);

    while (r.isValid()) {
        // Decode subsymbols and merge them to construct symbols
        uint64_t symbolValue = 0;

        for (uint8_t s = 0; s < stateVars.getNumSubsymbols(); s++) {
            subsymbols[s].subsymIdx = s;
            binParams[3] = ctxSelector.getContextIdxOrder0(s);

            subsymbols[s].subsymValue = (reader.*func)(binParams);

            if (diffEnabled) {
                subsymbols[s].subsymValue += subsymbols[s].prvValues[0];
                subsymbols[s].prvValues[0] = subsymbols[s].subsymValue;
            }

            symbolValue = (symbolValue << codingSubsymSize) | subsymbols[s].subsymValue;
        }

        decodeSignFlag(reader, binID, symbolValue);

        r.set(symbolValue);
        r.inc();
    }

    payloadSizeUsed = reader.close();

    decodedSymbols.swap(bitstream);

    return payloadSizeUsed;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t decodeTransformSubseqOrder1(const paramcabac::TransformedSubSeq &trnsfSubseqConf,
                                   const unsigned int numEncodedSymbols, util::DataBlock *bitstream,
                                   util::DataBlock *const depSymbols) {
    if (bitstream == nullptr) {
        UTILS_DIE("Bitstream is null");
    }

    if (numEncodedSymbols <= 0) return 0;

    const paramcabac::SupportValues &supportVals = trnsfSubseqConf.getSupportValues();
    const paramcabac::Binarization &binarzation = trnsfSubseqConf.getBinarization();
    const paramcabac::BinarizationParameters &binarzationParams = binarzation.getCabacBinarizationParameters();
    const paramcabac::StateVars &stateVars = trnsfSubseqConf.getStateVars();
    const paramcabac::BinarizationParameters::BinarizationId binID = binarzation.getBinarizationID();
    const core::Alphabet alphaProps = getAlphabetProperties(trnsfSubseqConf.getAlphabetID());

    const uint8_t outputSymbolSize = supportVals.getOutputSymbolSize();
    const uint8_t codingSubsymSize = supportVals.getCodingSubsymSize();
    const uint8_t codingOrder = supportVals.getCodingOrder();
    const uint64_t subsymMask = paramcabac::StateVars::get2PowN(codingSubsymSize) - 1;
    const bool bypassFlag = binarzation.getBypassFlag();
    size_t payloadSizeUsed = 0;

    uint8_t const numLuts =
        stateVars.getNumLuts(codingOrder, supportVals.getShareSubsymLutFlag(), trnsfSubseqConf.getTransformIDSubsym());
    uint8_t const numPrvs = stateVars.getNumPrvs(supportVals.getShareSubsymPrvFlag());

    Reader reader(bitstream, bypassFlag, stateVars.getNumCtxTotal());
    reader.start();

    std::vector<unsigned int> binParams(4,
                                        0);  // first three elements are for binarization params, last one is for ctxIdx

    util::DataBlock decodedSymbols(numEncodedSymbols, 4);
    util::BlockStepper r = decodedSymbols.getReader();
    std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());

    LUTsSubSymbolTransform invLutsSubsymTrnsfm(supportVals, stateVars, numLuts, numPrvs, false);
    bool customCmaxTU = false;
    if (numLuts > 0) {
        invLutsSubsymTrnsfm.decodeLUTs(reader);
        if (binID == paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY) {
            customCmaxTU = true;
        }
    }

    util::BlockStepper rDep;
    if (depSymbols) {
        rDep = depSymbols->getReader();
    }

    ContextSelector ctxSelector(stateVars);

    binFunc func = getBinarizor(outputSymbolSize, bypassFlag, binID, binarzationParams, stateVars, binParams);

    while (r.isValid()) {
        // Decode subsymbols and merge them to construct symbols
        uint64_t symbolValue = 0;

        uint64_t depSymbolValue = 0, depSubsymValue = 0;
        if (rDep.isValid()) {
            depSymbolValue = alphaProps.inverseLut[rDep.get()];
            rDep.inc();
        }

        uint32_t oss = outputSymbolSize;
        for (uint8_t s = 0; s < stateVars.getNumSubsymbols(); s++) {
            const uint8_t lutIdx = (numLuts > 1) ? s : 0;  // either private or shared LUT
            const uint8_t prvIdx = (numPrvs > 1) ? s : 0;  // either private or shared PRV

            if (depSymbols) {
                depSubsymValue = (depSymbolValue >> (oss -= codingSubsymSize)) & subsymMask;
                subsymbols[prvIdx].prvValues[0] = depSubsymValue;
            }

            subsymbols[s].subsymIdx = s;
            binParams[3] = ctxSelector.getContextIdxOrderGT0(s, prvIdx, subsymbols, codingOrder);

            if (customCmaxTU) {
                subsymbols[s].lutNumMaxElems = invLutsSubsymTrnsfm.getNumMaxElemsOrder1(subsymbols, lutIdx, prvIdx);
                binParams[0] =
                    std::min((uint64_t)binarzationParams.getCMax(), subsymbols[s].lutNumMaxElems);  // update cMax
            }
            subsymbols[s].subsymValue = (reader.*func)(binParams);

            if (numLuts > 0) {
                subsymbols[s].lutEntryIdx = subsymbols[s].subsymValue;
                invLutsSubsymTrnsfm.invTransformOrder1(subsymbols, s, lutIdx, prvIdx);
            }

            subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;

            symbolValue = (symbolValue << codingSubsymSize) | subsymbols[s].subsymValue;
        }

        decodeSignFlag(reader, binID, symbolValue);

        r.set(symbolValue);
        r.inc();
    }

    payloadSizeUsed = reader.close();

    decodedSymbols.swap(bitstream);

    return payloadSizeUsed;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t decodeTransformSubseqOrder2(const paramcabac::TransformedSubSeq &trnsfSubseqConf,
                                   const unsigned int numEncodedSymbols, util::DataBlock *bitstream) {
    if (bitstream == nullptr) {
        UTILS_DIE("Bitstream is null");
    }

    if (numEncodedSymbols <= 0) return 0;

    const paramcabac::SupportValues &supportVals = trnsfSubseqConf.getSupportValues();
    const paramcabac::Binarization &binarzation = trnsfSubseqConf.getBinarization();
    const paramcabac::BinarizationParameters &binarzationParams = binarzation.getCabacBinarizationParameters();
    const paramcabac::StateVars &stateVars = trnsfSubseqConf.getStateVars();
    const paramcabac::BinarizationParameters::BinarizationId binID = binarzation.getBinarizationID();

    const uint8_t outputSymbolSize = supportVals.getOutputSymbolSize();
    const uint8_t codingSubsymSize = supportVals.getCodingSubsymSize();
    const uint8_t codingOrder = supportVals.getCodingOrder();
    const bool bypassFlag = binarzation.getBypassFlag();

    uint8_t const numLuts =
        stateVars.getNumLuts(codingOrder, supportVals.getShareSubsymLutFlag(), trnsfSubseqConf.getTransformIDSubsym());
    uint8_t const numPrvs = stateVars.getNumPrvs(supportVals.getShareSubsymPrvFlag());
    size_t payloadSizeUsed = 0;

    Reader reader(bitstream, bypassFlag, stateVars.getNumCtxTotal());
    reader.start();

    std::vector<unsigned int> binParams(4,
                                        0);  // first three elements are for binarization params, last one is for ctxIdx

    util::DataBlock decodedSymbols(numEncodedSymbols, 4);
    util::BlockStepper r = decodedSymbols.getReader();
    std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());

    LUTsSubSymbolTransform invLutsSubsymTrnsfm(supportVals, stateVars, numLuts, numPrvs, false);
    bool customCmaxTU = false;
    if (numLuts > 0) {
        invLutsSubsymTrnsfm.decodeLUTs(reader);
        if (binID == paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY) {
            customCmaxTU = true;
        }
    }

    ContextSelector ctxSelector(stateVars);

    binFunc func = getBinarizor(outputSymbolSize, bypassFlag, binID, binarzationParams, stateVars, binParams);

    while (r.isValid()) {
        // Decode subsymbols and merge them to construct symbols
        uint64_t symbolValue = 0;

        for (uint8_t s = 0; s < stateVars.getNumSubsymbols(); s++) {
            const uint8_t lutIdx = (numLuts > 1) ? s : 0;  // either private or shared LUT
            const uint8_t prvIdx = (numPrvs > 1) ? s : 0;  // either private or shared PRV

            subsymbols[s].subsymIdx = s;
            binParams[3] = ctxSelector.getContextIdxOrderGT0(s, prvIdx, subsymbols, codingOrder);

            if (customCmaxTU) {
                subsymbols[s].lutNumMaxElems = invLutsSubsymTrnsfm.getNumMaxElemsOrder2(subsymbols, lutIdx, prvIdx);
                binParams[0] =
                    std::min((uint64_t)binarzationParams.getCMax(), subsymbols[s].lutNumMaxElems);  // update cMax
            }
            subsymbols[s].subsymValue = (reader.*func)(binParams);

            if (numLuts > 0) {
                subsymbols[s].lutEntryIdx = subsymbols[s].subsymValue;
                invLutsSubsymTrnsfm.invTransformOrder2(subsymbols, s, lutIdx, prvIdx);
            }

            subsymbols[prvIdx].prvValues[1] = subsymbols[prvIdx].prvValues[0];
            subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;

            symbolValue = (symbolValue << codingSubsymSize) | subsymbols[s].subsymValue;
        }

        decodeSignFlag(reader, binID, symbolValue);

        r.set(symbolValue);
        r.inc();
    }

    payloadSizeUsed = reader.close();

    decodedSymbols.swap(bitstream);

    return payloadSizeUsed;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t decodeTransformSubseq(const paramcabac::TransformedSubSeq &trnsfSubseqConf, const unsigned int numEncodedSymbols,
                             util::DataBlock *bitstream, util::DataBlock *const depSymbols) {
    switch (trnsfSubseqConf.getSupportValues().getCodingOrder()) {
        case 0:
            return decodeTransformSubseqOrder0(trnsfSubseqConf, numEncodedSymbols, bitstream);
            break;
        case 1:
            return decodeTransformSubseqOrder1(trnsfSubseqConf, numEncodedSymbols, bitstream, depSymbols);
            break;
        case 2:
            return decodeTransformSubseqOrder2(trnsfSubseqConf, numEncodedSymbols, bitstream);
            break;
        default:
            UTILS_DIE("Unknown coding order");
    }

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------