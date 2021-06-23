/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/fields/u_access_unit_info/u_access_unit_info.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

UAccessUnitInfo::UAccessUnitInfo()
    : num_U_clusters(0),
      multiple_signature_base(0),
      U_signature_size(0),
      U_signature_constant_length(false),
      U_signature_length(0) {}

// ---------------------------------------------------------------------------------------------------------------------

UAccessUnitInfo::UAccessUnitInfo(uint32_t _num_U_clusters)
    : num_U_clusters(_num_U_clusters),
      multiple_signature_base(0),
      U_signature_size(0),
      U_signature_constant_length(false),
      U_signature_length(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void UAccessUnitInfo::readUAccessUnitInfo(util::BitReader& reader) {
    //    size_t start_pos = reader.getPos();

    // num_U_clusters u(32), multiple_signature_base u(31)
    num_U_clusters = reader.read<uint32_t>();
    multiple_signature_base = reader.read<uint32_t>(31);

    if (multiple_signature_base > 0) {
        // U_signature_size u(6)
        U_signature_size = reader.read<uint8_t>(6);
    }

    // U_signature_constant_length u(1)
    U_signature_constant_length = reader.read<bool>(1);

    if (U_signature_constant_length) {
        // U_signature_length u(8)
        U_signature_length = reader.read<uint8_t>();
    }

    //    UTILS_DIE_IF(reader.getPos()-start_pos != length, "Invalid readUAccessUnitInfo length!");
}

// ---------------------------------------------------------------------------------------------------------------------

void UAccessUnitInfo::setMultipleSignature(uint32_t base, uint8_t size) {
    multiple_signature_base = base;
    U_signature_size = size;
}

// ---------------------------------------------------------------------------------------------------------------------

void UAccessUnitInfo::setConstantSignature(uint8_t sign_length) {
    U_signature_constant_length = true;
    U_signature_length = sign_length;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t UAccessUnitInfo::getBitLength() const {
    // num_U_clusters u(32), multiple_signature_base u(31)
    uint64_t bitlen = 32 + 31;

    if (multiple_signature_base > 0) {
        // U_signature_size u(6)
        bitlen += 6;
    }

    // U_signature_constant_length u(1)
    bitlen += 1;

    if (U_signature_constant_length) {
        // U_signature_length u(8)
        bitlen += 8;
    }

    return bitlen;
}

// ---------------------------------------------------------------------------------------------------------------------

void UAccessUnitInfo::write(util::BitWriter& bit_writer) const {
    // num_U_clusters u(32)
    bit_writer.write(num_U_clusters, 32);

    // multiple_signature_base u(31)
    bit_writer.write(multiple_signature_base, 31);

    if (multiple_signature_base > 0) {
        // U_signature_size u(6)
        bit_writer.write(U_signature_size, 6);
    }

    // U_signature_constant_length u(1)
    bit_writer.write(U_signature_constant_length, 1);

    if (U_signature_constant_length) {
        // U_signature_constant_length u(8)
        bit_writer.write(U_signature_constant_length, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
