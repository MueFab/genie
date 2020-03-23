/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "support_values.h"
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

SupportValues::SupportValues() : SupportValues(3, 3, 1, TransformIdSubsym::NO_TRANSFORM) {}

// ---------------------------------------------------------------------------------------------------------------------

SupportValues::SupportValues(uint8_t _output_symbol_size, uint8_t _coding_subsym_size, uint8_t _coding_order,
                             const TransformIdSubsym &transform_ID_subsym, bool _share_subsym_prv_flag,
                             bool _share_subsym_lut_flag)
    : output_symbol_size(_output_symbol_size),
      coding_subsym_size(_coding_subsym_size),
      coding_order(_coding_order),
      share_subsym_lut_flag(),
      share_subsym_prv_flag() {
    if (coding_subsym_size < output_symbol_size && coding_order > 0) {
        if (transform_ID_subsym == TransformIdSubsym::LUT_TRANSFORM) {
            share_subsym_lut_flag = _share_subsym_lut_flag;
        }
        share_subsym_prv_flag = _share_subsym_prv_flag;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

SupportValues::SupportValues(TransformIdSubsym transformIdSubsym, util::BitReader &reader) {
    output_symbol_size = reader.read<uint8_t>(6);
    coding_subsym_size = reader.read<uint8_t>(6);
    coding_order = reader.read<uint8_t>(2);
    if (coding_subsym_size < output_symbol_size && coding_order > 0) {
        if (transformIdSubsym == TransformIdSubsym::LUT_TRANSFORM) {
            share_subsym_lut_flag = reader.read(1);
        }
        share_subsym_prv_flag = reader.read(1);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SupportValues::getOutputSymbolSize() const { return output_symbol_size; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SupportValues::getCodingSubsymSize() const { return coding_subsym_size; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SupportValues::getCodingOrder() const { return coding_order; }

// ---------------------------------------------------------------------------------------------------------------------

bool SupportValues::getShareSubsymLutFlag() const { return *share_subsym_lut_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool SupportValues::getShareSubsymPrvFlag() const { return *share_subsym_prv_flag; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SupportValues::getMinimalSizeInBytes(uint8_t sizeInBit) const {
    return (sizeInBit / 8) + (sizeInBit % 8) ? 1 : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
