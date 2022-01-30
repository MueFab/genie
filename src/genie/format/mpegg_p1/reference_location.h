/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_REFERENCE_LOCATION_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_REFERENCE_LOCATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class ReferenceLocation {
 private:
    uint8_t reserved;
    bool external_ref_flag;

 public:
    virtual ~ReferenceLocation() = default;

    explicit ReferenceLocation(uint8_t _reserved, bool _external_ref_flag)
        : reserved(_reserved), external_ref_flag(_external_ref_flag) {}

    explicit ReferenceLocation(genie::util::BitReader& reader) {
        reserved = reader.read<uint8_t>(7);
        external_ref_flag = reader.read<bool>(1);
    }

    bool isExternal() const { return external_ref_flag; }

    static std::unique_ptr<ReferenceLocation> referenceLocationFactory(
        genie::util::BitReader& reader, size_t seq_count,
        genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000);

    virtual void write(genie::util::BitWriter& writer) {
        writer.write(reserved, 7);
        writer.write(external_ref_flag, 1);
    }
};

class ExternalReferenceLocation : public ReferenceLocation {
 public:
    enum class ChecksumAlgorithm : uint8_t { MD5 = 0, SHA256 = 1 };

    constexpr static const size_t checksum_sizes[] = {128 / 8, 256 / 8};

    enum class RefType : uint8_t { MPEGG_REF = 0, RAW_REF = 1, FASTA_REF = 2 };

 private:
    std::string uri;
    ChecksumAlgorithm checksum_algo;
    RefType reference_type;

 public:
    ExternalReferenceLocation(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, RefType type)
        : ReferenceLocation(_reserved, true), uri(std::move(_uri)), checksum_algo(algo), reference_type(type) {}

    explicit ExternalReferenceLocation(genie::util::BitReader& reader) : ReferenceLocation(reader) {
        reader.readBypass_null_terminated(uri);
        checksum_algo = reader.readBypassBE<ChecksumAlgorithm>();
        reference_type = reader.readBypassBE<RefType>();
    }

    ExternalReferenceLocation(genie::util::BitReader& reader, uint8_t _reserved) : ReferenceLocation(_reserved, true) {
        reader.readBypass_null_terminated(uri);
        checksum_algo = reader.readBypassBE<ChecksumAlgorithm>();
        reference_type = reader.readBypassBE<RefType>();
    }

    const std::string& getURI() const { return uri; }

    ChecksumAlgorithm getChecksumAlgorithm() const { return checksum_algo; }

    RefType getReferenceType() const { return reference_type; }

    static std::unique_ptr<ExternalReferenceLocation> externalReferenceLocationFactory(
        genie::util::BitReader& reader, uint8_t _reserved, size_t seq_count,
        genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000);

    void write(genie::util::BitWriter& writer) override {
        ReferenceLocation::write(writer);
        writer.writeBypass(uri.data(), uri.length());
        writer.writeBypassBE('\0');
        writer.writeBypassBE(checksum_algo);
        writer.writeBypassBE(reference_type);
    }

    virtual void addChecksum(std::string checksum) = 0;
};

class ExternalReferenceLocationMPEGG : public ExternalReferenceLocation {
 private:
    genie::core::MPEGMinorVersion version;
    uint8_t external_dataset_group_id;
    uint16_t external_dataset_id;
    std::string ref_checksum;
    std::vector<std::string> seq_checksums;

 public:
    ExternalReferenceLocationMPEGG(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, uint8_t _group_id,
                                   uint16_t _dataset_id, std::string _ref_checksum = "",
                                   genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000)
        : ExternalReferenceLocation(_reserved, std::move(_uri), algo, RefType::MPEGG_REF),
          version(_version),
          external_dataset_group_id(_group_id),
          external_dataset_id(_dataset_id),
          ref_checksum(std::move(_ref_checksum)) {
        UTILS_DIE_IF(version == genie::core::MPEGMinorVersion::V1900 &&
                         ref_checksum.size() != checksum_sizes[static_cast<uint8_t>(algo)],
                     "Invalid reference checksum");
    }

    explicit ExternalReferenceLocationMPEGG(
        genie::util::BitReader& reader, size_t seq_count,
        genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000)
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

    explicit ExternalReferenceLocationMPEGG(
        genie::util::BitReader& reader, uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, size_t seq_count,
        genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000)
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

    uint8_t getExternalDatasetGroupID() const { return external_dataset_group_id; }

    uint16_t getExternalDatasetID() const { return external_dataset_id; }

    const std::string& getRefChecksum() const { return ref_checksum; }

    const std::vector<std::string>& getSeqChecksums() const { return seq_checksums; }

