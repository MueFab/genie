/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `Sequence` class for representing reference sequences in
 * MPEG-G files.
 *
 * The `Sequence` class encapsulates information about a single sequence in a
 * reference, such as its name, length, identifier, and MPEG-G version. It
 * provides methods for constructing, serializing, and deserializing sequences
 * within the MPEG-G format, enabling interoperability with other MPEG-G
 * components.
 *
 * This class is designed to work with the MPEG-G reference metadata system,
 * allowing efficient handling of reference sequences during both encoding and
 * decoding processes.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_SEQUENCE_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_SEQUENCE_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/core/constants.h"
#include "genie/core/meta/sequence.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::reference {

/**
 * @brief Represents a reference sequence in an MPEG-G file.
 *
 * The `Sequence` class stores metadata about a single sequence within a
 * reference, such as its name, length, and ID. It also handles reading and
 * writing sequence data to and from MPEG-G bitstreams, making it integral to
 * the MPEG-G reference management system.
 */
class Sequence {
  std::string name_;          //!< @brief Name of the sequence.
  uint32_t sequence_length_;  //!< @brief Length of the sequence in base pairs.
  uint16_t sequence_id_;      //!< @brief Unique identifier for the sequence.
  core::MpegMinorVersion
      version_;  //!< @brief MPEG-G version for compatibility.

 public:
  /**
   * @brief Equality operator to compare two `Sequence` objects.
   *
   * This operator checks if two sequences have the same name, length, ID, and
   * version.
   *
   * @param other The sequence to compare with.
   * @return True if the sequences are equal, false otherwise.
   */
  bool operator==(const Sequence& other) const;

  /**
   * @brief Constructs a `Sequence` object with specified parameters.
   *
   * @param name Name of the sequence.
   * @param length Length of the sequence in base pairs.
   * @param id Unique identifier for the sequence.
   * @param version MPEG-G version for compatibility.
   */
  Sequence(std::string name, uint32_t length, uint16_t id,
           core::MpegMinorVersion version);

  /**
   * @brief Constructs a `Sequence` object from a `genie::core::meta::Sequence`
   * object.
   *
   * This constructor is used to convert a core `meta::Sequence` object into an
   * MPEG-G `Sequence` object.
   *
   * @param s The `meta::Sequence` object containing sequence metadata.
   * @param version The MPEG-G version for compatibility.
   */
  Sequence(core::meta::Sequence s,
           core::MpegMinorVersion version);

  /**
   * @brief Gets the name of the sequence.
   * @return The name of the sequence.
   */
  [[nodiscard]] const std::string& GetName() const;

  /**
   * @brief Gets the length of the sequence in base pairs.
   * @return The length of the sequence.
   */
  [[nodiscard]] uint32_t GetLength() const;

  /**
   * @brief Gets the unique identifier of the sequence.
   * @return The ID of the sequence.
   */
  [[nodiscard]] uint16_t GetId() const;

  /**
   * @brief Constructs a `Sequence` object by reading from a bitstream.
   *
   * This constructor initializes a `Sequence` object using data read from the
   * provided bitstream.
   *
   * @param reader The `BitReader` to read the sequence data from.
   * @param version The MPEG-G version for compatibility.
   */
  Sequence(util::BitReader& reader,
           core::MpegMinorVersion version);

  /**
   * @brief Writes the `Sequence` object to a bitstream.
   *
   * This method serializes the `Sequence`'s attributes (name, length, ID, and
   * version) into a bitstream for storage or transmission.
   *
   * @param writer The `BitWriter` to write the serialized data to.
   */
  void Write(util::BitWriter& writer) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_SEQUENCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
