/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference/location.h"
#include <memory>
#include <utility>
#include "genie/format/mgg/reference/location/external.h"
#include "genie/format/mgg/reference/location/external/fasta.h"
#include "genie/format/mgg/reference/location/external/mpeg.h"
#include "genie/format/mgg/reference/location/external/raw.h"
#include "genie/format/mgg/reference/location/internal.h"

#include "genie/util/runtime-exception.h"
#include "genie/util/string-helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference {

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Location> Location::factory(util::BitReader& reader, const size_t seq_count,
                                            const core::MPEGMinorVersion _version) {
    auto _reserved = reader.read<uint8_t>(7);
    const bool _external_ref_flag = reader.read<bool>(1);
    if (!_external_ref_flag) {
        return std::make_unique<location::Internal>(reader, _reserved);
    } else {
        return location::External::factory(reader, _reserved, seq_count, _version);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Location> Location::referenceLocationFactory(std::unique_ptr<core::meta::RefBase> base,
                                                             core::MPEGMinorVersion _version) {
    if (dynamic_cast<core::meta::external_ref::MPEG*>(base.get()) != nullptr) {
        const auto ref = dynamic_cast<core::meta::external_ref::MPEG*>(base.get());
        auto ret = std::make_unique<location::external::MPEG>(
            static_cast<uint8_t>(0), std::move(ref->getURI()),
            static_cast<location::External::ChecksumAlgorithm>(ref->getChecksumAlgo()),
            static_cast<uint8_t>(ref->getGroupID()), ref->getID(), std::move(ref->getChecksum()), _version);
        return ret;
    } else if (dynamic_cast<core::meta::external_ref::Fasta*>(base.get()) != nullptr) {
        const auto ref = dynamic_cast<core::meta::external_ref::Fasta*>(base.get());
        auto ret = std::make_unique<location::external::Fasta>(
            static_cast<uint8_t>(0), std::move(ref->getURI()),
            static_cast<location::External::ChecksumAlgorithm>(ref->getChecksumAlgo()));
        for (const auto& s : ref->getChecksums()) {
            ret->addChecksum(s);
        }
        return ret;
    } else if (dynamic_cast<core::meta::external_ref::Raw*>(base.get()) != nullptr) {
        const auto ref = dynamic_cast<core::meta::external_ref::Raw*>(base.get());
        auto ret = std::make_unique<location::external::Raw>(
            static_cast<uint8_t>(0), std::move(ref->getURI()),
            static_cast<location::External::ChecksumAlgorithm>(ref->getChecksumAlgo()));
        for (const auto& s : ref->getChecksums()) {
            ret->addChecksum(s);
        }
        return ret;
    } else if (dynamic_cast<core::meta::InternalRef*>(base.get()) != nullptr) {
        const auto ref = dynamic_cast<core::meta::InternalRef*>(base.get());
        return std::make_unique<location::Internal>(static_cast<uint8_t>(0), static_cast<uint8_t>(ref->getGroupID()),
                                                    ref->getID());
    } else {
        UTILS_DIE("Unknown reference location type");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Location::Location(const uint8_t _reserved, const bool _external_ref_flag)
    : reserved(_reserved), external_ref_flag(_external_ref_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

Location::Location(util::BitReader& reader) {
    reserved = reader.read<uint8_t>(7);
    external_ref_flag = reader.read<bool>(1);
}

// ---------------------------------------------------------------------------------------------------------------------

bool Location::isExternal() const { return external_ref_flag; }

// ---------------------------------------------------------------------------------------------------------------------

void Location::write(util::BitWriter& writer) {
    writer.writeBits(reserved, 7);
    writer.writeBits(external_ref_flag, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
