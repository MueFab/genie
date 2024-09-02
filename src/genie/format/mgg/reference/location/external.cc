/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/reference/location/external.h"
#include <utility>
#include "genie/format/mgg/reference/location/external/fasta.h"
#include "genie/format/mgg/reference/location/external/mpeg.h"
#include "genie/format/mgg/reference/location/external/raw.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference::location {

// ---------------------------------------------------------------------------------------------------------------------

const size_t External::checksum_sizes[] = {128 / 8, 256 / 8};

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Location> External::factory(genie::util::BitReader& reader, uint8_t _reserved, size_t seq_count,
                                            genie::core::MPEGMinorVersion _version) {
    std::string _ref_uri;
    reader.readBypass_null_terminated(_ref_uri);
    auto _checksum_algo = reader.readBypassBE<ChecksumAlgorithm>();
    auto _ref_type = reader.readBypassBE<RefType>();
    switch (_ref_type) {
        case RefType::MPEGG_REF:
            return std::make_unique<external::MPEG>(reader, _reserved, std::move(_ref_uri), _checksum_algo,
                                                            seq_count, _version);
        case RefType::RAW_REF:
            return std::make_unique<external::Raw>(reader, _reserved, std::move(_ref_uri), _checksum_algo,
                                                           seq_count);
        case RefType::FASTA_REF:
            return std::make_unique<external::Fasta>(reader, _reserved, std::move(_ref_uri), _checksum_algo,
                                                             seq_count);
        default:
            UTILS_DIE("Unknown ref type");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

External::External(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, RefType type)
    : Location(_reserved, true), uri(std::move(_uri)), checksum_algo(algo), reference_type(type) {}

// ---------------------------------------------------------------------------------------------------------------------

External::External(genie::util::BitReader& reader) : Location(reader) {
    reader.readBypass_null_terminated(uri);
    checksum_algo = reader.readBypassBE<ChecksumAlgorithm>();
    reference_type = reader.readBypassBE<RefType>();
}

// ---------------------------------------------------------------------------------------------------------------------

External::External(genie::util::BitReader& reader, uint8_t _reserved) : Location(_reserved, true) {
    reader.readBypass_null_terminated(uri);
    checksum_algo = reader.readBypassBE<ChecksumAlgorithm>();
    reference_type = reader.readBypassBE<RefType>();
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& External::getURI() const { return uri; }

// ---------------------------------------------------------------------------------------------------------------------

External::ChecksumAlgorithm External::getChecksumAlgorithm() const { return checksum_algo; }

// ---------------------------------------------------------------------------------------------------------------------

External::RefType External::getReferenceType() const { return reference_type; }

// ---------------------------------------------------------------------------------------------------------------------

void External::write(genie::util::BitWriter& writer) {
    Location::write(writer);
    writer.writeBypass(uri.data(), uri.length());
    writer.writeBypassBE('\0');
    writer.writeBypassBE(checksum_algo);
    writer.writeBypassBE(reference_type);
}

// ---------------------------------------------------------------------------------------------------------------------

std::string& External::getURI() { return uri; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
