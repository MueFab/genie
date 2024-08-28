/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/paramcabac/support_values.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

SupportValues::SupportValues() : SupportValues(8, 8, 0) {}

// ---------------------------------------------------------------------------------------------------------------------

SupportValues::SupportValues(uint8_t _output_symbol_size, uint8_t _coding_subsym_size, uint8_t _coding_order,
                             bool _share_subsym_lut_flag, bool _share_subsym_prv_flag)
    : output_symbol_size(_output_symbol_size),
      coding_subsym_size(_coding_subsym_size),
      coding_order(_coding_order),
      share_subsym_lut_flag(_share_subsym_lut_flag),
      share_subsym_prv_flag(_share_subsym_prv_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

SupportValues::SupportValues(TransformIdSubsym transformIdSubsym, util::BitReader &reader)
    : share_subsym_lut_flag(false), share_subsym_prv_flag(false) {
    output_symbol_size = reader.read<uint8_t>(6);
    coding_subsym_size = reader.read<uint8_t>(6);
    coding_order = reader.read<uint8_t>(2);
    if (coding_subsym_size < output_symbol_size && coding_order > 0) {
        if (transformIdSubsym == TransformIdSubsym::LUT_TRANSFORM) {
            share_subsym_lut_flag = reader.read<bool>(1);
        }
        share_subsym_prv_flag = reader.read<bool>(1);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SupportValues::write(TransformIdSubsym transformIdSubsym, util::BitWriter &writer) const {
    writer.write(output_symbol_size, 6);
    writer.write(coding_subsym_size, 6);
    writer.write(coding_order, 2);
    if (coding_subsym_size < output_symbol_size && coding_order > 0) {
        if (transformIdSubsym == TransformIdSubsym::LUT_TRANSFORM) {
            writer.write(share_subsym_lut_flag, 1);
        }
        writer.write(share_subsym_prv_flag, 1);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SupportValues::getOutputSymbolSize() const { return output_symbol_size; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SupportValues::getCodingSubsymSize() const { return coding_subsym_size; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SupportValues::getCodingOrder() const { return coding_order; }

// ---------------------------------------------------------------------------------------------------------------------

bool SupportValues::getShareSubsymLutFlag() const { return share_subsym_lut_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool SupportValues::getShareSubsymPrvFlag() const { return share_subsym_prv_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool SupportValues::operator==(const SupportValues &val) const {
    return output_symbol_size == val.output_symbol_size && coding_subsym_size == val.coding_subsym_size &&
           coding_order == val.coding_order && share_subsym_lut_flag == val.share_subsym_lut_flag &&
           share_subsym_prv_flag == val.share_subsym_prv_flag;
}

// ---------------------------------------------------------------------------------------------------------------------

SupportValues::SupportValues(nlohmann::json j, TransformIdSubsym transformIdSubsym) {
    share_subsym_lut_flag = false;
    share_subsym_prv_flag = false;
    output_symbol_size = j["output_symbol_size"];
    coding_subsym_size = j["coding_subsym_size"];
    coding_order = j["coding_order"];
    if (coding_subsym_size < output_symbol_size && coding_order > 0) {
        if (transformIdSubsym == TransformIdSubsym::LUT_TRANSFORM) {
            share_subsym_lut_flag = j["share_subsym_lut_flag"];
        }
        share_subsym_prv_flag = j["share_subsym_prv_flag"];
    }
}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json SupportValues::toJson(TransformIdSubsym transformIdSubsym) const {
    nlohmann::json ret;
    ret["output_symbol_size"] = output_symbol_size;
    ret["coding_subsym_size"] = coding_subsym_size;
    ret["coding_order"] = coding_order;
    if (coding_subsym_size < output_symbol_size && coding_order > 0) {
        if (transformIdSubsym == TransformIdSubsym::LUT_TRANSFORM) {
            ret["share_subsym_lut_flag"] = share_subsym_lut_flag;
        }
        ret["share_subsym_prv_flag"] = share_subsym_prv_flag;
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