    void addSeqChecksum(std::string checksum) {
        UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                     "Invalid checksum length");
        seq_checksums.emplace_back(std::move(checksum));
    }

    void write(genie::util::BitWriter& writer) override {
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

    void addChecksum(std::string checksum) override {
        UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                     "Invalid checksum length");
        if (version != core::MPEGMinorVersion::V1900) {
            seq_checksums.emplace_back(std::move(checksum));
        }
    }
};

class ExternalReferenceLocationRaw : public ExternalReferenceLocation {
 private:
    std::vector<std::string> seq_checksums;

 public:
    ExternalReferenceLocationRaw(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo)
        : ExternalReferenceLocation(_reserved, std::move(_uri), algo, RefType::RAW_REF) {}

    ExternalReferenceLocationRaw(genie::util::BitReader& reader, size_t seq_count) : ExternalReferenceLocation(reader) {
        for (size_t i = 0; i < seq_count; ++i) {
            seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
            reader.readBypass(seq_checksums.back());
        }
    }

    ExternalReferenceLocationRaw(genie::util::BitReader& reader, uint8_t _reserved, std::string _uri,
                                 ChecksumAlgorithm algo, size_t seq_count)
        : ExternalReferenceLocation(_reserved, std::move(_uri), algo, RefType::RAW_REF) {
        for (size_t i = 0; i < seq_count; ++i) {
            seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
            reader.readBypass(seq_checksums.back());
        }
    }

    const std::vector<std::string>& getSeqChecksums() const { return seq_checksums; }

    void addSeqChecksum(std::string checksum) {
        UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                     "Invalid checksum length");
        seq_checksums.emplace_back(std::move(checksum));
    }

    void write(genie::util::BitWriter& writer) override {
        ExternalReferenceLocation::write(writer);
        for (const auto& s : seq_checksums) {
            writer.writeBypass(s.data(), s.length());
        }
    }

    void addChecksum(std::string checksum) override {
        UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                     "Invalid checksum length");
        seq_checksums.emplace_back(std::move(checksum));
    }
};

class ExternalReferenceLocationFasta : public ExternalReferenceLocation {
 private:
    std::vector<std::string> seq_checksums;

 public:
    ExternalReferenceLocationFasta(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo)
        : ExternalReferenceLocation(_reserved, std::move(_uri), algo, RefType::RAW_REF) {}

    ExternalReferenceLocationFasta(genie::util::BitReader& reader, size_t seq_count)
        : ExternalReferenceLocation(reader) {
        for (size_t i = 0; i < seq_count; ++i) {
            seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
            reader.readBypass(seq_checksums.back());
        }
    }

    ExternalReferenceLocationFasta(genie::util::BitReader& reader, uint8_t _reserved, std::string _uri,
                                   ChecksumAlgorithm algo, size_t seq_count)
        : ExternalReferenceLocation(_reserved, std::move(_uri), algo, RefType::RAW_REF) {
        for (size_t i = 0; i < seq_count; ++i) {
            seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
            reader.readBypass(seq_checksums.back());
        }
    }

    const std::vector<std::string>& getSeqChecksums() const { return seq_checksums; }

    void addSeqChecksum(std::string checksum) {
        UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                     "Invalid checksum length");
        seq_checksums.emplace_back(std::move(checksum));
    }

    void write(genie::util::BitWriter& writer) override {
        ExternalReferenceLocation::write(writer);
        for (const auto& s : seq_checksums) {
            writer.writeBypass(s.data(), s.length());
        }
    }

    void addChecksum(std::string checksum) override {
        UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                     "Invalid checksum length");
        seq_checksums.emplace_back(std::move(checksum));
    }
};

class InternalReferenceLocation : public ReferenceLocation {
 private:
    uint8_t internal_dataset_group_id;
    uint16_t internal_dataset_id;

 public:
    InternalReferenceLocation(uint8_t _reserved, uint8_t _internal_dataset_group_id, uint16_t _internal_dataset_id)
        : ReferenceLocation(_reserved, false),
          internal_dataset_group_id(_internal_dataset_group_id),
          internal_dataset_id(_internal_dataset_id) {}

    explicit InternalReferenceLocation(genie::util::BitReader& reader) : ReferenceLocation(reader) {
        internal_dataset_group_id = reader.readBypassBE<uint8_t>();
        internal_dataset_id = reader.readBypassBE<uint16_t>();
    }

    InternalReferenceLocation(genie::util::BitReader& reader, uint8_t _reserved) : ReferenceLocation(_reserved, false) {
        internal_dataset_group_id = reader.readBypassBE<uint8_t>();
        internal_dataset_id = reader.readBypassBE<uint16_t>();
    }

    uint8_t getDatasetGroupID() const { return internal_dataset_group_id; }

    uint16_t getDatasetID() const { return internal_dataset_id; }

    void write(genie::util::BitWriter& writer) override {
        ReferenceLocation::write(writer);
        writer.writeBypassBE(internal_dataset_group_id);
        writer.writeBypassBE(internal_dataset_id);
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_REFERENCE_LOCATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
