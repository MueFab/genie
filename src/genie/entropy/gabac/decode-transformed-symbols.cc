/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------
#include "decode-transformed-symbols.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

TransformedSymbolsDecoder::TransformedSymbolsDecoder(util::DataBlock *bitstream,
                                                     const paramcabac::TransformedSubSeq &trnsfSubseqConf,
                                                     const unsigned int numSymbols)
    : numEncodedSymbols(numSymbols), numDecodedSymbols(0),
      binID(trnsfSubseqConf.getBinarization().getBinarizationID()), stateVars(trnsfSubseqConf.getStateVars()),
      reader(nullptr), ctxSelector(nullptr), invLutsSubsymTrnsfm(nullptr), diffEnabled(false), customCmaxTU(false) {

    if (bitstream == nullptr || bitstream->size() <= 0) return; // FIXME Should die?
    if (numEncodedSymbols <= 0) return; // FIXME Should die?

    const paramcabac::SupportValues &supportVals = trnsfSubseqConf.getSupportValues();
    const paramcabac::Binarization &binarzation = trnsfSubseqConf.getBinarization();
    const paramcabac::BinarizationParameters &binarzationParams = binarzation.getCabacBinarizationParameters();

    outputSymbolSize = supportVals.getOutputSymbolSize();
    codingSubsymSize = supportVals.getCodingSubsymSize();
    codingOrder = supportVals.getCodingOrder();
    subsymMask = paramcabac::StateVars::get2PowN(codingSubsymSize) - 1;
    const bool bypassFlag = binarzation.getBypassFlag();

    numLuts = stateVars.getNumLuts(codingOrder, supportVals.getShareSubsymLutFlag(), trnsfSubseqConf.getTransformIDSubsym());
    numPrvs = stateVars.getNumPrvs(supportVals.getShareSubsymPrvFlag());

    reader = new Reader(bitstream, bypassFlag, stateVars.getNumCtxTotal());
    reader->start();

    defaultCmax = binarzationParams.getCMax();
    if (trnsfSubseqConf.getTransformIDSubsym() == paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM) {
        invLutsSubsymTrnsfm = new LUTsSubSymbolTransform(supportVals, stateVars, numLuts, numPrvs, false);

        if (numLuts > 0) {
            invLutsSubsymTrnsfm->decodeLUTs(*reader);
            if (binID == paramcabac::BinarizationParameters::BinarizationId::TU) {
                customCmaxTU = true;
            }
        }
    } else {
        diffEnabled =
            (trnsfSubseqConf.getTransformIDSubsym() == paramcabac::SupportValues::TransformIdSubsym::DIFF_CODING);
    }

    ctxSelector = new ContextSelector(stateVars);

    binParams = std::vector<unsigned int>(4,  // first three elements are for binarization params, last one is for ctxIdx
                                          0);
    binarizor = getBinarizorReader(outputSymbolSize, bypassFlag, binID, binarzationParams, stateVars, binParams);
}

// ---------------------------------------------------------------------------------------------------------------------

