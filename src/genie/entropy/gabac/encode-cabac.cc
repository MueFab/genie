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

namespace genie {
namespace entropy {
namespace gabac {

void encode_cabac(const paramcabac::TransformedSeq &conf,
                  util::DataBlock* const symbols,
                  size_t maxSize) {
    util::DataBlock block(0, 1);

    assert(symbols != nullptr);
#ifndef NDEBUG
    //const unsigned int paramSize[unsigned(BinarizationId::STEG) + 1u] = {1, 1, 0, 0, 1, 1}; // FIXME what is this??? TBC
#endif
    //assert(binarzation.getCabacBinarizationParameters().size() >= paramSize[static_cast<int>(binarzation.binarization_ID)]);

    size_t numSymbols = symbols->size();
    if (numSymbols <= 0) return;

    const paramcabac::SupportValues &supportVals = conf.getSupportValues();
    const paramcabac::Binarization &binarzation = conf.getBinarization();
    const paramcabac::BinarizationParameters &binarzationParams = binarzation.getCabacBinarizationParameters();
    const paramcabac::StateVars &stateVars = conf.getStateVars();

    OBufferStream bitstream(&block);
    Writer writer(&bitstream);
    writer.start(numSymbols);

    unsigned int binarizationParameter = 0;

    util::BlockStepper r = symbols->getReader();

    if (binarzation.getBypassFlag()) { // bypass mode
        void (Writer::*func)(uint64_t, unsigned int) = nullptr;
        switch (binarzation.getBinarizationID()) {
            case paramcabac::BinarizationParameters::BinarizationId::BINARY_CODING:
                func = &Writer::writeAsBIbypass;
                binarizationParameter = stateVars.getCLengthBI();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY:
                func = &Writer::writeAsTUbypass;
                binarizationParameter = binarzationParams.getCMax();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::EXPONENTIAL_GOLOMB:
                func = &Writer::writeAsEGbypass;
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
                func = &Writer::writeAsSEGbypass;
                break;
            case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
                func = &Writer::writeAsTEGbypass;
                binarizationParameter = binarzationParams.getCMaxTeg();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
                func = &Writer::writeAsSTEGbypass;
                binarizationParameter = binarzationParams.getCMaxTeg();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
                //func = &Writer::writeAsSUTUbypass; TODO
                binarizationParameter = binarzationParams.getSplitUnitSize();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
                //func = &Writer::writeAsSSUTUbypass; TODO
                binarizationParameter = binarzationParams.getSplitUnitSize();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::DOUBLE_TRUNCATED_UNARY:
                //func = &Writer::writeAsDTUbypass; TODO
                //binarizationParameter = binarzationParams.getCMaxDtu(); // TODO make binarizationParameter a vector
                binarizationParameter = binarzationParams.getSplitUnitSize();
                break;
            case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
                //func = &Writer::writeAsSDTUbypass; TODO
                //binarizationParameter = binarzationParams.getCMaxDtu(); // TODO make binarizationParameter a vector
                binarizationParameter = binarzationParams.getSplitUnitSize();
                break;
            default:
                GABAC_DIE("Unknown Binarization");
        }
        while (r.isValid()) {
            if (maxSize <= bitstream.size()) {
                break;
            }
            (writer.*func)(r.get(), binarizationParameter);
            r.inc();
        }

        writer.reset();

        bitstream.flush(symbols);

        return;
    }

    void (Writer::*func)(uint64_t, unsigned int, unsigned int) = nullptr;
    switch (binarzation.getBinarizationID()) {
        case paramcabac::BinarizationParameters::BinarizationId::BINARY_CODING:
            func = &Writer::writeAsBIcabac;
            binarizationParameter = stateVars.getCLengthBI();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_UNARY:
            func = &Writer::writeAsTUcabac;
            binarizationParameter = binarzationParams.getCMax();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::EXPONENTIAL_GOLOMB:
            func = &Writer::writeAsEGcabac;
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_EXPONENTIAL_GOMB:
            func = &Writer::writeAsSEGcabac;
            break;
        case paramcabac::BinarizationParameters::BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
            func = &Writer::writeAsTEGcabac;
            binarizationParameter = binarzationParams.getCMaxTeg();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            func = &Writer::writeAsSTEGcabac;
            binarizationParameter = binarzationParams.getCMaxTeg();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
            //func = &Writer::writeAsSUTUcabac; TODO
            binarizationParameter = binarzationParams.getSplitUnitSize();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
            //func = &Writer::writeAsSSUTUcabac; TODO
            binarizationParameter = binarzationParams.getSplitUnitSize();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::DOUBLE_TRUNCATED_UNARY:
            //func = &Writer::writeAsDTUcabac; TODO
            //binarizationParameter = binarzationParams.getCMaxDtu(); // TODO make binarizationParameter a vector
            binarizationParameter = binarzationParams.getSplitUnitSize();
            break;
        case paramcabac::BinarizationParameters::BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
            //func = &Writer::writeAsSDTUcabac; TODO
            //binarizationParameter = binarzationParams.getCMaxDtu(); // TODO make binarizationParameter a vector
            binarizationParameter = binarzationParams.getSplitUnitSize();
            break;
        default:
            GABAC_DIE("Unknown Binarization");
    }

    uint8_t codingOrder = supportVals.getCodingOrder();
    if (codingOrder == 0) {
        while (r.isValid()) {
            if (maxSize <= bitstream.size()) {
                break;
            }
            (writer.*func)(r.get(), binarizationParameter, 0);
            r.inc();
        }
    } else if (codingOrder == 1) {
        unsigned int previousSymbol = 0;
        while (r.isValid()) {
            if (maxSize <= bitstream.size()) {
                break;
            }
            uint64_t symbol = r.get();
            (writer.*func)(r.get(), binarizationParameter, previousSymbol << 2u);
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
            if (maxSize <= bitstream.size()) {
                break;
            }
            uint64_t symbol = r.get();
            (writer.*func)(symbol, binarizationParameter, (previousSymbol << 2u) + previousPreviousSymbol);
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

    writer.reset();

    bitstream.flush(symbols);
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
