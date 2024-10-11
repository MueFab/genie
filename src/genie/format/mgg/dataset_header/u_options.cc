/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_header/u_options.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

// ---------------------------------------------------------------------------------------------------------------------

bool UOptions::operator==(const UOptions& other) const {
    return reserved1 == other.reserved1 && u_signature == other.u_signature && reserved2 == other.reserved2 &&
           reserved3 == other.reserved3;
}

// ---------------------------------------------------------------------------------------------------------------------

UOptions::UOptions(const uint64_t _reserved1, const bool _reserved3)
    : reserved1(_reserved1), u_signature(std::nullopt), reserved2(std::nullopt), reserved3(_reserved3) {}

// ---------------------------------------------------------------------------------------------------------------------

UOptions::UOptions(util::BitReader& reader) : reserved3(false) {
    reserved1 = reader.read<uint64_t>(62);
    const bool U_signature_flag = reader.read<bool>(1);
    if (U_signature_flag) {
        u_signature = USignature(reader);
    }
    const bool reserved_flag = reader.read<bool>(1);
    if (reserved_flag) {
        reserved2 = reader.read<uint8_t>(8);
    }
    //  reserved3 = reader.read<bool>(1); // TODO(muenteferi): Reference decoder and reference bitstreams contradict
    //  document
}

// ---------------------------------------------------------------------------------------------------------------------

void UOptions::write(util::BitWriter& writer) const {
    writer.writeBits(reserved1, 62);
    writer.writeBits(hasSignature(), 1);
    if (hasSignature()) {
        u_signature->write(writer);
    }
    writer.writeBits(hasReserved2(), 1);
    if (hasReserved2()) {
        writer.writeBits(getReserved2(), 8);
    }
    writer.writeBits(reserved3, 1);
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

}  // namespace genie::format::mgg::dataset_header

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
