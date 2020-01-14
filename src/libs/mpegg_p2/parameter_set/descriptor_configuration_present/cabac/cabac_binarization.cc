#include "cabac_binarization.h"
#include "util/bitwriter.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {
CabacBinarization::CabacBinarization(const CabacBinarizationParameters::BinarizationId& _binarization_ID,
                                     std::unique_ptr<CabacBinarizationParameters> _cabac_binarization_parameters)
    : binarization_ID(_binarization_ID),
      bypass_flag(true),
      cabac_binarization_parameters(std::move(_cabac_binarization_parameters)),
      cabac_context_parameters(nullptr) {}

// -----------------------------------------------------------------------------------------------------------------

CabacBinarization::CabacBinarization()
    : CabacBinarization(CabacBinarizationParameters::BinarizationId::BINARY_CODING,
                        util::make_unique<CabacBinarizationParameters>(
                            CabacBinarizationParameters::BinarizationId::BINARY_CODING, 0)) {}

CabacBinarization::CabacBinarization(uint8_t coding_subsym_size, uint8_t output_symbol_size, util::BitReader& reader) {
    binarization_ID = CabacBinarizationParameters::BinarizationId(reader.read(5));
    bypass_flag = reader.read(1);
    cabac_binarization_parameters = util::make_unique<CabacBinarizationParameters>(binarization_ID, reader);
    if (!bypass_flag) {
        cabac_context_parameters =
            util::make_unique<CabacContextParameters>(coding_subsym_size, output_symbol_size, reader);
    }
}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<CabacBinarization> CabacBinarization::clone() const {
    auto ret = util::make_unique<CabacBinarization>();
    ret->binarization_ID = binarization_ID;
    ret->bypass_flag = bypass_flag;
    ret->cabac_binarization_parameters = cabac_binarization_parameters->clone();
    if (cabac_context_parameters) {
        ret->cabac_context_parameters = cabac_context_parameters->clone();
    }
    return ret;
}

CabacBinarizationParameters::BinarizationId CabacBinarization::getBinarizationID() const { return binarization_ID; }

bool CabacBinarization::getBypassFlag() const { return bypass_flag; }

const CabacBinarizationParameters* CabacBinarization::getCabacBinarizationParameters() const {
    return cabac_binarization_parameters.get();
}

const CabacContextParameters* CabacBinarization::getCabacContextParameters() const {
    return cabac_context_parameters.get();
}

// -----------------------------------------------------------------------------------------------------------------

void CabacBinarization::setContextParameters(std::unique_ptr<CabacContextParameters> _cabac_context_parameters) {
    bypass_flag = false;
    cabac_context_parameters = std::move(_cabac_context_parameters);
}

// -----------------------------------------------------------------------------------------------------------------

void CabacBinarization::write(util::BitWriter& writer) const {
    writer.write(uint8_t(binarization_ID), 5);
    writer.write(bypass_flag, 1);
    cabac_binarization_parameters->write(writer);
    if (cabac_context_parameters) {
        cabac_context_parameters->write(writer);
    }
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace genie