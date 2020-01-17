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

Binarization::Binarization(const BinarizationParameters::BinarizationId& _binarization_ID,
                           std::unique_ptr<BinarizationParameters> _cabac_binarization_parameters)
    : binarization_ID(_binarization_ID),
      bypass_flag(true),
      cabac_binarization_parameters(std::move(_cabac_binarization_parameters)),
      cabac_context_parameters(nullptr) {}

// ---------------------------------------------------------------------------------------------------------------------

Binarization::Binarization()
    : Binarization(
          BinarizationParameters::BinarizationId::BINARY_CODING,
          util::make_unique<BinarizationParameters>(BinarizationParameters::BinarizationId::BINARY_CODING, 0)) {}

// ---------------------------------------------------------------------------------------------------------------------

Binarization::Binarization(uint8_t coding_subsym_size, uint8_t output_symbol_size, util::BitReader& reader) {
    binarization_ID = BinarizationParameters::BinarizationId(reader.read(5));
    bypass_flag = reader.read(1);
    cabac_binarization_parameters = util::make_unique<BinarizationParameters>(binarization_ID, reader);
    if (!bypass_flag) {
        cabac_context_parameters = util::make_unique<Context>(coding_subsym_size, output_symbol_size, reader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Binarization> Binarization::clone() const {
    auto ret = util::make_unique<Binarization>();
    ret->binarization_ID = binarization_ID;
    ret->bypass_flag = bypass_flag;
    ret->cabac_binarization_parameters = cabac_binarization_parameters->clone();
    if (cabac_context_parameters) {
        ret->cabac_context_parameters = cabac_context_parameters->clone();
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

BinarizationParameters::BinarizationId Binarization::getBinarizationID() const { return binarization_ID; }

// ---------------------------------------------------------------------------------------------------------------------

bool Binarization::getBypassFlag() const { return bypass_flag; }

// ---------------------------------------------------------------------------------------------------------------------

const BinarizationParameters* Binarization::getCabacBinarizationParameters() const {
    return cabac_binarization_parameters.get();
}

// ---------------------------------------------------------------------------------------------------------------------

const Context* Binarization::getCabacContextParameters() const { return cabac_context_parameters.get(); }

// ---------------------------------------------------------------------------------------------------------------------

void Binarization::setContextParameters(std::unique_ptr<Context> _cabac_context_parameters) {
    bypass_flag = false;
    cabac_context_parameters = std::move(_cabac_context_parameters);
}

// ---------------------------------------------------------------------------------------------------------------------

void Binarization::write(util::BitWriter& writer) const {
    writer.write(uint8_t(binarization_ID), 5);
    writer.write(bypass_flag, 1);
    cabac_binarization_parameters->write(writer);
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