TransformedSymbolsDecoder::~TransformedSymbolsDecoder() {
    if (reader != nullptr) delete reader;
    if (ctxSelector != nullptr) delete ctxSelector;
    if (invLutsSubsymTrnsfm != nullptr) delete invLutsSubsymTrnsfm;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t TransformedSymbolsDecoder::decodeNextSymbol(uint64_t *depSymbol) {
    switch (codingOrder) {
        case 0:
            return decodeNextSymbolOrder0();
            break;
        case 1:
            return decodeNextSymbolOrder1(depSymbol);
            break;
        case 2:
            return decodeNextSymbolOrder2();
            break;
        default:
            UTILS_DIE("Unknown coding order");
    }

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t TransformedSymbolsDecoder::decodeNextSymbolOrder0() {
    uint64_t symbolValue = 0;
    if (numDecodedSymbols < numEncodedSymbols) {
        // Decode subsymbols and merge them to construct the symbol
        std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());
        for (uint8_t s = 0; s < stateVars.getNumSubsymbols(); s++) {
            subsymbols[s].subsymIdx = s;
            binParams[3] = ctxSelector->getContextIdxOrder0(s);

            subsymbols[s].subsymValue = (reader->*binarizor)(binParams);

            if (diffEnabled) {
                subsymbols[s].subsymValue += subsymbols[s].prvValues[0];
                subsymbols[s].prvValues[0] = subsymbols[s].subsymValue;
            }

            symbolValue = (symbolValue << codingSubsymSize) | subsymbols[s].subsymValue;
        }

        decodeSignFlag(*reader, binID, symbolValue);

        numDecodedSymbols++;
    }

    return symbolValue;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t TransformedSymbolsDecoder::decodeNextSymbolOrder1(uint64_t *depSymbol) {
    uint64_t symbolValue = 0;
    if (numDecodedSymbols < numEncodedSymbols) {
        // Decode subsymbols and merge them to construct the symbol
        uint64_t depSubsymValue = 0;

        uint32_t oss = outputSymbolSize;
        std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());
        for (uint8_t s = 0; s < stateVars.getNumSubsymbols(); s++) {
            const uint8_t lutIdx = (numLuts > 1) ? s : 0;  // either private or shared LUT
            const uint8_t prvIdx = (numPrvs > 1) ? s : 0;  // either private or shared PRV

            if (depSymbol) {
                depSubsymValue = (*depSymbol >> (oss -= codingSubsymSize)) & subsymMask;
                subsymbols[prvIdx].prvValues[0] = depSubsymValue;
            }

            subsymbols[s].subsymIdx = s;
            binParams[3] = ctxSelector->getContextIdxOrderGT0(s, prvIdx, subsymbols, codingOrder);

            if (customCmaxTU) {
                subsymbols[s].lutNumMaxElems = invLutsSubsymTrnsfm->getNumMaxElemsOrder1(subsymbols, lutIdx, prvIdx);
                binParams[0] =
                    std::min(defaultCmax, subsymbols[s].lutNumMaxElems);  // update cMax
            }
            subsymbols[s].subsymValue = (reader->*binarizor)(binParams);

            if (numLuts > 0) {
                subsymbols[s].lutEntryIdx = subsymbols[s].subsymValue;
                invLutsSubsymTrnsfm->invTransformOrder1(subsymbols, s, lutIdx, prvIdx);
            }

            subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;

            symbolValue = (symbolValue << codingSubsymSize) | subsymbols[s].subsymValue;
        }

        decodeSignFlag(*reader, binID, symbolValue);

        numDecodedSymbols++;
    }

    return symbolValue;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t TransformedSymbolsDecoder::decodeNextSymbolOrder2() {
    uint64_t symbolValue = 0;
    if (numDecodedSymbols < numEncodedSymbols) {
        // Decode subsymbols and merge them to construct the symbol
        std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());
        for (uint8_t s = 0; s < stateVars.getNumSubsymbols(); s++) {
            const uint8_t lutIdx = (numLuts > 1) ? s : 0;  // either private or shared LUT
            const uint8_t prvIdx = (numPrvs > 1) ? s : 0;  // either private or shared PRV

            subsymbols[s].subsymIdx = s;
            binParams[3] = ctxSelector->getContextIdxOrderGT0(s, prvIdx, subsymbols, codingOrder);

            if (customCmaxTU) {
                subsymbols[s].lutNumMaxElems = invLutsSubsymTrnsfm->getNumMaxElemsOrder2(subsymbols, lutIdx, prvIdx);
                binParams[0] =
                    std::min(defaultCmax, subsymbols[s].lutNumMaxElems);  // update cMax
            }
            subsymbols[s].subsymValue = (reader->*binarizor)(binParams);

            if (numLuts > 0) {
                subsymbols[s].lutEntryIdx = subsymbols[s].subsymValue;
                invLutsSubsymTrnsfm->invTransformOrder2(subsymbols, s, lutIdx, prvIdx);
            }

            subsymbols[prvIdx].prvValues[1] = subsymbols[prvIdx].prvValues[0];
            subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;

            symbolValue = (symbolValue << codingSubsymSize) | subsymbols[s].subsymValue;
        }

        decodeSignFlag(*reader, binID, symbolValue);

        numDecodedSymbols++;
    }

    return symbolValue;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t TransformedSymbolsDecoder::symbolsAvail() const {
    return numEncodedSymbols - numDecodedSymbols;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------