#include "support_values.h"
#include "util/bitwriter.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {
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
            share_subsym_lut_flag = util::make_unique<bool>(_share_subsym_lut_flag);
        }
        share_subsym_prv_flag = util::make_unique<bool>(_share_subsym_prv_flag);
    }
}

// -----------------------------------------------------------------------------------------------------------------

SupportValues::SupportValues() : SupportValues(0, 0, 0, TransformIdSubsym::NO_TRANSFORM) {}

SupportValues::SupportValues(TransformIdSubsym transformIdSubsym, util::BitReader &reader) {
    output_symbol_size = reader.read(6);
    coding_subsym_size = reader.read(6);
    coding_order = reader.read(2);
    if (coding_subsym_size < output_symbol_size && coding_order > 0) {
        if (transformIdSubsym == TransformIdSubsym::LUT_TRANSFORM) {
            share_subsym_lut_flag = util::make_unique<bool>(reader.read(1));
        }
        share_subsym_prv_flag = util::make_unique<bool>(reader.read(1));
    }
}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<SupportValues> SupportValues::clone() const {
    auto ret = util::make_unique<SupportValues>();
    ret->output_symbol_size = output_symbol_size;
    ret->coding_subsym_size = coding_subsym_size;
    ret->coding_order = coding_order;
    if (share_subsym_lut_flag) {
        ret->share_subsym_lut_flag = util::make_unique<bool>(*share_subsym_lut_flag);
    }
    if (share_subsym_prv_flag) {
        ret->share_subsym_prv_flag = util::make_unique<bool>(*share_subsym_prv_flag);
    }
    return ret;
}

// -----------------------------------------------------------------------------------------------------------------

void SupportValues::write(util::BitWriter &writer) const {
    writer.write(output_symbol_size, 6);
    writer.write(coding_subsym_size, 6);
    writer.write(coding_order, 2);
    if (share_subsym_lut_flag) {
        writer.write(*share_subsym_lut_flag, 1);
    }
    if (share_subsym_prv_flag) {
        writer.write(*share_subsym_prv_flag, 1);
    }
}

uint8_t SupportValues::getOutputSymbolSize() const { return output_symbol_size; }

uint8_t SupportValues::getCodingSubsymSize() const { return coding_subsym_size; }

uint8_t SupportValues::getCodingOrder() const { return coding_order; }

bool SupportValues::getShareSubsymLutFlag() const { return *share_subsym_lut_flag; }

bool SupportValues::getShareSubsymPrvFlag() const { return *share_subsym_prv_flag; }

}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace genie