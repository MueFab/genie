#include "cabac_binarization.h"
#include "../../../make_unique.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace desc_conf_pres {
namespace cabac {
CabacBinarization::CabacBinarization(const CabacBinarizationParameters::BinarizationId &_binarization_ID,
                                     std::unique_ptr<CabacBinarizationParameters> _cabac_binarization_parameters)
    : binarization_ID(_binarization_ID),
      bypass_flag(true),
      cabac_binarization_parameters(std::move(_cabac_binarization_parameters)),
      cabac_context_parameters(nullptr) {}

// -----------------------------------------------------------------------------------------------------------------

CabacBinarization::CabacBinarization()
    : CabacBinarization(
          CabacBinarizationParameters::BinarizationId::BINARY_CODING,
          make_unique<CabacBinarizationParameters>(CabacBinarizationParameters::BinarizationId::BINARY_CODING, 0)) {}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<CabacBinarization> CabacBinarization::clone() const {
    auto ret = make_unique<CabacBinarization>();
    ret->binarization_ID = binarization_ID;
    ret->bypass_flag = bypass_flag;
    ret->cabac_binarization_parameters = cabac_binarization_parameters->clone();
    ret->cabac_context_parameters = cabac_context_parameters->clone();
    return ret;
}

// -----------------------------------------------------------------------------------------------------------------

void CabacBinarization::setContextParameters(std::unique_ptr<CabacContextParameters> _cabac_context_parameters) {
    bypass_flag = false;
    cabac_context_parameters = std::move(_cabac_context_parameters);
}

// -----------------------------------------------------------------------------------------------------------------

void CabacBinarization::write(util::BitWriter *writer) const {
    writer->write(uint8_t(binarization_ID), 5);
    writer->write(bypass_flag, 1);
    cabac_binarization_parameters->write(writer);
    if (cabac_context_parameters) {
        cabac_context_parameters->write(writer);
    }
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format
