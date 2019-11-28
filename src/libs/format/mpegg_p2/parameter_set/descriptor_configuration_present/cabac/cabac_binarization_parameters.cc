#include "cabac_binarization_parameters.h"
#include "../../../make_unique.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace desc_conf_pres {
namespace cabac {
void CabacBinarizationParameters::write(util::BitWriter *writer) const {
    if (cmax) {
        writer->write(*cmax, 8);
    }
    if (cmax_teg) {
        writer->write(*cmax_teg, 8);
    }
    if (cmax_dtu) {
        writer->write(*cmax_dtu, 8);
    }
    if (split_unit_size) {
        writer->write(*split_unit_size, 4);
    }
}

// -----------------------------------------------------------------------------------------------------------------

CabacBinarizationParameters::CabacBinarizationParameters()
    : CabacBinarizationParameters(BinarizationId::BINARY_CODING, 0) {}

// -----------------------------------------------------------------------------------------------------------------

CabacBinarizationParameters::CabacBinarizationParameters(const BinarizationId &_binarization_id, uint8_t param)
    : cmax(nullptr), cmax_teg(nullptr), cmax_dtu(nullptr), split_unit_size(nullptr) {
    switch (_binarization_id) {
        case BinarizationId::TRUNCATED_UNARY:
            cmax = make_unique<uint8_t>(param);
            break;
        case BinarizationId::TRUNCATED_EXPONENTIAL_GOLOMB:
        case BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB:
            cmax_teg = make_unique<uint8_t>(param);
            break;
        case BinarizationId::BINARY_CODING:
        case BinarizationId::EXPONENTIAL_GOLOMB:
        case BinarizationId::SIGNED_EXPONENTIAL_GOMB:
            break;
        default:
            UTILS_THROW_RUNTIME_EXCEPTION("Binarization not supported");
    }
}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<CabacBinarizationParameters> CabacBinarizationParameters::clone() const {
    auto r = make_unique<CabacBinarizationParameters>();
    r->cmax = make_unique<uint8_t>(*cmax);
    r->cmax_teg = make_unique<uint8_t>(*cmax_teg);
    r->cmax_dtu = make_unique<uint8_t>(*cmax_dtu);
    r->split_unit_size = make_unique<uint8_t>(*split_unit_size);
    return r;
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format
