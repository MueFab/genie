/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/dataset_header/u_options.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace dataset_header {

// ---------------------------------------------------------------------------------------------------------------------

bool UOptions::operator==(const UOptions& other) const {
    return reserved1 == other.reserved1 && u_signature == other.u_signature && reserved2 == other.reserved2 &&
           reserved3 == other.reserved3;
}

// ---------------------------------------------------------------------------------------------------------------------

UOptions::UOptions(uint64_t _reserved1, bool _reserved3)
    : reserved1(_reserved1), u_signature(std::nullopt), reserved2(std::nullopt), reserved3(_reserved3) {}

// ---------------------------------------------------------------------------------------------------------------------

UOptions::UOptions(genie::util::BitReader& reader) : reserved3(false) {
    reserved1 = reader.read<uint64_t>(62);
    bool U_signature_flag = reader.read<bool>(1);
    if (U_signature_flag) {
        u_signature = USignature(reader);
    }
    bool reserved_flag = reader.read<bool>(1);
    if (reserved_flag) {
        reserved2 = reader.read<uint8_t>(8);
    }
    //  reserved3 = reader.read<bool>(1); // TODO(muenteferi): Reference decoder and reference bitstreams contradict
    //  document
}

// ---------------------------------------------------------------------------------------------------------------------

void UOptions::write(genie::util::BitWriter& writer) const {
    writer.write(reserved1, 62);
    writer.write(hasSignature(), 1);
    if (hasSignature()) {
        u_signature->write(writer);
    }
    writer.write(hasReserved2(), 1);
    if (hasReserved2()) {
        writer.write(getReserved2(), 8);
    }
    writer.write(reserved3, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t UOptions::getReserved1() const { return reserved1; }

// ---------------------------------------------------------------------------------------------------------------------

bool UOptions::getReserved3() const { return reserved3; }

// ---------------------------------------------------------------------------------------------------------------------

bool UOptions::hasReserved2() const { return reserved2 != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t UOptions::getReserved2() const { return *reserved2; }

// ---------------------------------------------------------------------------------------------------------------------

bool UOptions::hasSignature() const { return u_signature != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

const USignature& UOptions::getSignature() const { return *u_signature; }

// ---------------------------------------------------------------------------------------------------------------------

void UOptions::addSignature(USignature s) { u_signature = s; }

// ---------------------------------------------------------------------------------------------------------------------

void UOptions::addReserved2(uint8_t r) { reserved2 = r; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace dataset_header
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
