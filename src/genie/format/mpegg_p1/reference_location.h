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

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class ReferenceLocation {
 private:
    uint8_t reserved;        //!< @brief
    bool external_ref_flag;  //!< @brief

 public:
    /**
     * @brief
     */
    virtual ~ReferenceLocation() = default;

    /**
     * @brief
     * @param _reserved
     * @param _external_ref_flag
     */
    explicit ReferenceLocation(uint8_t _reserved, bool _external_ref_flag);

    /**
     * @brief
     * @param reader
     */
    explicit ReferenceLocation(genie::util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    bool isExternal() const;

    /**
     * @brief
     * @param reader
     * @param seq_count
     * @param _version
     * @return
     */
    static std::unique_ptr<ReferenceLocation> referenceLocationFactory(
        genie::util::BitReader& reader, size_t seq_count,
        genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000);

    /**
     * @brief
     * @param writer
     */
    virtual void write(genie::util::BitWriter& writer);
};

/**
 * @brief
 */
class ExternalReferenceLocation : public ReferenceLocation {
 public:
    /**
     * @brief
     */
    enum class ChecksumAlgorithm : uint8_t { MD5 = 0, SHA256 = 1 };

    /**
     * @brief
     */
    constexpr static const size_t checksum_sizes[] = {128 / 8, 256 / 8};

    /**
     * @brief
     */
    enum class RefType : uint8_t { MPEGG_REF = 0, RAW_REF = 1, FASTA_REF = 2 };

 private:
    std::string uri;                  //!< @brief
    ChecksumAlgorithm checksum_algo;  //!< @brief
    RefType reference_type;           //!< @brief

 public:
    /**
     * @brief
     * @param _reserved
     * @param _uri
     * @param algo
     * @param type
     */
    ExternalReferenceLocation(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, RefType type);

    /**
     * @brief
     * @param reader
     */
    explicit ExternalReferenceLocation(genie::util::BitReader& reader);

    /**
     * @brief
     * @param reader
     * @param _reserved
     */
    ExternalReferenceLocation(genie::util::BitReader& reader, uint8_t _reserved);

    /**
     * @brief
     * @return
     */
    const std::string& getURI() const;

    /**
     * @brief
     * @return
     */
    ChecksumAlgorithm getChecksumAlgorithm() const;

    /**
     * @brief
     * @return
     */
    RefType getReferenceType() const;

    /**
     * @brief
     * @param reader
     * @param _reserved
     * @param seq_count
     * @param _version
     * @return
     */
    static std::unique_ptr<ExternalReferenceLocation> externalReferenceLocationFactory(
        genie::util::BitReader& reader, uint8_t _reserved, size_t seq_count,
        genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) override;

    /**
     * @brief
     * @param checksum
     */
    virtual void addChecksum(std::string checksum) = 0;
};

/**
 * @brief
 */
class ExternalReferenceLocationMPEGG : public ExternalReferenceLocation {
 private:
    genie::core::MPEGMinorVersion version;   //!< @brief
    uint8_t external_dataset_group_id;       //!< @brief
    uint16_t external_dataset_id;            //!< @brief
    std::string ref_checksum;                //!< @brief
    std::vector<std::string> seq_checksums;  //!< @brief

 public:
    /**
     * @brief
     * @param _reserved
     * @param _uri
     * @param algo
     * @param _group_id
     * @param _dataset_id
     * @param _ref_checksum
     * @param _version
     */
    ExternalReferenceLocationMPEGG(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, uint8_t _group_id,
                                   uint16_t _dataset_id, std::string _ref_checksum = "",
                                   genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000);

    /**
     * @brief
     * @param reader
     * @param seq_count
     * @param _version
     */
    explicit ExternalReferenceLocationMPEGG(
        genie::util::BitReader& reader, size_t seq_count,
        genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000);

    /**
     * @brief
     * @param reader
     * @param _reserved
     * @param _uri
     * @param algo
     * @param seq_count
     * @param _version
     */
    explicit ExternalReferenceLocationMPEGG(
        genie::util::BitReader& reader, uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, size_t seq_count,
        genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000);

    /**
     * @brief
     * @return
     */
    uint8_t getExternalDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    uint16_t getExternalDatasetID() const;

    /**
     * @brief
     * @return
     */
    const std::string& getRefChecksum() const;

    /**
     * @brief
     * @return
     */
    const std::vector<std::string>& getSeqChecksums() const;

    /**
     * @brief
     * @param checksum
     */
    void addSeqChecksum(std::string checksum);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) override;

    /**
     * @brief
     * @param checksum
     */
    void addChecksum(std::string checksum) override;
};

