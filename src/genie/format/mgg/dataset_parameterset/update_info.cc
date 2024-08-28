/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/dataset_parameterset/update_info.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace dataset_parameterset {

// ---------------------------------------------------------------------------------------------------------------------

bool UpdateInfo::operator==(const UpdateInfo& other) const {
    return multiple_alignment_flag == other.multiple_alignment_flag && pos_40_bits_flag == other.pos_40_bits_flag &&
           alphabetId == other.alphabetId && u_signature == other.u_signature;
}

// ---------------------------------------------------------------------------------------------------------------------

UpdateInfo::UpdateInfo(bool _multiple_alignment_flag, bool _pos_40_bits_flag, core::AlphabetID _alphabetId)
    : multiple_alignment_flag(_multiple_alignment_flag), pos_40_bits_flag(_pos_40_bits_flag), alphabetId(_alphabetId) {}

// ---------------------------------------------------------------------------------------------------------------------

UpdateInfo::UpdateInfo(genie::util::BitReader& reader) {
    multiple_alignment_flag = reader.read<bool>(1);
    pos_40_bits_flag = reader.read<bool>(1);
    alphabetId = reader.read<core::AlphabetID>(8);
    if (reader.read<bool>(1)) {
        u_signature = USignature(reader);
    }
    reader.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

void UpdateInfo::write(genie::util::BitWriter& writer) const {
    writer.write(multiple_alignment_flag, 1);
    writer.write(pos_40_bits_flag, 1);
    writer.write(static_cast<uint8_t>(alphabetId), 8);
    writer.write(u_signature != std::nullopt, 1);
    if (u_signature != std::nullopt) {
        u_signature->write(writer);
    }
    writer.flush();
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

}  // namespace dataset_parameterset
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
