/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `MPEG` class for managing external MPEG reference
 * locations in MPEG-G files.
 *
 * The `MPEG` class is a specialized type of `External` location that references
 * sequences stored in another MPEG-G dataset. It includes functionality for
 * managing checksums associated with the sequences and metadata necessary to
 * identify the referenced dataset. This class is used in the context of MPEG-G
 * reference metadata to efficiently manage and reference external MPEG-G
 * datasets.
 *
 * @copyright
 * This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_MPEG_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_MPEG_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

#include "genie/format/mgg/reference/location/external.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::reference::location::external {

/**
 * @brief Represents an external MPEG-G reference location in an MPEG-G file.
 *
 * The `MPEG` class extends the `External` location type to support references
 * to external MPEG-G datasets. It manages dataset identifiers and
 * sequence-level checksums for external references. This class is used when
 * sequences in a dataset need to refer to another dataset stored within the
 * same or a different MPEG-G file.
 */
class Mpeg final : public External {
  core::MpegMinorVersion
      version_;  //!< @brief MPEG-G version of the external reference.
  uint8_t external_dataset_group_id_;  //!< @brief ID of the external dataset
                                      //!< group being referenced.
  uint16_t external_dataset_id_;  //!< @brief ID of the external dataset being
                                 //!< referenced.
  std::string ref_checksum_;  //!< @brief Checksum for the referenced dataset.
  std::vector<std::string>
      seq_checksums_;  //!< @brief Checksums for individual sequences in the
                      //!< external dataset.

 public:
  /**
   * @brief Decapsulates the `MPEG` location into a core metadata object.
   *
   * This method extracts the internal state of the `MPEG` object and converts
   * it into a more generic core metadata representation.
   *
   * @return A unique pointer to the decapsulated `RefBase` object.
   */
  std::unique_ptr<core::meta::RefBase> decapsulate() override;

  /**
   * @brief Constructs an `MPEG` object with the specified parameters.
   *
   * @param reserved Reserved byte value.
   * @param uri The URI pointing to the external MPEG-G reference.
   * @param algo The checksum algorithm used for validating the sequence data.
   * @param group_id The ID of the external dataset group.
   * @param dataset_id The ID of the external dataset.
   * @param ref_checksum The checksum of the referenced dataset.
   * @param version The MPEG-G version of the external reference.
   */
  Mpeg(uint8_t reserved, std::string uri, ChecksumAlgorithm algo,
       uint8_t group_id, uint16_t dataset_id, std::string ref_checksum,
       core::MpegMinorVersion version);

  /**
   * @brief Constructs an `MPEG` object by reading from a bitstream.
   *
   * This constructor initializes the `MPEG` object by reading its state from a
   * `BitReader`. The sequence count is used to determine the number of
   * checksums to read.
   *
   * @param reader The `BitReader` to read the data from.
   * @param seq_count The number of sequences expected in the MPEG reference.
   * @param version The MPEG-G version of the external reference.
   */
  explicit Mpeg(util::BitReader& reader, size_t seq_count,
                core::MpegMinorVersion version);

  /**
   * @brief Constructs an `MPEG` object with additional parameters.
   *
   * @param reader The `BitReader` to read the data from.
   * @param reserved Reserved byte value.
   * @param uri The URI pointing to the external MPEG-G reference.
   * @param algo The checksum algorithm used for validating the sequence data.
   * @param seq_count The number of sequences expected in the MPEG reference.
   * @param version The MPEG-G version of the external reference.
   */
  Mpeg(util::BitReader& reader, uint8_t reserved, std::string uri,
       ChecksumAlgorithm algo, size_t seq_count,
       core::MpegMinorVersion version);

  /**
   * @brief Gets the external dataset group ID.
   * @return The ID of the external dataset group being referenced.
   */
  [[nodiscard]] uint8_t GetExternalDatasetGroupId() const;

  /**
   * @brief Gets the external dataset ID.
   * @return The ID of the external dataset being referenced.
   */
  [[nodiscard]] uint16_t GetExternalDatasetId() const;

  /**
   * @brief Gets the checksum for the referenced dataset.
   * @return The checksum of the referenced dataset.
   */
  [[nodiscard]] const std::string& GetRefChecksum() const;

  /**
   * @brief Gets the checksums for the sequences in the MPEG reference.
   * @return A reference to the vector of sequence checksums.
   */
  [[nodiscard]] const std::vector<std::string>& GetSeqChecksums() const;

  /**
   * @brief Adds a new checksum for a sequence in the MPEG reference.
   * @param checksum The checksum value to add.
   */
  void AddSeqChecksum(std::string checksum);

  /**
   * @brief Serializes the `MPEG` object to a bitstream.
   * @param writer The `BitWriter` to serialize the data to.
   */
  void Write(util::BitWriter& writer) override;

  /**
   * @brief Adds a checksum to the `MPEG` location.
   *
   * This method is part of the `External` interface and is used to add a
   * checksum to the `MPEG` object.
   *
   * @param checksum The checksum value to add.
   */
  void AddChecksum(std::string checksum) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location::external

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_MPEG_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
