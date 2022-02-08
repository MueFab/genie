/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/reference_location.h"
#include <utility>
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

const size_t ExternalReferenceLocation::checksum_sizes[] = {128 / 8, 256 / 8};

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<ReferenceLocation> ReferenceLocation::referenceLocationFactory(genie::util::BitReader& reader,
                                                                               size_t seq_count,
                                                                               genie::core::MPEGMinorVersion _version) {
    auto _reserved = reader.read<uint8_t>(7);
    bool _external_ref_flag = reader.read<bool>(1);
    if (!_external_ref_flag) {
        return genie::util::make_unique<InternalReferenceLocation>(reader, _reserved);
    } else {
        return ExternalReferenceLocation::externalReferenceLocationFactory(reader, _reserved, seq_count, _version);
    }
}

std::unique_ptr<ReferenceLocation> ReferenceLocation::referenceLocationFactory(
    std::unique_ptr<genie::core::meta::RefBase> base, genie::core::MPEGMinorVersion _version) {
    if (dynamic_cast<genie::core::meta::external_ref::MPEG*>(base.get()) != nullptr) {
        auto ref = dynamic_cast<genie::core::meta::external_ref::MPEG*>(base.get());
        auto ret = genie::util::make_unique<ExternalReferenceLocationMPEGG>(
            0, std::move(ref->getURI()),
            genie::format::mgg::ExternalReferenceLocation::ChecksumAlgorithm(ref->getChecksumAlgo()),
            static_cast<uint8_t>(ref->getGroupID()), ref->getID(), std::move(ref->getChecksum()), _version);
        return ret;
    } else if (dynamic_cast<genie::core::meta::external_ref::Fasta*>(base.get()) != nullptr) {
        auto ref = dynamic_cast<genie::core::meta::external_ref::Fasta*>(base.get());
        auto ret = genie::util::make_unique<ExternalReferenceLocationFasta>(
            0, std::move(ref->getURI()),
            genie::format::mgg::ExternalReferenceLocation::ChecksumAlgorithm(ref->getChecksumAlgo()));
        for (auto& s : ref->getChecksums()) {
            ret->addChecksum(std::move(s));
        }
        return ret;
    } else if (dynamic_cast<genie::core::meta::external_ref::Raw*>(base.get()) != nullptr) {
        auto ref = dynamic_cast<genie::core::meta::external_ref::Raw*>(base.get());
        auto ret = genie::util::make_unique<ExternalReferenceLocationRaw>(
            0, std::move(ref->getURI()),
            genie::format::mgg::ExternalReferenceLocation::ChecksumAlgorithm(ref->getChecksumAlgo()));
        for (auto& s : ref->getChecksums()) {
            ret->addChecksum(std::move(s));
        }
        return ret;
    } else if (dynamic_cast<genie::core::meta::InternalRef*>(base.get()) != nullptr) {
        auto ref = dynamic_cast<genie::core::meta::InternalRef*>(base.get());
        return genie::util::make_unique<InternalReferenceLocation>(0, ref->getGroupID(), ref->getID());
    } else {
        UTILS_DIE("Unknown reference location type");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<ExternalReferenceLocation> ExternalReferenceLocation::externalReferenceLocationFactory(
    genie::util::BitReader& reader, uint8_t _reserved, size_t seq_count, genie::core::MPEGMinorVersion _version) {
    std::string _ref_uri;
    reader.readBypass_null_terminated(_ref_uri);
    auto _checksum_algo = reader.readBypassBE<ChecksumAlgorithm>();
    auto _ref_type = reader.readBypassBE<RefType>();
    switch (_ref_type) {
        case RefType::MPEGG_REF:
            return genie::util::make_unique<ExternalReferenceLocationMPEGG>(reader, _reserved, std::move(_ref_uri),
                                                                            _checksum_algo, seq_count, _version);
        case RefType::RAW_REF:
            return genie::util::make_unique<ExternalReferenceLocationRaw>(reader, _reserved, std::move(_ref_uri),
                                                                          _checksum_algo, seq_count);
        case RefType::FASTA_REF:
            return genie::util::make_unique<ExternalReferenceLocationFasta>(reader, _reserved, std::move(_ref_uri),
                                                                            _checksum_algo, seq_count);
        default:
            UTILS_DIE("Unknown ref type");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceLocation::ReferenceLocation(uint8_t _reserved, bool _external_ref_flag)
    : reserved(_reserved), external_ref_flag(_external_ref_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceLocation::ReferenceLocation(genie::util::BitReader& reader) {
    reserved = reader.read<uint8_t>(7);
    external_ref_flag = reader.read<bool>(1);
}

// ---------------------------------------------------------------------------------------------------------------------

bool ReferenceLocation::isExternal() const { return external_ref_flag; }

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceLocation::write(genie::util::BitWriter& writer) {
    writer.write(reserved, 7);
    writer.write(external_ref_flag, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocation::ExternalReferenceLocation(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo,
                                                     RefType type)
    : ReferenceLocation(_reserved, true), uri(std::move(_uri)), checksum_algo(algo), reference_type(type) {}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocation::ExternalReferenceLocation(genie::util::BitReader& reader) : ReferenceLocation(reader) {
    reader.readBypass_null_terminated(uri);
    checksum_algo = reader.readBypassBE<ChecksumAlgorithm>();
    reference_type = reader.readBypassBE<RefType>();
}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocation::ExternalReferenceLocation(genie::util::BitReader& reader, uint8_t _reserved)
    : ReferenceLocation(_reserved, true) {
    reader.readBypass_null_terminated(uri);
    checksum_algo = reader.readBypassBE<ChecksumAlgorithm>();
    reference_type = reader.readBypassBE<RefType>();
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ExternalReferenceLocation::getURI() const { return uri; }

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocation::ChecksumAlgorithm ExternalReferenceLocation::getChecksumAlgorithm() const {
    return checksum_algo;
}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocation::RefType ExternalReferenceLocation::getReferenceType() const { return reference_type; }

// ---------------------------------------------------------------------------------------------------------------------

void ExternalReferenceLocation::write(genie::util::BitWriter& writer) {
    ReferenceLocation::write(writer);
    writer.writeBypass(uri.data(), uri.length());
    writer.writeBypassBE('\0');
    writer.writeBypassBE(checksum_algo);
    writer.writeBypassBE(reference_type);
}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocationMPEGG::ExternalReferenceLocationMPEGG(uint8_t _reserved, std::string _uri,
                                                               ChecksumAlgorithm algo, uint8_t _group_id,
                                                               uint16_t _dataset_id, std::string _ref_checksum,
                                                               genie::core::MPEGMinorVersion _version)
    : ExternalReferenceLocation(_reserved, std::move(_uri), algo, RefType::MPEGG_REF),
      version(_version),
      external_dataset_group_id(_group_id),
      external_dataset_id(_dataset_id),
      ref_checksum(std::move(_ref_checksum)) {
    UTILS_DIE_IF(version == genie::core::MPEGMinorVersion::V1900 &&
                     ref_checksum.size() != checksum_sizes[static_cast<uint8_t>(algo)],
                 "Invalid reference checksum");
}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocationMPEGG::ExternalReferenceLocationMPEGG(genie::util::BitReader& reader, size_t seq_count,
                                                               genie::core::MPEGMinorVersion _version)
    : ExternalReferenceLocation(reader),
      version(_version),
      ref_checksum(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0') {
    external_dataset_group_id = reader.readBypassBE<uint8_t>();
    external_dataset_id = reader.readBypassBE<uint16_t>();
    if (version == core::MPEGMinorVersion::V1900) {
        reader.readBypass(ref_checksum);
    } else {
        ref_checksum.clear();
        for (size_t i = 0; i < seq_count; ++i) {
            seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
            reader.readBypass(seq_checksums.back());
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocationMPEGG::ExternalReferenceLocationMPEGG(genie::util::BitReader& reader, uint8_t _reserved,
                                                               std::string _uri, ChecksumAlgorithm algo,
                                                               size_t seq_count, genie::core::MPEGMinorVersion _version)
    : ExternalReferenceLocation(_reserved, std::move(_uri), algo, RefType::MPEGG_REF),
      version(_version),
      ref_checksum(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0') {
    external_dataset_group_id = reader.readBypassBE<uint8_t>();
    external_dataset_id = reader.readBypassBE<uint16_t>();
    if (version == core::MPEGMinorVersion::V1900) {
        reader.readBypass(ref_checksum);
    } else {
        ref_checksum.clear();
        for (size_t i = 0; i < seq_count; ++i) {
            seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
            reader.readBypass(seq_checksums.back());
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ExternalReferenceLocationMPEGG::getExternalDatasetGroupID() const { return external_dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t ExternalReferenceLocationMPEGG::getExternalDatasetID() const { return external_dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ExternalReferenceLocationMPEGG::getRefChecksum() const { return ref_checksum; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& ExternalReferenceLocationMPEGG::getSeqChecksums() const { return seq_checksums; }

// ---------------------------------------------------------------------------------------------------------------------

void ExternalReferenceLocationMPEGG::addSeqChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    seq_checksums.emplace_back(std::move(checksum));
}

// ---------------------------------------------------------------------------------------------------------------------

void ExternalReferenceLocationMPEGG::write(genie::util::BitWriter& writer) {
    ExternalReferenceLocation::write(writer);
    writer.writeBypassBE(external_dataset_group_id);
    writer.writeBypassBE(external_dataset_id);
    if (version == core::MPEGMinorVersion::V1900) {
        writer.writeBypass(ref_checksum.data(), ref_checksum.length());
    } else {
        for (const auto& s : seq_checksums) {
            writer.writeBypass(s.data(), s.length());
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ExternalReferenceLocationMPEGG::addChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    if (version != core::MPEGMinorVersion::V1900) {
        seq_checksums.emplace_back(std::move(checksum));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocationRaw::ExternalReferenceLocationRaw(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo)
    : ExternalReferenceLocation(_reserved, std::move(_uri), algo, RefType::RAW_REF) {}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocationRaw::ExternalReferenceLocationRaw(genie::util::BitReader& reader, size_t seq_count)
    : ExternalReferenceLocation(reader) {
    for (size_t i = 0; i < seq_count; ++i) {
        seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
        reader.readBypass(seq_checksums.back());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocationRaw::ExternalReferenceLocationRaw(genie::util::BitReader& reader, uint8_t _reserved,
                                                           std::string _uri, ChecksumAlgorithm algo, size_t seq_count)
    : ExternalReferenceLocation(_reserved, std::move(_uri), algo, RefType::RAW_REF) {
    for (size_t i = 0; i < seq_count; ++i) {
        seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
        reader.readBypass(seq_checksums.back());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& ExternalReferenceLocationRaw::getSeqChecksums() const { return seq_checksums; }

// ---------------------------------------------------------------------------------------------------------------------

void ExternalReferenceLocationRaw::addSeqChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    seq_checksums.emplace_back(std::move(checksum));
}

// ---------------------------------------------------------------------------------------------------------------------

void ExternalReferenceLocationRaw::write(genie::util::BitWriter& writer) {
    ExternalReferenceLocation::write(writer);
    for (const auto& s : seq_checksums) {
        writer.writeBypass(s.data(), s.length());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ExternalReferenceLocationRaw::addChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    seq_checksums.emplace_back(std::move(checksum));
}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocationFasta::ExternalReferenceLocationFasta(uint8_t _reserved, std::string _uri,
                                                               ChecksumAlgorithm algo)
    : ExternalReferenceLocation(_reserved, std::move(_uri), algo, RefType::RAW_REF) {}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocationFasta::ExternalReferenceLocationFasta(genie::util::BitReader& reader, size_t seq_count)
    : ExternalReferenceLocation(reader) {
    for (size_t i = 0; i < seq_count; ++i) {
        seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
        reader.readBypass(seq_checksums.back());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReferenceLocationFasta::ExternalReferenceLocationFasta(genie::util::BitReader& reader, uint8_t _reserved,
                                                               std::string _uri, ChecksumAlgorithm algo,
                                                               size_t seq_count)
    : ExternalReferenceLocation(_reserved, std::move(_uri), algo, RefType::RAW_REF) {
    for (size_t i = 0; i < seq_count; ++i) {
        seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
        reader.readBypass(seq_checksums.back());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& ExternalReferenceLocationFasta::getSeqChecksums() const { return seq_checksums; }

// ---------------------------------------------------------------------------------------------------------------------

void ExternalReferenceLocationFasta::addSeqChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    seq_checksums.emplace_back(std::move(checksum));
}

// ---------------------------------------------------------------------------------------------------------------------

void ExternalReferenceLocationFasta::write(genie::util::BitWriter& writer) {
    ExternalReferenceLocation::write(writer);
    for (const auto& s : seq_checksums) {
        writer.writeBypass(s.data(), s.length());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ExternalReferenceLocationFasta::addChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    seq_checksums.emplace_back(std::move(checksum));
}

// ---------------------------------------------------------------------------------------------------------------------

InternalReferenceLocation::InternalReferenceLocation(uint8_t _reserved, uint8_t _internal_dataset_group_id,
                                                     uint16_t _internal_dataset_id)
    : ReferenceLocation(_reserved, false),
      internal_dataset_group_id(_internal_dataset_group_id),
      internal_dataset_id(_internal_dataset_id) {}

// ---------------------------------------------------------------------------------------------------------------------

InternalReferenceLocation::InternalReferenceLocation(genie::util::BitReader& reader) : ReferenceLocation(reader) {
    internal_dataset_group_id = reader.readBypassBE<uint8_t>();
    internal_dataset_id = reader.readBypassBE<uint16_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

InternalReferenceLocation::InternalReferenceLocation(genie::util::BitReader& reader, uint8_t _reserved)
    : ReferenceLocation(_reserved, false) {
    internal_dataset_group_id = reader.readBypassBE<uint8_t>();
    internal_dataset_id = reader.readBypassBE<uint16_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t InternalReferenceLocation::getDatasetGroupID() const { return internal_dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t InternalReferenceLocation::getDatasetID() const { return internal_dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

void InternalReferenceLocation::write(genie::util::BitWriter& writer) {
    ReferenceLocation::write(writer);
    writer.writeBypassBE(internal_dataset_group_id);
    writer.writeBypassBE(internal_dataset_id);
}

// ---------------------------------------------------------------------------------------------------------------------

std::string& ExternalReferenceLocation::getURI() { return uri; }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::meta::RefBase> ExternalReferenceLocationMPEGG::decapsulate() {
    auto ret = genie::util::make_unique<genie::core::meta::external_ref::MPEG>(
        std::move(getURI()), genie::core::meta::ExternalRef::ChecksumAlgorithm(getChecksumAlgorithm()),
        external_dataset_group_id, external_dataset_id, std::move(ref_checksum));
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::meta::RefBase> ExternalReferenceLocationRaw::decapsulate() {
    auto ret = genie::util::make_unique<genie::core::meta::external_ref::Raw>(
        std::move(getURI()), genie::core::meta::ExternalRef::ChecksumAlgorithm(getChecksumAlgorithm()));

    for (auto& s : seq_checksums) {
        ret->addChecksum(std::move(s));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::meta::RefBase> ExternalReferenceLocationFasta::decapsulate() {
    auto ret = genie::util::make_unique<genie::core::meta::external_ref::Fasta>(
        std::move(getURI()), genie::core::meta::ExternalRef::ChecksumAlgorithm(getChecksumAlgorithm()));

    for (auto& s : seq_checksums) {
        ret->addChecksum(std::move(s));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::meta::RefBase> InternalReferenceLocation::decapsulate() {
    auto ret = genie::util::make_unique<genie::core::meta::InternalRef>(internal_dataset_group_id, internal_dataset_id);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
