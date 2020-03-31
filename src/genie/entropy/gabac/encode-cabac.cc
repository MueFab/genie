/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encode-cabac.h"

#include <cassert>

#include <genie/util/block-stepper.h>
#include <genie/util/data-block.h>
#include "constants.h"
#include "exceptions.h"
#include "writer.h"

#include "context-selector.h"
#include "luts-subsymbol-transform.h"

namespace genie {
namespace entropy {
namespace gabac {

static inline
void encode_sign_flag(Writer &writer,
                      const paramcabac::BinarizationParameters::BinarizationId binID,
                      const uint64_t symbolValue) {
    if(symbolValue != 0) {
        switch(binID) {
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
                writer.writeSignFlag(symbolValue);
                break;
            default:
                break;
        }
    }
}

void encode_cabac(const paramcabac::TransformedSeq &conf,
                  util::DataBlock* const symbols,
                  size_t maxSize) {
    util::DataBlock block(0, 1);

    assert(symbols != nullptr);

    size_t numSymbols = symbols->size();
    if (numSymbols <= 0) return;

    const paramcabac::SupportValues &supportVals = conf.getSupportValues();
    const paramcabac::Binarization &binarzation = conf.getBinarization();
    const paramcabac::BinarizationParameters &binarzationParams = binarzation.getCabacBinarizationParameters();
    const paramcabac::StateVars &stateVars = conf.getStateVars();
    const paramcabac::BinarizationParameters::BinarizationId binID = binarzation.getBinarizationID();

    const uint8_t outputSymbolSize = supportVals.getOutputSymbolSize();
    const uint8_t codingSubsymSize = supportVals.getCodingSubsymSize();
    const uint8_t codingOrder = supportVals.getCodingOrder();
    const uint64_t subsymMask = paramcabac::StateVars::get2PowN(codingSubsymSize)-1;
    const bool bypassFlag = binarzation.getBypassFlag();

    uint8_t const numLuts = stateVars.getNumLuts(codingOrder,
                                                 supportVals.getShareSubsymLutFlag());
    uint8_t const numPrvs = stateVars.getNumPrvs(codingOrder,
                                                 supportVals.getShareSubsymPrvFlag());

    OBufferStream bitstream(&block);
    Writer writer(&bitstream,
                  bypassFlag,
                  stateVars.getNumCtxTotal());
    writer.start(numSymbols);

    std::vector<unsigned int> binParams(3, 0);

    util::BlockStepper r = symbols->getReader();
    std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());
    LUTsSubSymbolTransformation lutsSubsymTrnsfm(supportVals, stateVars, true);
    if(numLuts > 0) {
        lutsSubsymTrnsfm.encodeLUTs(writer, symbols);
    }

    void (Writer::*func)(uint64_t, const std::vector<unsigned int>, const unsigned int) = nullptr;
    switch (binID) {
        case paramcabac::BinarizationParameters::BinarizationId::BINARY_CODING:
            func = &Writer::writeBI;
            binParams[0] = stateVars.getCLengthBI();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY:
            func = &Writer::writeTU;
            binParams[0] = binarzationParams.getCMax();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::EXPONENTIAL_GOLOMB:
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
            func = &Writer::writeEG;
            break;
        case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            func = &Writer::writeTEG;
            binParams[0] = binarzationParams.getCMaxTeg();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
            func = &Writer::writeSUTU;
            binParams[0] = outputSymbolSize;
            binParams[1] = binarzationParams.getSplitUnitSize();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::DOUBLE_TRUNCATED_UNARY:
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
            func = &Writer::writeDTU;
            binParams[0] = outputSymbolSize;
            binParams[1] = binarzationParams.getSplitUnitSize();
            binParams[2] = binarzationParams.getCMaxDtu();
            break;
        default:
            GABAC_DIE("Unknown Binarization");
    }

    while (r.isValid()) {
        if (maxSize <= bitstream.size()) {
            break;
        }

        // Split symbol into subsymbols and then encode subsymbols
        const uint64_t origSymbol = r.get();
        const uint64_t symbolValue = abs((int64_t) origSymbol);
        uint64_t subsymValToCode = 0;
        uint32_t oss = outputSymbolSize;
        for (uint8_t s=0; s<stateVars.getNumSubsymbols(); s++) {
            const uint8_t lutIdx = (numLuts > 1) ? s : 0; // either private or shared LUT
            const uint8_t prvIdx = (numPrvs > 1) ? s : 0; // either private or shared PRV

            subsymValToCode = subsymbols[s].subsymValue = (symbolValue>>(oss-=codingSubsymSize)) & subsymMask;
            subsymbols[s].subsymIdx = s;
            uint32_t ctxIdx = ContextSelector::getContextIdx(stateVars,
                                                             bypassFlag,
                                                             codingOrder,
                                                             s,
                                                             subsymbols,
                                                             prvIdx);

            if(numLuts > 0) {
                subsymbols[s].lutEntryIdx = 0;
                lutsSubsymTrnsfm.transform(subsymbols, s, lutIdx, prvIdx);
                subsymValToCode = subsymbols[s].lutEntryIdx;
                if(binID == paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY) {
                    binParams[0] = std::min((uint64_t) binarzationParams.getCMax(),subsymbols[s].lutNumMaxElems); // update cMax
                }
            }

            if(numPrvs > 0) {
                if(codingOrder == 2) {
                    subsymbols[prvIdx].prvValues[1] = subsymbols[prvIdx].prvValues[0];
                    subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;
                } else if(codingOrder == 1) {
                    subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;
                }
            }

            (writer.*func)(subsymValToCode, binParams, ctxIdx);
        }

        encode_sign_flag(writer, binID, origSymbol);

        r.inc();
    }

    writer.close();

    bitstream.flush(symbols);
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
