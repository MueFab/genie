/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "gabac/encode_cabac.h"

#include <cassert>

#include "gabac/block_stepper.h"
#include "gabac/constants.h"
#include "gabac/data_block.h"
#include "gabac/writer.h"

namespace gabac {

void encode_cabac(
        const BinarizationId& binarizationId,
        const std::vector<unsigned int>& binarizationParameters,
        const ContextSelectionId& contextSelectionId,
        DataBlock *const symbols,
        size_t maxSize
){
    DataBlock bitstream(0, 1);
    assert(symbols != nullptr);
#ifndef NDEBUG
    const unsigned int paramSize[unsigned(BinarizationId::STEG) + 1u] = {1, 1, 0, 0, 1, 1};
#endif
    assert(binarizationParameters.size() >= paramSize[static_cast<int>(binarizationId)]);

    Writer writer(&bitstream);
    writer.start(symbols->size());

    unsigned int binarizationParameter = 0;
    if (!binarizationParameters.empty()) {
        binarizationParameter = binarizationParameters[0];
    }

    BlockStepper r = symbols->getReader();

    if (contextSelectionId == ContextSelectionId::bypass) {
        void (Writer::*func)(uint64_t, unsigned int);
        switch (binarizationId) {
            case BinarizationId::BI:
                func = &Writer::writeAsBIbypass;
                break;
            case BinarizationId::TU:
                func = &Writer::writeAsTUbypass;
                break;
            case BinarizationId::EG:
                func = &Writer::writeAsEGbypass;
                break;
            case BinarizationId::SEG:
                func = &Writer::writeAsSEGbypass;
                break;
            case BinarizationId::TEG:
                func = &Writer::writeAsTEGbypass;
                break;
            case BinarizationId::STEG:
                func = &Writer::writeAsSTEGbypass;
                break;
            default:
                GABAC_DIE("Unknown Binarization");
        }
        while (r.isValid()) {
            if (maxSize <= bitstream.size()) {
                break;
            }
            (writer.*func)(
                    r.get(),
                    binarizationParameter
            );
            r.inc();
        }

        writer.reset();

        symbols->swap(&bitstream);

        return;
    }

    void (Writer::*func)(uint64_t, unsigned int, unsigned int);
    switch (binarizationId) {
        case BinarizationId::BI:
            func = &Writer::writeAsBIcabac;
            break;
        case BinarizationId::TU:
            func = &Writer::writeAsTUcabac;
            break;
        case BinarizationId::EG:
            func = &Writer::writeAsEGcabac;
            break;
        case BinarizationId::SEG:
            func = &Writer::writeAsSEGcabac;
            break;
        case BinarizationId::TEG:
            func = &Writer::writeAsTEGcabac;
            break;
        case BinarizationId::STEG:
            func = &Writer::writeAsSTEGcabac;
            break;
        default:
            GABAC_DIE("Unknown Binarization");
    }

    if (contextSelectionId == ContextSelectionId::adaptive_coding_order_0) {
        while (r.isValid()) {
            if (maxSize <= bitstream.size()) {
                break;
            }
            (writer.*func)(
                    r.get(),
                    binarizationParameter,
                    0
            );
            r.inc();
        }
    } else if (contextSelectionId == ContextSelectionId::adaptive_coding_order_1) {
        unsigned int previousSymbol = 0;
        while (r.isValid()) {
            if (maxSize <= bitstream.size()) {
                break;
            }
            uint64_t symbol = r.get();
            (writer.*func)(
                    r.get(),
                    binarizationParameter,
                    previousSymbol << 2u
            );
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
            if (maxSize <= bitstream.size()) {
                break;
            }
            uint64_t symbol = r.get();
            (writer.*func)(
                    symbol,
                    binarizationParameter,
                    (previousSymbol << 2u) + previousPreviousSymbol
            );
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

    symbols->swap(&bitstream);
}

}  // namespace gabac
