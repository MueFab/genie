/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/paramcabac/binarization.h"
#include <utility>
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

Binarization::Binarization()
    : Binarization(BinarizationParameters::BinarizationId::BI, false, BinarizationParameters(), Context()) {}

// ---------------------------------------------------------------------------------------------------------------------

Binarization::Binarization(BinarizationParameters::BinarizationId _binarization_ID, bool _bypass_flag,
                           BinarizationParameters&& _cabac_binarization_parameters, Context&& _cabac_Context_parameters)
    : binarization_ID(_binarization_ID),
      bypass_flag(_bypass_flag),
      cabac_binarization_parameters(_cabac_binarization_parameters) {
    if (!bypass_flag) {
        cabac_context_parameters = _cabac_Context_parameters;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Binarization::Binarization(uint8_t output_symbol_size, uint8_t coding_subsym_size, util::BitReader& reader) {
    binarization_ID = reader.read<BinarizationParameters::BinarizationId>(5);
    bypass_flag = reader.read<bool>(1);
    cabac_binarization_parameters = BinarizationParameters(binarization_ID, reader);
    if (!bypass_flag) {
        cabac_context_parameters = Context(output_symbol_size, coding_subsym_size, reader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

BinarizationParameters::BinarizationId Binarization::getBinarizationID() const { return binarization_ID; }

// ---------------------------------------------------------------------------------------------------------------------

bool Binarization::getBypassFlag() const { return bypass_flag; }

// ---------------------------------------------------------------------------------------------------------------------

const BinarizationParameters& Binarization::getCabacBinarizationParameters() const {
    return cabac_binarization_parameters;
}

// ---------------------------------------------------------------------------------------------------------------------

const Context& Binarization::getCabacContextParameters() const { return cabac_context_parameters; }

// ---------------------------------------------------------------------------------------------------------------------

void Binarization::setContextParameters(Context&& _cabac_context_parameters) {
    bypass_flag = false;
    cabac_context_parameters = _cabac_context_parameters;
}

// ---------------------------------------------------------------------------------------------------------------------

void Binarization::write(util::BitWriter& writer) const {
    writer.writeBits(uint8_t(binarization_ID), 5);
    writer.writeBits(bypass_flag, 1);
    cabac_binarization_parameters.write(binarization_ID, writer);
    if (!bypass_flag) {
        cabac_context_parameters.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Binarization::getNumBinarizationParams() {
    return BinarizationParameters::getNumBinarizationParams(binarization_ID);
}

// ---------------------------------------------------------------------------------------------------------------------

bool Binarization::operator==(const Binarization& bin) const {
    return binarization_ID == bin.binarization_ID && bypass_flag == bin.bypass_flag &&
           cabac_binarization_parameters == bin.cabac_binarization_parameters &&
           cabac_context_parameters == bin.cabac_context_parameters;
}

// ---------------------------------------------------------------------------------------------------------------------

Binarization::Binarization(nlohmann::json j) {
    binarization_ID = static_cast<BinarizationParameters::BinarizationId>(j["binarization_ID"]);
    bypass_flag = static_cast<bool>(uint8_t(j["bypass_flag"]));
    cabac_binarization_parameters = BinarizationParameters(j["cabac_binarization_parameters"], binarization_ID);
    if (!bypass_flag) {
        cabac_context_parameters = Context(j["cabac_context_parameters"]);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json Binarization::toJson() const {
    nlohmann::json ret;
    ret["binarization_ID"] = static_cast<uint8_t>(binarization_ID);
    ret["bypass_flag"] = static_cast<bool>(bypass_flag);
    if (!cabac_binarization_parameters.toJson(binarization_ID).is_null()) {
        ret["cabac_binarization_parameters"] = cabac_binarization_parameters.toJson(binarization_ID);
    }
    if (!bypass_flag) {
        ret["cabac_context_parameters"] = cabac_context_parameters.toJson();
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
