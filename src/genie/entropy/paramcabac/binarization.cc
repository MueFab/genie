/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "binarization.h"
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

Binarization::Binarization(BinarizationParameters::BinarizationId _binarization_ID,
                           BinarizationParameters&& _cabac_binarization_parameters)
    : binarization_ID(_binarization_ID),
      bypass_flag(true),
      cabac_binarization_parameters(std::move(_cabac_binarization_parameters)),
      cabac_context_parameters() {}

// ---------------------------------------------------------------------------------------------------------------------

Binarization::Binarization()
    : Binarization(BinarizationParameters::BinarizationId::BINARY_CODING,
                   BinarizationParameters(BinarizationParameters::BinarizationId::BINARY_CODING, 0)) {}

// ---------------------------------------------------------------------------------------------------------------------

Binarization::Binarization(uint8_t coding_subsym_size, uint8_t output_symbol_size, util::BitReader& reader) {
    binarization_ID = reader.read<BinarizationParameters::BinarizationId>(5);
    bypass_flag = reader.read<bool>(1);
    cabac_binarization_parameters = BinarizationParameters(binarization_ID, reader);
    if (!bypass_flag) {
        cabac_context_parameters = Context(coding_subsym_size, output_symbol_size, reader);
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

const Context& Binarization::getCabacContextParameters() const { return cabac_context_parameters.get(); }

// ---------------------------------------------------------------------------------------------------------------------

void Binarization::setContextParameters(Context&& _cabac_context_parameters) {
    bypass_flag = false;
    cabac_context_parameters = std::move(_cabac_context_parameters);
}

// ---------------------------------------------------------------------------------------------------------------------

void Binarization::write(util::BitWriter& writer) const {
    writer.write(uint8_t(binarization_ID), 5);
    writer.write(bypass_flag, 1);
    cabac_binarization_parameters.write(writer);
    if (cabac_context_parameters) {
        cabac_context_parameters->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------