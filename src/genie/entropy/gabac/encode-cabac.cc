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

typedef void (Writer::*binFunc)(uint64_t, const std::vector<unsigned int>);

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

static inline
binFunc get_binarizor(const uint8_t outputSymbolSize,
                      const bool bypassFlag,
                      const paramcabac::BinarizationParameters::BinarizationId binID,
                      const paramcabac::BinarizationParameters &binarzationParams,
                      const paramcabac::StateVars &stateVars,
                      std::vector<unsigned int>& binParams) {
    binFunc func = nullptr;
    if(bypassFlag) {
        switch (binID) {
            case paramcabac::BinarizationParameters::BinarizationId::BINARY_CODING:
                func = &Writer::writeAsBIbypass;
                binParams[0] = stateVars.getCLengthBI();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY:
                func = &Writer::writeAsTUbypass;
                binParams[0] = binarzationParams.getCMax();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::EXPONENTIAL_GOLOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
                func = &Writer::writeAsEGbypass;
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
                func = &Writer::writeAsTEGbypass;
                binParams[0] = binarzationParams.getCMaxTeg();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
                func = &Writer::writeAsSUTUbypass;
                binParams[0] = outputSymbolSize;
                binParams[1] = binarzationParams.getSplitUnitSize();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::DOUBLE_TRUNCATED_UNARY:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
                func = &Writer::writeAsDTUbypass;
                binParams[0] = outputSymbolSize;
                binParams[1] = binarzationParams.getSplitUnitSize();
                binParams[2] = binarzationParams.getCMaxDtu();
                break;
            default:
                GABAC_DIE("Unknown Binarization");
        }
    } else {
        switch (binID) {
            case paramcabac::BinarizationParameters::BinarizationId::BINARY_CODING:
                func = &Writer::writeAsBIcabac;
                binParams[0] = stateVars.getCLengthBI();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY:
                func = &Writer::writeAsTUcabac;
                binParams[0] = binarzationParams.getCMax();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::EXPONENTIAL_GOLOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
                func = &Writer::writeAsEGcabac;
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
                func = &Writer::writeAsTEGcabac;
                binParams[0] = binarzationParams.getCMaxTeg();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
                func = &Writer::writeAsSUTUcabac;
                binParams[0] = outputSymbolSize;
                binParams[1] = binarzationParams.getSplitUnitSize();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::DOUBLE_TRUNCATED_UNARY:
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
                func = &Writer::writeAsDTUcabac;
                binParams[0] = outputSymbolSize;
                binParams[1] = binarzationParams.getSplitUnitSize();
                binParams[2] = binarzationParams.getCMaxDtu();
                break;
            default:
                GABAC_DIE("Unknown Binarization");
        }
    }

    return func;
}

void encode_cabac_order0(const paramcabac::TransformedSeq &conf,
                         util::DataBlock* symbols,
                         size_t maxSize) {
    assert(symbols != nullptr);

    size_t numSymbols = symbols->size();
    if (numSymbols <= 0) return;

    const paramcabac::SupportValues &supportVals = conf.getSupportValues();
    const paramcabac::Binarization &binarzation = conf.getBinarization();
    const paramcabac::BinarizationParameters &binarzationParams = binarzation.getCabacBinarizationParameters();
    const paramcabac::StateVars &stateVars = conf.getStateVars();
    const paramcabac::BinarizationParameters::BinarizationId binID = binarzation.getBinarizationID();
    const core::Alphabet alphaProps = getAlphabetProperties(conf.getAlphabetID());

    const uint8_t outputSymbolSize = supportVals.getOutputSymbolSize();
    const uint8_t codingSubsymSize = supportVals.getCodingSubsymSize();
    const uint64_t subsymMask = paramcabac::StateVars::get2PowN(codingSubsymSize)-1;
    const bool bypassFlag = binarzation.getBypassFlag();

    util::DataBlock block(0, 1);
    OBufferStream bitstream(&block);
    Writer writer(&bitstream,
                  bypassFlag,
                  stateVars.getNumCtxTotal());
    writer.start(numSymbols);

    std::vector<unsigned int> binParams(4, 0);  // first three elements are for binarization params, last one is for ctxIdx

    util::BlockStepper r = symbols->getReader();
    std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());

    binFunc func = get_binarizor(outputSymbolSize,
                                 bypassFlag,
                                 binID,
                                 binarzationParams,
                                 stateVars,
                                 binParams);

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

            subsymValToCode = subsymbols[s].subsymValue = (symbolValue>>(oss-=codingSubsymSize)) & subsymMask;
            subsymbols[s].subsymIdx = s;

            binParams[3] = ContextSelector::getContextIdx(stateVars,
                                                          bypassFlag,
                                                          0, // codingOrder
                                                          s,
                                                          subsymbols,
                                                          0); // prvIdx - N/A

            (writer.*func)(subsymValToCode, binParams);
        }

        encode_sign_flag(writer, binID, origSymbol);

        r.inc();
    }

    writer.close();

    bitstream.flush(symbols);
}