/**
 * @brief
 */
class ExternalReferenceLocationRaw : public ExternalReferenceLocation {
 private:
    std::vector<std::string> seq_checksums;  //!< @brief

 public:
    /**
     * @brief
     * @param _reserved
     * @param _uri
     * @param algo
     */
    ExternalReferenceLocationRaw(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo);

    /**
     * @brief
     * @param reader
     * @param seq_count
     */
    ExternalReferenceLocationRaw(genie::util::BitReader& reader, size_t seq_count);

    /**
     * @brief
     * @param reader
     * @param _reserved
     * @param _uri
     * @param algo
     * @param seq_count
     */
    ExternalReferenceLocationRaw(genie::util::BitReader& reader, uint8_t _reserved, std::string _uri,
                                 ChecksumAlgorithm algo, size_t seq_count);

    /**
     * @brief
     * @return
     */
    const std::vector<std::string>& getSeqChecksums() const;

    /**
     * @brief
     * @param checksum
     */
    void addSeqChecksum(std::string checksum);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) override;

    /**
     * @brief
     * @param checksum
     */
    void addChecksum(std::string checksum) override;
};

/**
 * @brief
 */
class ExternalReferenceLocationFasta : public ExternalReferenceLocation {
 private:
    std::vector<std::string> seq_checksums;  //!< @brief

 public:
    /**
     * @brief
     * @param _reserved
     * @param _uri
     * @param algo
     */
    ExternalReferenceLocationFasta(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo);

    /**
     * @brief
     * @param reader
     * @param seq_count
     */
    ExternalReferenceLocationFasta(genie::util::BitReader& reader, size_t seq_count);

    /**
     * @brief
     * @param reader
     * @param _reserved
     * @param _uri
     * @param algo
     * @param seq_count
     */
    ExternalReferenceLocationFasta(genie::util::BitReader& reader, uint8_t _reserved, std::string _uri,
                                   ChecksumAlgorithm algo, size_t seq_count);

    /**
     * @brief
     * @return
     */
    const std::vector<std::string>& getSeqChecksums() const;

    /**
     * @brief
     * @param checksum
     */
    void addSeqChecksum(std::string checksum);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) override;

    /**
     * @brief
     * @param checksum
     */
    void addChecksum(std::string checksum) override;
};

/**
 * @brief
 */
class InternalReferenceLocation : public ReferenceLocation {
 private:
    uint8_t internal_dataset_group_id;  //!< @brief
    uint16_t internal_dataset_id;       //!< @brief

 public:
    /**
     * @brief
     * @param _reserved
     * @param _internal_dataset_group_id
     * @param _internal_dataset_id
     */
    InternalReferenceLocation(uint8_t _reserved, uint8_t _internal_dataset_group_id, uint16_t _internal_dataset_id);

    /**
     * @brief
     * @param reader
     */
    explicit InternalReferenceLocation(genie::util::BitReader& reader);

    /**
     * @brief
     * @param reader
     * @param _reserved
     */
    InternalReferenceLocation(genie::util::BitReader& reader, uint8_t _reserved);

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    uint16_t getDatasetID() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_REFERENCE_LOCATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
