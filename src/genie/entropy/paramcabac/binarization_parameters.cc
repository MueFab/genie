/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "binarization_parameters.h"
#include <genie/util/bitwriter.h>
#include <genie/util/exceptions.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters() : BinarizationParameters(BinarizationId::BINARY_CODING, std::vector<uint8_t>()) {}

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

BinarizationParameters::BinarizationParameters(const BinarizationId &_binarization_id, std::vector<uint8_t> params)
    : cmax(), cmax_teg(), cmax_dtu(), split_unit_size() {
    switch (_binarization_id) {
        case BinarizationId::TRUNCATED_UNARY:
            cmax = params[0];
            break;
        case BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
        case BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            cmax_teg = params[0];
            break;
        case BinarizationId::DOUBLE_TRUNCATED_UNARY:
        case BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
            cmax_dtu = params[1];  // Fall-through
        case BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
        case BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
            split_unit_size = params[0];
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

void BinarizationParameters::write(BinarizationId binID, util::BitWriter &writer) const {
    switch (binID) {
        case BinarizationId::TRUNCATED_UNARY:
            writer.write(cmax, 8);
            break;
        case BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
        case BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            writer.write(cmax_teg, 8);
            break;
        case BinarizationId::DOUBLE_TRUNCATED_UNARY:
        case BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY:
            writer.write(cmax_dtu, 8);  // Fall-through
        case BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY:
        case BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY:
            writer.write(split_unit_size, 4);
            break;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::getCMax() const {
    return cmax;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::getCMaxTeg() const {
    return cmax_teg;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::getCMaxDtu() const {
    return cmax_dtu;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::getSplitUnitSize() const {
    return split_unit_size;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::numParams[unsigned(BinarizationId::SIGNED_DOUBLE_TRUNCATED_UNARY) + 1u] = {0, 1, 0, 0, 1, 1, 1, 1, 2, 2};

uint8_t BinarizationParameters::getNumBinarizationParams(BinarizationParameters::BinarizationId binarzationId) {
    return BinarizationParameters::numParams[uint8_t(binarzationId)];
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
