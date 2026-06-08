/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `External` class for representing external reference
 * locations in MPEG-G files.
 *
 * The `External` class is a specialized type of `Location` used to represent
 * references to external sequence files such as raw, FASTA, or other MPEG-G
 * references. It manages URI references, checksum algorithms, and the type of
 * external reference. The class also provides functionality for serialization
 * and deserialization of the reference data, making it a key component in
 * MPEG-G metadata handling.
 *
 * @copyright
 * This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>

#include "genie/format/mgg/reference/location.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::reference::location {

/**
 * @brief Represents an external reference location in an MPEG-G file.
 *
 * The `External` class is an abstract base class that extends the `Location`
 * class and represents a reference to an external sequence. It provides
 * mechanisms to handle different external reference types (e.g., MPEG-G, raw,
 * or FASTA), manage their URIs, and store checksums to ensure data integrity.
 * This class serves as a foundation for derived classes that specialize in
 * managing the specifics of each type of external reference.
 */
class External : public Location {
 public:
  /**
   * @brief Enum defining the supported checksum algorithms.
   */
  enum class ChecksumAlgorithm : uint8_t {
    MD5 = 0,    //!< @brief MD5 checksum algorithm.
    SHA256 = 1  //!< @brief SHA-256 checksum algorithm.
  };

  /**
   * @brief Array specifying the checksum sizes for each algorithm.
   */
  static constexpr size_t checksum_sizes_[2] = {
      128 / 8, 256 / 8};  //!< @brief Sizes of checksum outputs
                          //!< for different algorithms.

  /**
   * @brief Enum defining the type of external reference.
   */
  enum class RefType : uint8_t {
    MPEGG_REF = 0,  //!< @brief MPEG-G formatted reference.
    RAW_REF = 1,    //!< @brief Raw formatted reference.
    FASTA_REF = 2   //!< @brief FASTA formatted reference.
  };

 private:
  std::string uri_;  //!< @brief URI pointing to the external reference file.
  ChecksumAlgorithm checksum_algo_;  //!< @brief The checksum algorithm used for
                                     //!< verifying the reference.
  RefType reference_type_;  //!< @brief Type of the external reference (e.g.,
                            //!< MPEGG_REF, RAW_REF, FASTA_REF).

 public:
  /**
   * @brief Constructs an `External` reference with specified parameters.
   *
   * @param reserved Reserved byte for additional options.
   * @param uri URI pointing to the external reference.
   * @param algo Checksum algorithm used for validation.
   * @param type Type of the external reference.
   */
  External(uint8_t reserved, std::string uri, ChecksumAlgorithm algo,
           RefType type);

  /**
   * @brief Constructs an `External` reference from a bitstream.
   *
   * @param reader The `BitReader` to read the data from.
   */
  explicit External(util::BitReader& reader);

  /**
   * @brief Constructs an `External` reference from a bitstream with a reserved
   * byte.
   *
   * @param reader The `BitReader` to read the data from.
   * @param reserved Reserved byte value.
   */
  External(util::BitReader& reader, uint8_t reserved);

  /**
   * @brief Gets the URI of the external reference.
   * @return A const reference to the URI string.
   */
  [[nodiscard]] const std::string& GetUri() const;

  /**
   * @brief Gets a mutable reference to the URI string.
   * @return A reference to the URI string.
   */
  std::string& GetUri();

  /**
   * @brief Gets the checksum algorithm used for validating the reference.
   * @return The checksum algorithm type.
   */
  [[nodiscard]] ChecksumAlgorithm GetChecksumAlgorithm() const;

  /**
   * @brief Gets the type of the external reference.
   * @return The type of the external reference.
   */
  [[nodiscard]] RefType GetReferenceType() const;

  /**
   * @brief Factory method to create a specific external reference based on the
   * bitstream data.
   *
   * This static method reads the bitstream and determines the appropriate
   * external reference type to instantiate.
   *
   * @param reader The `BitReader` to read the data from.
   * @param reserved Reserved byte value.
   * @param seq_count Number of sequences expected in the reference.
   * @param version The MPEG-G minor version.
   * @return A unique pointer to the constructed `Location` object.
   */
  static std::unique_ptr<Location> factory(util::BitReader& reader,
                                           uint8_t reserved, size_t seq_count,
                                           core::MpegMinorVersion version);

  /**
   * @brief Serializes the `External` reference to a bitstream.
   *
   * @param writer The `BitWriter` to serialize the data to.
   */
  void Write(util::BitWriter& writer) override;

  /**
   * @brief Adds a checksum to the external reference.
   *
   * This is a pure virtual method that must be implemented by derived classes.
   *
   * @param checksum The checksum value to add.
   */
  virtual void AddChecksum(std::string checksum) = 0;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
