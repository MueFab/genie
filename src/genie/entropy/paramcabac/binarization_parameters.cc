/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/paramcabac/binarization_parameters.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters() : BinarizationParameters(BinarizationId::BI, std::vector<uint8_t>()) {}

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
            writer.writeBits(cmax, 8);
            break;
        case BinarizationId::TEG:
        case BinarizationId::STEG:
            writer.writeBits(cmax_teg, 8);
            break;
        case BinarizationId::DTU:
        case BinarizationId::SDTU:
            writer.writeBits(cmax_dtu, 8);  // Fall-through
        case BinarizationId::SUTU:
        case BinarizationId::SSUTU:
            writer.writeBits(split_unit_size, 4);
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

uint8_t BinarizationParameters::numParams[unsigned(BinarizationId::SDTU) + 1u] = {0, 1, 0, 0, 1, 1, 1, 1, 2, 2};

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BinarizationParameters::getNumBinarizationParams(BinarizationParameters::BinarizationId binarzationId) {
    return BinarizationParameters::numParams[uint8_t(binarzationId)];
}

// ---------------------------------------------------------------------------------------------------------------------

bool BinarizationParameters::operator==(const BinarizationParameters &bin) const {
    return cmax == bin.cmax && cmax_teg == bin.cmax_teg && cmax_dtu == bin.cmax_dtu &&
           split_unit_size == bin.split_unit_size;
}

// ---------------------------------------------------------------------------------------------------------------------

BinarizationParameters::BinarizationParameters(nlohmann::json j, BinarizationId binID) {
    cmax = 0;
    cmax_teg = 0;
    cmax_dtu = 0;
    split_unit_size = 0;
    switch (binID) {
        case BinarizationId::TU:
            cmax = j["cmax"];
            break;
        case BinarizationId::TEG:
        case BinarizationId::STEG:
            cmax = j["cmax_teg"];
            break;
        case BinarizationId::DTU:
        case BinarizationId::SDTU:
            cmax_dtu = j["cmax_dtu"];  // Fall-through
        case BinarizationId::SUTU:
        case BinarizationId::SSUTU:
            cmax_dtu = j["split_unit_size"];
            break;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json BinarizationParameters::toJson(BinarizationId binID) const {
    nlohmann::json ret;
    switch (binID) {
        case BinarizationId::TU:
            ret["cmax"] = cmax;
            break;
        case BinarizationId::TEG:
        case BinarizationId::STEG:
            ret["cmax_teg"] = cmax_teg;
            break;
        case BinarizationId::DTU:
        case BinarizationId::SDTU:
            ret["cmax_dtu"] = cmax_dtu;  // Fall-through
        case BinarizationId::SUTU:
        case BinarizationId::SSUTU:
            ret["split_unit_size"] = cmax_dtu;
            break;
        default:
            break;
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
