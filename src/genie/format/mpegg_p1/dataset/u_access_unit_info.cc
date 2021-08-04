/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "u_access_unit_info.h"
#include <sstream>

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

UAccessUnitInfo::UAccessUnitInfo(util::BitReader& reader, FileHeader& fhd) :
    num_U_access_units(reader.read<uint32_t>()),
    num_U_clusters(),
    U_signature_flag(false),
    multiple_signature_base(0),
    U_signature_size(0),
    U_signature_constant_length(false),
    U_signature_length(0)
{

    if (num_U_access_units > 0) {
        reader.read<uint64_t>(62);                /// backward compatibility
        U_signature_flag = reader.read<bool>(1);  /// U_signature_flag u(1)
        if (U_signature_flag) {
            /// U_signature_constant_length u(1)
            U_signature_constant_length = reader.read<bool>(1);
            if (U_signature_constant_length) {
                /// U_signature_length u(8)
                U_signature_length = reader.read<uint8_t>();
            }
        }

        reserved_flag = reader.read<bool>(1); /// reserved_flag u(1)

        if (reserved_flag) {
            reader.read<uint8_t>();  /// reserved u(8)
        }
        reader.read<bool>(1);  /// reserved_flag u(8)
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t UAccessUnitInfo::getNumAccessUnits() const {return num_U_access_units; }

// ---------------------------------------------------------------------------------------------------------------------

bool UAccessUnitInfo::getUSignatureFlag() const {return U_signature_flag;}

// ---------------------------------------------------------------------------------------------------------------------

bool UAccessUnitInfo::getUSignatureConstantLength() const {return U_signature_constant_length;}

uint8_t UAccessUnitInfo::getUSignatureLength() const {
    UTILS_DIE_IF(!U_signature_flag,
                 "Cannot request U_signature_length when U_signature_flag is 0!");
    UTILS_DIE_IF(!U_signature_constant_length,
                 "Cannot request U_signature_length when U_signature_constant_length is 0!");
    return U_signature_length;
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

//    /// num_U_access_units u(32)
//    uint64_t bitlen = 32;
//
//    if (num_U_access_units > 0) {
//        bitlen += 62; /// backward compatibility
//        bitlen += 1; /// U_signature_flag u(1)
//        if (U_signature_flag) {
//            bitlen += 1; /// U_signature_constant_length u(1)
//            if (U_signature_constant_length) {
//                bitlen += 8; /// U_signature_length u(8)
//            }
//        }
//
//        bitlen += 1; /// reserved_flag u(1)
//
//        if (reserved_flag) {
//            bitlen += 8; /// reserved u(8)
//        }
//        bitlen += 1;  /// reserved_flag u(1)
//    }
//
//    return bitlen;

    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    write(tmp_writer);
    return tmp_writer.getBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

void UAccessUnitInfo::write(util::BitWriter& writer) const {

    /// num_U_access_units u(32)
    writer.write(num_U_access_units, 32);

    if (num_U_access_units > 0) {
        writer.write(0, 62); /// backward compatibility
        writer.write(U_signature_flag, 1); /// U_signature_flag u(1)
        if (U_signature_flag) {
            writer.write(U_signature_constant_length, 1); /// U_signature_constant_length u(1)
            if (U_signature_constant_length) {
                writer.write(U_signature_length, 8); /// U_signature_length u(8)
            }
        }

        writer.write(reserved_flag, 1); /// reserved_flag u(1)

        if (reserved_flag) {
            writer.write(0, 8);  /// reserved u(8)
        }
        writer.write(0, 1);  /// reserved_flag u(8)
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
