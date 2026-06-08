/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `Offset` class used in MPEG-G data structures.
 *
 * The `Offset` class represents a key-value pair where the key is a sub key
 * string and the value is a numerical offset. It is used within MPEG-G
 * containers to denote positions within the data streams, helping to keep track
 * of the locations of specific elements for faster access and manipulation.
 *
 * @copyright
 * This file is part of Genie.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_OFFSET_H_
#define SRC_GENIE_FORMAT_MGG_OFFSET_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @class Offset
 * @brief Represents an MPEG-G offset entry.
 *
 * This class encapsulates a mapping between a sub key string and a 64-bit
 * offset value. Offsets are used within MPEG-G containers to denote byte
 * positions or other references, enabling efficient data retrieval and
 * manipulation. This class allows for reading from and writing to bitstreams.
 */
class Offset {
  std::string sub_key_;  //!< @brief Sub key identifying the specific entry in
                         //!< the MPEG-G structure.
  uint64_t offset_;      //!< @brief Offset value corresponding to the sub key.

 public:
  /**
   * @brief Constructs an `Offset` object by reading from a bitstream.
   *
   * This constructor reads the sub key and the offset value from the given
   * bitstream, initializing the internal state of the `Offset` object.
   *
   * @param bit_reader Bitstream reader used to extract the sub key and offset.
   */
  explicit Offset(util::BitReader& bit_reader);

  /**
   * @brief Writes the `Offset` object to a bitstream.
   *
   * This method serializes the sub key and the offset value into the specified
   * bitstream, ensuring that the offset information is correctly encoded.
   *
   * @param writer Bitstream writer to serialize the offset data.
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief Retrieves the sub key associated with the offset.
   * @return Constant reference to the sub key string.
   */
  [[nodiscard]] const std::string& GetSubKey() const;

  /**
   * @brief Retrieves the numerical offset value.
   * @return 64-bit unsigned integer representing the offset.
   */
  [[nodiscard]] uint64_t GetOffset() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_OFFSET_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