void encode_cabac_order1(const paramcabac::TransformedSeq &conf,
                         util::DataBlock* symbols,
                         util::DataBlock* const depSymbols,
                         size_t maxSize) {
    assert(symbols != nullptr);

    size_t numSymbols = symbols->size();
    if (numSymbols <= 0) return;

    const paramcabac::SupportValues &supportVals = conf.getSupportValues();
    const paramcabac::Binarization &binarzation = conf.getBinarization();
    const paramcabac::BinarizationParameters &binarzationParams = binarzation.getCabacBinarizationParameters();
    const paramcabac::StateVars &stateVars = conf.getStateVars();
    const paramcabac::BinarizationParameters::BinarizationId binID = binarzation.getBinarizationID();
    const core::Alphabet alphaProps = getAlphabetProperties(conf.getAlphabetID());

    const uint8_t outputSymbolSize = supportVals.getOutputSymbolSize();
    const uint8_t codingSubsymSize = supportVals.getCodingSubsymSize();
    const uint8_t codingOrder = supportVals.getCodingOrder();
    const uint64_t subsymMask = paramcabac::StateVars::get2PowN(codingSubsymSize)-1;
    const bool bypassFlag = binarzation.getBypassFlag();

    uint8_t const numLuts = stateVars.getNumLuts(codingOrder,
                                                 supportVals.getShareSubsymLutFlag(),
                                                 conf.getTransformIDSubsym());
    uint8_t const numPrvs = stateVars.getNumPrvs(codingOrder,
                                                 supportVals.getShareSubsymPrvFlag());

    util::DataBlock block(0, 1);
    OBufferStream bitstream(&block);
    Writer writer(&bitstream,
                  bypassFlag,
                  stateVars.getNumCtxTotal());
    writer.start(numSymbols);

    std::vector<unsigned int> binParams(4, 0); // first three elements are for binarization params, last one is for ctxIdx

    util::BlockStepper r = symbols->getReader();
    std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());

    LUTsSubSymbolTransform lutsSubsymTrnsfm(supportVals, stateVars, numLuts, numPrvs, true);
    if(numLuts > 0) {
        lutsSubsymTrnsfm.encodeLUTs(writer, alphaProps, symbols, depSymbols);
    }

    util::BlockStepper rDep;
    if(depSymbols) {
        rDep = depSymbols->getReader();
    }

    binFunc func = get_binarizor(outputSymbolSize,
                                 bypassFlag,
                                 binID,
                                 binarzationParams,
                                 stateVars,
                                 binParams);

    while (r.isValid()) {
        if (maxSize <= bitstream.size()) {
            break;
        }

        // Split symbol into subsymbols and then encode subsymbols
        const uint64_t origSymbol = r.get();
        const uint64_t symbolValue = abs((int64_t) origSymbol);
        uint64_t subsymValToCode = 0;

        uint64_t depSymbolValue = 0, depSubsymValue = 0;
        if(rDep.isValid()) {
            depSymbolValue = alphaProps.inverseLut[rDep.get()];
            rDep.inc();
        }

        uint32_t oss = outputSymbolSize;
        for (uint8_t s=0; s<stateVars.getNumSubsymbols(); s++) {
            const uint8_t lutIdx = (numLuts > 1) ? s : 0; // either private or shared LUT
            const uint8_t prvIdx = (numPrvs > 1) ? s : 0; // either private or shared PRV

            if(depSymbols) {
                depSubsymValue = (depSymbolValue>>(oss-codingSubsymSize)) & subsymMask;
                subsymbols[prvIdx].prvValues[0] = depSubsymValue;
            }

            subsymValToCode = subsymbols[s].subsymValue = (symbolValue>>(oss-=codingSubsymSize)) & subsymMask;
            subsymbols[s].subsymIdx = s;

            binParams[3] = ContextSelector::getContextIdx(stateVars,
                                                          bypassFlag,
                                                          codingOrder,
                                                          s,
                                                          subsymbols,
                                                          prvIdx);

            if(numLuts > 0) {
                subsymbols[s].lutEntryIdx = 0;
                lutsSubsymTrnsfm.transformOrder1(subsymbols, s, lutIdx, prvIdx);
                subsymValToCode = subsymbols[s].lutEntryIdx;
                if(binID == paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY) {
                    binParams[0] = std::min((uint64_t) binarzationParams.getCMax(),subsymbols[s].lutNumMaxElems); // update cMax
                }
            }

            (writer.*func)(subsymValToCode, binParams);

            subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;
        }

        encode_sign_flag(writer, binID, origSymbol);

        r.inc();
    }

    writer.close();

    bitstream.flush(symbols);
}

