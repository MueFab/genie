/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/entropy/gabac/decode-transformed-symbols.h"
#include <algorithm>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

TransformedSymbolsDecoder::TransformedSymbolsDecoder(util::DataBlock *bitstream,
                                                     const paramcabac::TransformedSubSeq &trnsfSubseqConf,
                                                     const unsigned int numSymbols)
    : numEncodedSymbols(numSymbols),
      numDecodedSymbols(0),
      outputSymbolSize(trnsfSubseqConf.getSupportValues().getOutputSymbolSize()),
      codingSubsymSize(trnsfSubseqConf.getSupportValues().getCodingSubsymSize()),
      codingOrder(trnsfSubseqConf.getSupportValues().getCodingOrder()),
      subsymMask(paramcabac::StateVars::get2PowN(codingSubsymSize) - 1),
      numSubSyms((uint8_t)trnsfSubseqConf.getStateVars().getNumSubsymbols()),
      numLuts(trnsfSubseqConf.getStateVars().getNumLuts(codingOrder,
                                                        trnsfSubseqConf.getSupportValues().getShareSubsymLutFlag(),
                                                        trnsfSubseqConf.getTransformIDSubsym())),
      numPrvs(trnsfSubseqConf.getStateVars().getNumPrvs(trnsfSubseqConf.getSupportValues().getShareSubsymPrvFlag())),
      reader(bitstream, trnsfSubseqConf.getBinarization().getBypassFlag(),
             (unsigned int)trnsfSubseqConf.getStateVars().getNumCtxTotal()),
      ctxSelector(trnsfSubseqConf.getStateVars()),
      invLutsSubsymTrnsfm(trnsfSubseqConf.getSupportValues(), trnsfSubseqConf.getStateVars(), numLuts, numPrvs, false),
      diffEnabled(trnsfSubseqConf.getTransformIDSubsym() == paramcabac::SupportValues::TransformIdSubsym::DIFF_CODING),
      customCmaxTU(false),
      defaultCmax(trnsfSubseqConf.getBinarization().getCabacBinarizationParameters().getCMax()),
      binID(trnsfSubseqConf.getBinarization().getBinarizationID()),
      binParams(
          std::vector<unsigned int>(4, 0)),  // first three elements are for binarization params, last one is for ctxIdx
      binarizor(getBinarizorReader(outputSymbolSize, trnsfSubseqConf.getBinarization().getBypassFlag(), binID,
                                   trnsfSubseqConf.getBinarization().getCabacBinarizationParameters(),
                                   trnsfSubseqConf.getStateVars(), binParams)) {
    if (bitstream == nullptr || bitstream->empty()) return;  // Simple return as bitstream can be empty
    if (numEncodedSymbols <= 0) return;                      // Simple return as numEncodedSymbols can be zero

    reader.start();

    if (numLuts > 0) {
        invLutsSubsymTrnsfm.decodeLUTs(reader);
        if (binID == paramcabac::BinarizationParameters::BinarizationId::TU) {
            customCmaxTU = true;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TransformedSymbolsDecoder::TransformedSymbolsDecoder(const TransformedSymbolsDecoder &src) = default;

// ---------------------------------------------------------------------------------------------------------------------

uint64_t TransformedSymbolsDecoder::decodeNextSymbol(uint64_t *depSymbol) {
    if (symbolsAvail() <= 0) UTILS_DIE("No more transformed symbols available.");

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
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t TransformedSymbolsDecoder::decodeNextSymbolOrder0() {
    uint64_t symbolValue = 0;
    if (numDecodedSymbols < numEncodedSymbols) {
        // Decode subsymbols and merge them to construct the symbol
        std::vector<Subsymbol> subsymbols(numSubSyms);
        for (uint8_t s = 0; s < numSubSyms; s++) {
            subsymbols[s].subsymIdx = s;
            binParams[3] = ctxSelector.getContextIdxOrder0(s);

            subsymbols[s].subsymValue = (reader.*binarizor)(binParams);

            if (diffEnabled) {
                subsymbols[s].subsymValue += subsymbols[s].prvValues[0];
                subsymbols[s].prvValues[0] = subsymbols[s].subsymValue;
            }

            symbolValue = (symbolValue << codingSubsymSize) | subsymbols[s].subsymValue;
        }

        decodeSignFlag(reader, binID, symbolValue);

        numDecodedSymbols++;
    }

    return symbolValue;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t TransformedSymbolsDecoder::decodeNextSymbolOrder1(const uint64_t *depSymbol) {
    uint64_t symbolValue = 0;
    if (numDecodedSymbols < numEncodedSymbols) {
        // Decode subsymbols and merge them to construct the symbol
        uint64_t depSubsymValue = 0;

        uint32_t oss = outputSymbolSize;
        std::vector<Subsymbol> subsymbols(numSubSyms);
        for (uint8_t s = 0; s < numSubSyms; s++) {
            const uint8_t lutIdx = (numLuts > 1) ? s : 0;  // either private or shared LUT
            const uint8_t prvIdx = (numPrvs > 1) ? s : 0;  // either private or shared PRV

            if (depSymbol) {
                depSubsymValue = (*depSymbol >> (oss -= codingSubsymSize)) & subsymMask;
                subsymbols[prvIdx].prvValues[0] = depSubsymValue;
            }

            subsymbols[s].subsymIdx = s;
            binParams[3] = ctxSelector.getContextIdxOrderGT0(s, prvIdx, subsymbols, codingOrder);

            if (customCmaxTU) {
                subsymbols[s].lutNumMaxElems = invLutsSubsymTrnsfm.getNumMaxElemsOrder1(subsymbols, lutIdx, prvIdx);
                binParams[0] = (unsigned int)std::min(defaultCmax, subsymbols[s].lutNumMaxElems);  // update cMax
            }
            subsymbols[s].subsymValue = (reader.*binarizor)(binParams);

            if (numLuts > 0) {
                subsymbols[s].lutEntryIdx = subsymbols[s].subsymValue;
                invLutsSubsymTrnsfm.invTransformOrder1(subsymbols, s, lutIdx, prvIdx);
            }

            subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;

            symbolValue = (symbolValue << codingSubsymSize) | subsymbols[s].subsymValue;
        }

        decodeSignFlag(reader, binID, symbolValue);

        numDecodedSymbols++;
    }

    return symbolValue;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t TransformedSymbolsDecoder::decodeNextSymbolOrder2() {
    uint64_t symbolValue = 0;
    if (numDecodedSymbols < numEncodedSymbols) {
        // Decode subsymbols and merge them to construct the symbol
        std::vector<Subsymbol> subsymbols(numSubSyms);
        for (uint8_t s = 0; s < numSubSyms; s++) {
            const uint8_t lutIdx = (numLuts > 1) ? s : 0;  // either private or shared LUT
            const uint8_t prvIdx = (numPrvs > 1) ? s : 0;  // either private or shared PRV

            subsymbols[s].subsymIdx = s;
            binParams[3] = ctxSelector.getContextIdxOrderGT0(s, prvIdx, subsymbols, codingOrder);

            if (customCmaxTU) {
                subsymbols[s].lutNumMaxElems = invLutsSubsymTrnsfm.getNumMaxElemsOrder2(subsymbols, lutIdx, prvIdx);
                binParams[0] = (unsigned int)std::min(defaultCmax, subsymbols[s].lutNumMaxElems);  // update cMax
            }
            subsymbols[s].subsymValue = (reader.*binarizor)(binParams);

            if (numLuts > 0) {
                subsymbols[s].lutEntryIdx = subsymbols[s].subsymValue;
                invLutsSubsymTrnsfm.invTransformOrder2(subsymbols, s, lutIdx, prvIdx);
            }

            subsymbols[prvIdx].prvValues[1] = subsymbols[prvIdx].prvValues[0];
            subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;

            symbolValue = (symbolValue << codingSubsymSize) | subsymbols[s].subsymValue;
        }

        decodeSignFlag(reader, binID, symbolValue);

        numDecodedSymbols++;
    }

    return symbolValue;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t TransformedSymbolsDecoder::symbolsAvail() const { return numEncodedSymbols - numDecodedSymbols; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
