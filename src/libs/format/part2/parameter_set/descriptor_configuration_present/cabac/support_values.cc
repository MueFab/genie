#include "support_values.h"
#include "../../../make_unique.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace desc_conf_pres {
namespace cabac {
SupportValues::SupportValues(uint8_t _output_symbol_size, uint8_t _coding_subsym_size, uint8_t _coding_order,
                             const TransformIdSubsym &transform_ID_subsym, bool _share_subsym_prv_flag,
                             bool _share_subsym_lut_flag)
    : output_symbol_size(_output_symbol_size),
      coding_subsym_size(_coding_subsym_size),
      coding_order(_coding_order),
      share_subsym_lut_flag(nullptr),
      share_subsym_prv_flag(nullptr) {
    if (coding_subsym_size < output_symbol_size && coding_order > 0) {
        if (transform_ID_subsym == TransformIdSubsym::LUT_TRANSFORM) {
            share_subsym_lut_flag = make_unique<bool>(_share_subsym_lut_flag);
        }
        share_subsym_prv_flag = make_unique<bool>(_share_subsym_prv_flag);
    }
}

// -----------------------------------------------------------------------------------------------------------------

SupportValues::SupportValues() : SupportValues(0, 0, 0, TransformIdSubsym::NO_TRANSFORM) {}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<SupportValues> SupportValues::clone() const {
    auto ret = make_unique<SupportValues>();
    ret->output_symbol_size = output_symbol_size;
    ret->coding_subsym_size = coding_subsym_size;
    ret->coding_order = coding_order;
    ret->share_subsym_lut_flag = make_unique<bool>(*share_subsym_lut_flag);
    ret->share_subsym_prv_flag = make_unique<bool>(*share_subsym_prv_flag);
    return ret;
}

// -----------------------------------------------------------------------------------------------------------------

void SupportValues::write(util::BitWriter *writer) const {
    writer->write(output_symbol_size, 6);
    writer->write(coding_subsym_size, 6);
    writer->write(coding_order, 2);
    if (share_subsym_lut_flag) {
        writer->write(*share_subsym_lut_flag, 1);
    }
    if (share_subsym_prv_flag) {
        writer->write(*share_subsym_prv_flag, 1);
    }
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format
