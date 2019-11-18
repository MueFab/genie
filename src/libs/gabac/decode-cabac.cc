/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "decode-cabac.h"

#include <cassert>
#include <limits>

#include "constants.h"
#include "data-block.h"
#include "reader.h"

namespace gabac {

void decode_cabac(const BinarizationId& binarizationId, const std::vector<unsigned int>& binarizationParameters,
                  const ContextSelectionId& contextSelectionId, const uint8_t wordsize, DataBlock* const bitstream) {
    DataBlock symbols(0, wordsize);
    if (bitstream == nullptr) {
        GABAC_DIE("Bitstream is null");
    }

    Reader reader(bitstream);
    size_t numSymbols = reader.start();
    if (numSymbols <= 0) return;

    // symbols->clear();
    symbols.resize(numSymbols);

    unsigned int binarizationParameter = 0;
    if (!binarizationParameters.empty()) {
        binarizationParameter = binarizationParameters[0];
    }

    BlockStepper r = symbols.getReader();

    if (contextSelectionId == ContextSelectionId::bypass) {
        uint64_t (Reader::*func)(unsigned int);
        switch (binarizationId) {
            case BinarizationId::BI:
                func = &Reader::readAsBIbypass;
                break;
            case BinarizationId::TU:
                func = &Reader::readAsTUbypass;
                break;
            case BinarizationId::EG:
                func = &Reader::readAsEGbypass;
                break;
            case BinarizationId::SEG:
                func = &Reader::readAsSEGbypass;
                break;
            case BinarizationId::TEG:
                func = &Reader::readAsTEGbypass;
                break;
            case BinarizationId::STEG:
                func = &Reader::readAsSTEGbypass;
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

    uint64_t (Reader::*func)(unsigned int, unsigned int);
    switch (binarizationId) {
        case BinarizationId::BI:
            func = &Reader::readAsBIcabac;
            break;
        case BinarizationId::TU:
            func = &Reader::readAsTUcabac;
            break;
        case BinarizationId::EG:
            func = &Reader::readAsEGcabac;
            break;
        case BinarizationId::SEG:
            func = &Reader::readAsSEGcabac;
            break;
        case BinarizationId::TEG:
            func = &Reader::readAsTEGcabac;
            break;
        case BinarizationId::STEG:
            func = &Reader::readAsSTEGcabac;
            break;
        default:
            GABAC_DIE("Invalid binarization");
    }

    if (contextSelectionId == ContextSelectionId::adaptive_coding_order_0) {
        while (r.isValid()) {
            uint64_t symbol = (reader.*func)(binarizationParameter, 0);
            r.set(symbol);
            r.inc();
        }
    } else if (contextSelectionId == ContextSelectionId::adaptive_coding_order_1) {
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
    } else if (contextSelectionId == ContextSelectionId::adaptive_coding_order_2) {
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
