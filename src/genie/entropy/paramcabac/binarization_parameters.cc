/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "binarization_parameters.h"
#include <genie/util/bitwriter.h>
#include <genie/util/runtime-exception.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters()
    : BinarizationParameters(BinarizationId::BI, std::vector<uint8_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters(BinarizationId binID, util::BitReader &reader) {
    switch (binID) {
        case BinarizationId::TU:
            cmax = reader.read<uint8_t>();
            break;
        case BinarizationId::TEG:
        case BinarizationId::STEG:
            cmax_teg = reader.read<uint8_t>();
            break;
        case BinarizationId::DTU:
        case BinarizationId::SDTU:
            cmax_dtu = reader.read<uint8_t>();  // Fall-through
        case BinarizationId::SUTU:
        case BinarizationId::SSUTU:
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
        case BinarizationId::TU:
            cmax = params[0];
            break;
        case BinarizationId::TEG:
        case BinarizationId::STEG:
            cmax_teg = params[0];
            break;
        case BinarizationId::DTU:
        case BinarizationId::SDTU:
            cmax_dtu = params[1];  // Fall-through
        case BinarizationId::SUTU:
        case BinarizationId::SSUTU:
            split_unit_size = params[0];
            break;
        case BinarizationId::BI:
        case BinarizationId::EG:
        case BinarizationId::SEG:
            break;
        default:
            UTILS_THROW_RUNTIME_EXCEPTION("Binarization not supported");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void BinarizationParameters::write(BinarizationId binID, util::BitWriter &writer) const {
    switch (binID) {
        case BinarizationId::TU:
            writer.write(cmax, 8);
            break;
        case BinarizationId::TEG:
        case BinarizationId::STEG:
            writer.write(cmax_teg, 8);
            break;
        case BinarizationId::DTU:
        case BinarizationId::SDTU:
            writer.write(cmax_dtu, 8);  // Fall-through
        case BinarizationId::SUTU:
        case BinarizationId::SSUTU:
            writer.write(split_unit_size, 4);
            break;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::getCMax() const { return cmax; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::getCMaxTeg() const { return cmax_teg; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::getCMaxDtu() const { return cmax_dtu; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::getSplitUnitSize() const { return split_unit_size; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::numParams[unsigned(BinarizationId::SDTU) + 1u] = {
    0, 1, 0, 0, 1, 1, 1, 1, 2, 2};

uint8_t BinarizationParameters::getNumBinarizationParams(BinarizationParameters::BinarizationId binarzationId) {
    return BinarizationParameters::numParams[uint8_t(binarzationId)];
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
