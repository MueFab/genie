/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decode-cabac.h"

#include <cassert>
#include <limits>

#include <genie/util/data-block.h>
#include "constants.h"
#include "exceptions.h"
#include "reader.h"

#include "context-selector.h"
#include "luts-subsymbol-transform.h"

namespace genie {
namespace entropy {
namespace gabac {

static inline
void decode_sign_flag(Reader &reader,
                      const paramcabac::BinarizationParameters::BinarizationId binID,
                      uint64_t &symbolValue) {
    if(symbolValue != 0) {
        switch(binID) {
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
                if(reader.readSignFlag()) {
                    int64_t symbolValueSigned = -(symbolValue);
                    symbolValue = (uint64_t) symbolValueSigned;
                }
                break;
            default:
                break;
        }
    }
}

void decode_cabac(const paramcabac::TransformedSeq &conf,
                  util::DataBlock* const bitstream) {
    if (bitstream == nullptr) {
        GABAC_DIE("Bitstream is null");
    }

    util::DataBlock symbols(0,
                            4);
                            //conf.getSupportValues().getMinimalSizeInBytes(conf.getSupportValues().getOutputSymbolSize()));

    const paramcabac::SupportValues &supportVals = conf.getSupportValues();
    const paramcabac::Binarization &binarzation = conf.getBinarization();
    const paramcabac::BinarizationParameters &binarzationParams = binarzation.getCabacBinarizationParameters();
    const paramcabac::StateVars &stateVars = conf.getStateVars();
    const paramcabac::BinarizationParameters::BinarizationId binID = binarzation.getBinarizationID();

    const uint8_t outputSymbolSize = supportVals.getOutputSymbolSize();
    const uint8_t codingSubsymSize = supportVals.getCodingSubsymSize();
    const uint8_t codingOrder = supportVals.getCodingOrder();
    const bool bypassFlag = binarzation.getBypassFlag();

    uint8_t const numLuts = stateVars.getNumLuts(codingOrder,
                                                 supportVals.getShareSubsymLutFlag());
    uint8_t const numPrvs = stateVars.getNumPrvs(codingOrder,
                                                 supportVals.getShareSubsymPrvFlag());

    Reader reader(bitstream,
                  bypassFlag,
                  stateVars.getNumCtxTotal());
    size_t numSymbols = reader.start();
    if (numSymbols <= 0) return;

    // symbols->clear();
    symbols.resize(numSymbols);

    std::vector<unsigned int> binParams(3, 0);
    util::BlockStepper r = symbols.getReader();
    std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());
    LUTsSubSymbolTransform invLutsSubsymTrnsfm(supportVals, stateVars, false);
    if(numLuts > 0) {
        invLutsSubsymTrnsfm.decodeLUTs(reader);
    }

    uint64_t (Reader::*func)(const std::vector<unsigned int>, const unsigned int) = nullptr;
    switch (binID) {
        case paramcabac::BinarizationParameters::BinarizationId::BINARY_CODING:
            func = &Reader::readBI;
            binParams[0] = stateVars.getCLengthBI();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY:
            func = &Reader::readTU;
            binParams[0] = binarzationParams.getCMax();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::EXPONENTIAL_GOLOMB:
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
            func = &Reader::readEG;
            break;
        case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            func = &Reader::readTEG;
            binParams[0] = binarzationParams.getCMaxTeg();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
            func = &Reader::readSUTU;
            binParams[0] = outputSymbolSize;
            binParams[1] = binarzationParams.getSplitUnitSize();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::DOUBLE_TRUNCATED_UNARY:
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
            func = &Reader::readDTU;
            binParams[0] = outputSymbolSize;
            binParams[1] = binarzationParams.getSplitUnitSize();
            binParams[2] = binarzationParams.getCMaxDtu();
            break;
        default:
            GABAC_DIE("Invalid binarization");
    }

    while (r.isValid()) {
        // Decode subsymbols and merge them to construct symbols
        uint64_t symbolValue = 0;
        uint64_t decodedSubsym = 0;
        for (uint8_t s=0; s<stateVars.getNumSubsymbols(); s++) {
            const uint8_t lutIdx = (numLuts > 1) ? s : 0; // either private or shared LUT
            const uint8_t prvIdx = (numPrvs > 1) ? s : 0; // either private or shared PRV

            subsymbols[s].subsymIdx = s;
            uint32_t ctxIdx = ContextSelector::getContextIdx(stateVars,
                                                             bypassFlag,
                                                             codingOrder,
                                                             s,
                                                             subsymbols,
                                                             prvIdx);
            if(numLuts > 0) {
                if(binID == paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY) {
                    subsymbols[s].lutNumMaxElems = invLutsSubsymTrnsfm.getNumMaxElems(subsymbols,
                                                                                      lutIdx,
                                                                                      prvIdx);
                    binParams[0] = std::min((uint64_t) binarzationParams.getCMax(),subsymbols[s].lutNumMaxElems); // update cMax
                }
            }
            subsymbols[s].subsymValue = decodedSubsym = (reader.*func)(binParams, ctxIdx);

            if(numLuts > 0) {
                subsymbols[s].lutEntryIdx = decodedSubsym;
                invLutsSubsymTrnsfm.invTransform(subsymbols, s, lutIdx, prvIdx);
            }

            if(numPrvs > 0) {
                if(codingOrder == 2) {
                    subsymbols[prvIdx].prvValues[1] = subsymbols[prvIdx].prvValues[0];
                    subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;
                } else if(codingOrder == 1) {
                    subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;
                }
            }

            symbolValue = (symbolValue<<codingSubsymSize) | subsymbols[s].subsymValue;
        }

        decode_sign_flag(reader, binID, symbolValue);

        r.set(symbolValue);
        r.inc();
    }

    reader.close();

    symbols.swap(bitstream);
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
