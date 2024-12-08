/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `Fasta` class for managing external FASTA reference
 * locations in MPEG-G files.
 *
 * The `Fasta` class is a specialized type of `External` location for
 * referencing sequences stored in a FASTA file format. It includes
 * functionality for managing and validating checksums associated with each
 * sequence. This class is used in the context of MPEG-G reference metadata,
 * allowing efficient referencing and management of external sequence data.
 *
 * @copyright
 * This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_FASTA_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_FASTA_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

#include "genie/format/mgg/reference/location/external.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::reference::location::external {

/**
 * @brief Represents an external FASTA reference location in an MPEG-G file.
 *
 * The `Fasta` class extends the `External` location type to support external
 * references to FASTA files. It provides functionality for managing
 * sequence-level checksums, decapsulation, and serialization. The class is
 * primarily used for referencing large genomic sequences stored externally in
 * FASTA format.
 */
class Fasta final : public External {
  std::vector<std::string> seq_checksums_;  //!< @brief Checksums for individual
                                            //!< sequences in the FASTA file.

 public:
  /**
   * @brief Decapsulates the `Fasta` location into a core metadata object.
   *
   * This method extracts the internal state of the `Fasta` object and converts
   * it into a more generic core metadata representation.
   *
   * @return A unique pointer to the decapsulated `RefBase` object.
   */
  std::unique_ptr<core::meta::RefBase> decapsulate() override;

  /**
   * @brief Constructs a `Fasta` object with specified parameters.
   *
   * @param reserved Reserved byte value.
   * @param uri The URI pointing to the external FASTA file.
   * @param algo The checksum algorithm used for validating sequence data.
   */
  Fasta(uint8_t reserved, std::string uri, ChecksumAlgorithm algo);

  /**
   * @brief Constructs a `Fasta` object by reading from a bitstream.
   *
   * This constructor initializes the `Fasta` object by reading its state from a
   * `BitReader`. The sequence count is used to determine the number of
   * checksums to read.
   *
   * @param reader The `BitReader` to read the data from.
   * @param seq_count The number of sequences expected in the FASTA file.
   */
  Fasta(util::BitReader& reader, size_t seq_count);

  /**
   * @brief Constructs a `Fasta` object with additional parameters.
   *
   * @param reader The `BitReader` to read the data from.
   * @param reserved Reserved byte value.
   * @param uri The URI pointing to the external FASTA file.
   * @param algo The checksum algorithm used for validating sequence data.
   * @param seq_count The number of sequences expected in the FASTA file.
   */
  Fasta(util::BitReader& reader, uint8_t reserved, std::string uri,
        ChecksumAlgorithm algo, size_t seq_count);

  /**
   * @brief Gets the checksums for the sequences in the FASTA file.
   * @return A reference to the vector of sequence checksums.
   */
  [[nodiscard]] const std::vector<std::string>& GetSeqChecksums() const;

  /**
   * @brief Adds a new checksum for a sequence in the FASTA file.
   * @param checksum The checksum value to add.
   */
  void AddSeqChecksum(std::string checksum);

  /**
   * @brief Serializes the `Fasta` object to a bitstream.
   * @param writer The `BitWriter` to serialize the data to.
   */
  void Write(util::BitWriter& writer) override;

  /**
   * @brief Adds a checksum to the `Fasta` location.
   *
   * This method is part of the `External` interface and is used to add a
   * checksum to the `Fasta` object.
   *
   * @param checksum The checksum value to add.
   */
  void AddChecksum(std::string checksum) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location::external

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_FASTA_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
