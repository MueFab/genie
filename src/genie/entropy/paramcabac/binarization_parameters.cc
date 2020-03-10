/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "binarization_parameters.h"
#include <genie/util/bitwriter.h>
#include <genie/util/exceptions.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::getParameter() const {
    if (cmax) {
        return *cmax;
    }
    if (cmax_teg) {
        return *cmax_teg;
    }
    if (cmax_dtu) {
        return *cmax_dtu;
    }
    if (split_unit_size) {
        return *split_unit_size;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

void BinarizationParameters::write(util::BitWriter &writer) const {
    if (cmax) {
        writer.write(*cmax, 8);
    }
    if (cmax_teg) {
        writer.write(*cmax_teg, 8);
    }
    if (cmax_dtu) {
        writer.write(*cmax_dtu, 8);
    }
    if (split_unit_size) {
        writer.write(*split_unit_size, 4);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters() : BinarizationParameters(BinarizationId::BINARY_CODING, 0) {}

// ---------------------------------------------------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters(BinarizationId binID, util::BitReader &reader) {
    switch (binID) {
        case BinarizationId::TRUNCATED_UNARY:
            cmax = reader.read<uint8_t>();
            break;
        case BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
        case BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            cmax_teg = reader.read<uint8_t>();
            break;
        case BinarizationId::DOUBLE_TRUNCATED_UNARY:
        case BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
            cmax_dtu = reader.read<uint8_t>();  // Fall-through
        case BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
        case BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
            split_unit_size = reader.read<uint8_t>(4);
            break;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters(const BinarizationId &_binarization_id, uint8_t param)
    : cmax(), cmax_teg(), cmax_dtu(), split_unit_size() {
    switch (_binarization_id) {
        case BinarizationId::TRUNCATED_UNARY:
            cmax = param;
            break;
        case BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
        case BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            cmax_teg = param;
            break;
        case BinarizationId::BINARY_CODING:
        case BinarizationId::EXPONENTIAL_GOLOMB:
        case BinarizationId::SIGNED_EXPONENTIAL_GOMB:
            break;
        default:
            UTILS_THROW_RUNTIME_EXCEPTION("Binarization not supported");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------