void encode_cabac_order2(const paramcabac::TransformedSeq &conf,
                         util::DataBlock* symbols,
                         size_t maxSize) {
    assert(symbols != nullptr);

    size_t numSymbols = symbols->size();
    if (numSymbols <= 0) return;

    const paramcabac::SupportValues &supportVals = conf.getSupportValues();
    const paramcabac::Binarization &binarzation = conf.getBinarization();
    const paramcabac::BinarizationParameters &binarzationParams = binarzation.getCabacBinarizationParameters();
    const paramcabac::StateVars &stateVars = conf.getStateVars();
    const paramcabac::BinarizationParameters::BinarizationId binID = binarzation.getBinarizationID();
    const core::Alphabet alphaProps = getAlphabetProperties(conf.getAlphabetID());

    const uint8_t outputSymbolSize = supportVals.getOutputSymbolSize();
    const uint8_t codingSubsymSize = supportVals.getCodingSubsymSize();
    const uint8_t codingOrder = supportVals.getCodingOrder();
    const uint64_t subsymMask = paramcabac::StateVars::get2PowN(codingSubsymSize)-1;
    const bool bypassFlag = binarzation.getBypassFlag();

    uint8_t const numLuts = stateVars.getNumLuts(codingOrder,
                                                 supportVals.getShareSubsymLutFlag(),
                                                 conf.getTransformIDSubsym());
    uint8_t const numPrvs = stateVars.getNumPrvs(codingOrder,
                                                 supportVals.getShareSubsymPrvFlag());

    util::DataBlock block(0, 1);
    OBufferStream bitstream(&block);
    Writer writer(&bitstream,
                  bypassFlag,
                  stateVars.getNumCtxTotal());
    writer.start(numSymbols);

    std::vector<unsigned int> binParams(4, 0); // first three elements are for binarization params, last one is for ctxIdx

    util::BlockStepper r = symbols->getReader();
    std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());

    LUTsSubSymbolTransform lutsSubsymTrnsfm(supportVals, stateVars, numLuts, numPrvs, true);
    if(numLuts > 0) {
        lutsSubsymTrnsfm.encodeLUTs(writer, alphaProps, symbols, nullptr);
    }

    binFunc func = get_binarizor(outputSymbolSize,
                                 bypassFlag,
                                 binID,
                                 binarzationParams,
                                 stateVars,
                                 binParams);

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

            binParams[3] = ContextSelector::getContextIdx(stateVars,
                                                          bypassFlag,
                                                          codingOrder,
                                                          s,
                                                          subsymbols,
                                                          prvIdx);

            if(numLuts > 0) {
                subsymbols[s].lutEntryIdx = 0;
                lutsSubsymTrnsfm.transformOrder2(subsymbols, s, lutIdx, prvIdx);
                subsymValToCode = subsymbols[s].lutEntryIdx;
                if(binID == paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY) {
                    binParams[0] = std::min((uint64_t) binarzationParams.getCMax(),subsymbols[s].lutNumMaxElems); // update cMax
                }
            }

            (writer.*func)(subsymValToCode, binParams);

            subsymbols[prvIdx].prvValues[1] = subsymbols[prvIdx].prvValues[0];
            subsymbols[prvIdx].prvValues[0] = subsymbols[s].subsymValue;
        }

        encode_sign_flag(writer, binID, origSymbol);

        r.inc();
    }

    writer.close();

    bitstream.flush(symbols);
}

void encode_cabac(const paramcabac::TransformedSeq &conf,
                  util::DataBlock* symbols,
                  util::DataBlock* const depSymbols,
                  size_t maxSize) {

    switch(conf.getSupportValues().getCodingOrder()) {
        case 0:
            encode_cabac_order0(conf, symbols, maxSize);
        break;
        case 1:
            encode_cabac_order1(conf, symbols, depSymbols, maxSize);
        break;
        case 2:
            encode_cabac_order2(conf, symbols, maxSize);
        break;
        default:
            GABAC_DIE("Unknown coding order");
    }
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
