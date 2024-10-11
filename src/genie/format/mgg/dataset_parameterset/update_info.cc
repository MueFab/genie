/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_parameterset/update_info.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_parameterset {

// ---------------------------------------------------------------------------------------------------------------------

bool UpdateInfo::operator==(const UpdateInfo& other) const {
    return multiple_alignment_flag == other.multiple_alignment_flag && pos_40_bits_flag == other.pos_40_bits_flag &&
           alphabetId == other.alphabetId && u_signature == other.u_signature;
}

// ---------------------------------------------------------------------------------------------------------------------

UpdateInfo::UpdateInfo(bool _multiple_alignment_flag, bool _pos_40_bits_flag, core::AlphabetID _alphabetId)
    : multiple_alignment_flag(_multiple_alignment_flag), pos_40_bits_flag(_pos_40_bits_flag), alphabetId(_alphabetId) {}

// ---------------------------------------------------------------------------------------------------------------------

UpdateInfo::UpdateInfo(util::BitReader& reader) {
    multiple_alignment_flag = reader.read<bool>(1);
    pos_40_bits_flag = reader.read<bool>(1);
    alphabetId = reader.read<core::AlphabetID>(8);
    if (reader.read<bool>(1)) {
        u_signature = USignature(reader);
    }
    reader.flushHeldBits();
}

// ---------------------------------------------------------------------------------------------------------------------

void UpdateInfo::write(util::BitWriter& writer) const {
    writer.writeBits(multiple_alignment_flag, 1);
    writer.writeBits(pos_40_bits_flag, 1);
    writer.writeBits(static_cast<uint8_t>(alphabetId), 8);
    writer.writeBits(u_signature != std::nullopt, 1);
    if (u_signature != std::nullopt) {
        u_signature->write(writer);
    }
    writer.flushBits();
}

// ---------------------------------------------------------------------------------------------------------------------

void UpdateInfo::addUSignature(USignature signature) { u_signature = signature; }

// ---------------------------------------------------------------------------------------------------------------------

bool UpdateInfo::getMultipleAlignmentFlag() const { return multiple_alignment_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool UpdateInfo::getPos40BitsFlag() const { return pos_40_bits_flag; }

// ---------------------------------------------------------------------------------------------------------------------

core::AlphabetID UpdateInfo::getAlphabetID() const { return alphabetId; }

// ---------------------------------------------------------------------------------------------------------------------

bool UpdateInfo::hasUSignature() const { return u_signature != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

const USignature& UpdateInfo::getUSignature() const { return *u_signature; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_parameterset

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
