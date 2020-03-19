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

#include <genie/entropy/paramcabac/transformed-seq.h>

namespace genie {
namespace entropy {
namespace gabac {

void decode_cabac(const paramcabac::TransformedSeq &conf,
                  util::DataBlock* const bitstream) {
    if (bitstream == nullptr) {
        GABAC_DIE("Bitstream is null");
    }

    util::DataBlock symbols(0, conf.getSupportValues().getMinimalSizeInBytes(conf.getSupportValues().getOutputSymbolSize()));

    const paramcabac::SupportValues &supportVals = conf.getSupportValues();
    const paramcabac::Binarization &binarzation = conf.getBinarization();
    const paramcabac::BinarizationParameters &binarzationParams = binarzation.getCabacBinarizationParameters();
    const paramcabac::StateVars &stateVars = conf.getStateVars();

    Reader reader(bitstream,
                  binarzation.getBypassFlag(),
                  stateVars.getNumCtxTotal());
    size_t numSymbols = reader.start();
    if (numSymbols <= 0) return;

    // symbols->clear();
    symbols.resize(numSymbols);

    unsigned int binarizationParameter = 0;
    util::BlockStepper r = symbols.getReader();

    if (binarzation.getBypassFlag()) { // bypass mode
        uint64_t (Reader::*func)(unsigned int) = nullptr;
        switch (binarzation.getBinarizationID()) {
            case paramcabac::BinarizationParameters::BinarizationId::BINARY_CODING:
                func = &Reader::readAsBIbypass;
                binarizationParameter = stateVars.getCLengthBI();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY:
                func = &Reader::readAsTUbypass;
                binarizationParameter = binarzationParams.getCMax();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::EXPONENTIAL_GOLOMB:
                func = &Reader::readAsEGbypass;
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
                func = &Reader::readAsSEGbypass;
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
                func = &Reader::readAsTEGbypass;
                binarizationParameter = binarzationParams.getCMaxTeg();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
                func = &Reader::readAsSTEGbypass;
                binarizationParameter = binarzationParams.getCMaxTeg();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
                //func = &Reader::readAsSUTUbypass; TODO
                binarizationParameter = binarzationParams.getSplitUnitSize();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
                //func = &Reader::readAsSSUTUbypass; TODO
                binarizationParameter = binarzationParams.getSplitUnitSize();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::DOUBLE_TRUNCATED_UNARY:
                //func = &Reader::readAsDTUbypass; TODO
                //binarizationParameter = binarzationParams.getCMaxDtu(); // TODO make binarizationParameter a vector
                binarizationParameter = binarzationParams.getSplitUnitSize();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
                //func = &Reader::readAsSDTUbypass; TODO
                //binarizationParameter = binarzationParams.getCMaxDtu(); // TODO make binarizationParameter a vector
                binarizationParameter = binarzationParams.getSplitUnitSize();
                break;
            default:
                GABAC_DIE("Invalid binarization");
        }
        while (r.isValid()) {
            uint64_t symbol = (reader.*func)(binarizationParameter);
            r.set(symbol);
            r.inc();
        }

        reader.reset();

        symbols.swap(bitstream);

        return;
    }

    uint64_t (Reader::*func)(unsigned int, unsigned int) = nullptr;
    switch (binarzation.getBinarizationID()) {
        case paramcabac::BinarizationParameters::BinarizationId::BINARY_CODING:
            func = &Reader::readAsBIcabac;
            binarizationParameter = stateVars.getCLengthBI();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY:
            func = &Reader::readAsTUcabac;
            binarizationParameter = binarzationParams.getCMax();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::EXPONENTIAL_GOLOMB:
            func = &Reader::readAsEGcabac;
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
            func = &Reader::readAsSEGcabac;
            break;
        case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
            func = &Reader::readAsTEGcabac;
            binarizationParameter = binarzationParams.getCMaxTeg();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            func = &Reader::readAsSTEGcabac;
            binarizationParameter = binarzationParams.getCMaxTeg();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
            //func = &Reader::readAsSUTUcabac; TODO
            binarizationParameter = binarzationParams.getSplitUnitSize();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
            //func = &Reader::readAsSSUTUcabac; TODO
            binarizationParameter = binarzationParams.getSplitUnitSize();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::DOUBLE_TRUNCATED_UNARY:
            //func = &Reader::readAsDTUcabac; TODO
            //binarizationParameter = binarzationParams.getCMaxDtu(); // TODO make binarizationParameter a vector
            binarizationParameter = binarzationParams.getSplitUnitSize();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
            //func = &Reader::readAsSDTUcabac; TODO
            //binarizationParameter = binarzationParams.getCMaxDtu(); // TODO make binarizationParameter a vector
            binarizationParameter = binarzationParams.getSplitUnitSize();
            break;
        default:
            GABAC_DIE("Invalid binarization");
    }

    uint8_t codingOrder = supportVals.getCodingOrder();
    if (codingOrder == 0) {
        while (r.isValid()) {
            uint64_t symbol = (reader.*func)(binarizationParameter, 0);
            r.set(symbol);
            r.inc();
        }
    } else if (codingOrder == 1) {
        unsigned int previousSymbol = 0;
        while (r.isValid()) {
            uint64_t symbol = (reader.*func)(binarizationParameter, previousSymbol << 2u);
            r.set(symbol);
            if (int64_t(symbol) < 0) {
                symbol = uint64_t(-int64_t(symbol));
            }
            if (symbol > 3) {
                previousSymbol = 3;
            } else {
                assert(symbol <= std::numeric_limits<unsigned int>::max());
                previousSymbol = static_cast<unsigned int>(symbol);
            }
            r.inc();
        }
    } else if (codingOrder == 2) {
        unsigned int previousSymbol = 0;
        unsigned int previousPreviousSymbol = 0;

        while (r.isValid()) {
            uint64_t symbol = (reader.*func)(binarizationParameter, (previousSymbol << 2u) + previousPreviousSymbol);
            r.set(symbol);
            previousPreviousSymbol = previousSymbol;
            if (int64_t(symbol) < 0) {
                symbol = uint64_t(-int64_t(symbol));
            }
            if (symbol > 3) {
                previousSymbol = 3;
            } else {
                assert(symbol <= std::numeric_limits<unsigned int>::max());
                previousSymbol = static_cast<unsigned int>(symbol);
            }
            r.inc();
        }
    } else {
        GABAC_DIE("Invalid context selection");
    }

    reader.reset();

    symbols.swap(bitstream);
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
