/**
 * Copyright 2018-2024 The Genie Authors.
 * @file reference.h
 * @copyright This file is part of Genie.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `Reference` class for representing reference sequences
 * within the MGG (MPEG-G) container format in Genie.
 *
 * The `Reference` class is used to encapsulate details about genomic reference
 * sequences stored within an MGG (MPEG-G) container, part of the MPEG-G
 * standard for genomic data representation. This class provides methods to
 * manage sequence information, versioning, metadata, and associated storage
 * locations for a given reference sequence. Additionally, it supports
 * serialization and deserialization of the reference information from a
 * bitstream.
 *
 * MPEG-G (ISO/IEC 23092) is a comprehensive standard designed to efficiently
 * store, transmit, and process large-scale genomic data. The `Reference` class
 * helps facilitate this by providing a structured way to manage and access
 * reference sequence details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/meta/reference.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/reference/location.h"
#include "genie/format/mgg/reference/sequence.h"
#include "genie/format/mgg/reference/version.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @class Reference
 * @brief Represents genomic reference information within an MGG (MPEG-G)
 * container.
 *
 * The `Reference` class is responsible for encapsulating and managing all
 * metadata and sequence information associated with a genomic reference stored
 * in an MGG container file. It provides methods to manipulate reference
 * details, including sequence data, versioning, and storage location.
 *
 * The MPEG-G standard is designed to enable efficient genomic data compression
 * and transport, making it suitable for handling large-scale datasets.
 * `Reference` objects are a key component in managing reference sequence
 * details for genomic datasets.
 */
class Reference final : public GenInfo {
 public:
  /**
   * @enum ReferenceType
   * @brief Enumerates the supported reference types in the MPEG-G format.
   */
  enum class ReferenceType : uint8_t {
    MPEGG_REF = 0,
    RAW_REF = 1,
    FASTA_REF = 2
  };  //!< @brief Type of reference

 private:
  uint8_t dataset_group_id_;    //!< @brief ID of the associated dataset group.
  uint8_t reference_id_;        //!< @brief ID of the reference sequence.
  std::string reference_name_;  //!< @brief Name of the reference sequence.
  reference::Version
      ref_version_;  //!< @brief Version of the reference sequence.

  std::vector<reference::Sequence>
      sequences_;  //!< @brief Container holding reference sequences.

  std::unique_ptr<reference::Location>
      reference_location_;  //!< @brief Storage location details for the
                            //!< reference.

  core::MpegMinorVersion version_;  //!< @brief MPEG-G minor version used.

 public:
  /**
   * @brief Updates the dataset group ID.
   * @param group_id New dataset group ID.
   */
  void PatchId(uint8_t group_id);

  /**
   * @brief Patches the reference ID.
   * @param old Previous reference ID.
   * @param _new New reference ID.
   */
  void PatchRefId(uint8_t old, uint8_t _new);

  /**
   * @brief Decapsulates the reference into a `meta::Reference` object.
   * @param meta Metadata for the reference.
   * @return A `meta::Reference` object containing the decapsulated details.
   */
  core::meta::Reference decapsulate(std::string meta);

  /**
   * @brief Constructor to create a `Reference` object from a `meta::Reference`
   * object.
   * @param dataset_group_id Dataset group ID.
   * @param reference_id Reference ID.
   * @param ref Metadata object for the reference.
   * @param version MPEG minor version.
   */
  Reference(uint8_t dataset_group_id, uint8_t reference_id,
            core::meta::Reference ref, core::MpegMinorVersion version);

  /**
   * @brief Checks for equality between two `GenInfo` objects.
   * @param info The other `GenInfo` object.
   * @return True if equal, false otherwise.
   */
  bool operator==(const GenInfo& info) const override;

  /**
   * @brief Constructor to create a `Reference` object from a bitstream.
   * @param reader Bitstream reader to extract the reference data.
   * @param version MPEG minor version.
   */
  explicit Reference(util::BitReader& reader,
                     core::MpegMinorVersion version);

  /**
   * @brief Constructor to create a `Reference` object with the provided
   * details.
   * @param group_id Dataset group ID.
   * @param ref_id Reference ID.
   * @param ref_name Name of the reference.
   * @param ref_version Version information.
   * @param location Location details for the reference.
   * @param version MPEG minor version.
   */
  Reference(uint8_t group_id, uint8_t ref_id, std::string ref_name,
            reference::Version ref_version,
            std::unique_ptr<reference::Location> location,
            core::MpegMinorVersion version);

  /**
   * @brief Retrieves the dataset group ID.
   * @return Dataset group ID.
   */
  [[nodiscard]] uint8_t GetDatasetGroupId() const;

  /**
   * @brief Retrieves the reference ID.
   * @return Reference ID.
   */
  [[nodiscard]] uint8_t GetReferenceId() const;

  /**
   * @brief Retrieves the name of the reference.
   * @return Reference name as a string.
   */
  [[nodiscard]] const std::string& GetReferenceName() const;

  /**
   * @brief Retrieves the version of the reference.
   * @return Reference version object.
   */
  [[nodiscard]] const reference::Version& GetRefVersion() const;

  /**
   * @brief Retrieves the reference sequences.
   * @return Vector of reference sequences.
   */
  [[nodiscard]] const std::vector<reference::Sequence>& GetSequences() const;

  /**
   * @brief Retrieves the key for the `GenInfo` object.
   * @return Key as a string.
   */
  [[nodiscard]] const std::string& GetKey() const override;

  /**
   * @brief Adds a new sequence to the reference.
   * @param seq Sequence object.
   * @param checksum Checksum for the sequence.
   */
  void AddSequence(reference::Sequence seq, std::string checksum);

  /**
   * @brief Retrieves the location details of the reference.
   * @return Location object.
   */
  [[nodiscard]] const reference::Location& GetLocation() const;

  /**
   * @brief Writes the reference data to a bitstream.
   * @param writer Bitstream writer to output the reference data.
   */
  void BoxWrite(util::BitWriter& writer) const override;

  /**
   * @brief Prints debug information for the reference.
   * @param output Output stream.
   * @param depth Current depth in the print hierarchy.
   * @param max_depth Maximum depth to print.
   */
  void PrintDebug(std::ostream& output, uint8_t depth,
                   uint8_t max_depth) const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
