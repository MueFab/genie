/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `BlockHeaderOffOptions` struct used within the MPEG-G
 * dataset header.
 *
 * This struct represents the optional settings for block headers in a dataset.
 * It controls properties such as whether blocks are ordered within the dataset.
 * This is primarily used in the MPEG-G format for encoding metadata about
 * dataset organization.
 *
 * @copyright
 * This file is part of Genie.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_OFF_OPTIONS_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_OFF_OPTIONS_H_

// -----------------------------------------------------------------------------

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

/**
 * @struct BlockHeaderOffOptions
 * @brief Represents optional flags for block headers in the MPEG-G dataset
 * header.
 *
 * This struct encapsulates optional settings for block headers, specifically
 * the `ordered_blocks_flag` which indicates whether the blocks are ordered or
 * not. It provides functionalities for reading from and writing to bitstreams,
 * enabling the serialization and deserialization of this metadata within MPEG-G
 * files.
 */
class BlockHeaderOffOptions {
  bool ordered_blocks_flag_;  //!< @brief Flag indicating whether the blocks are
                              //!< ordered within the dataset.

 public:
  /**
   * @brief Checks for equality between two `BlockHeaderOffOptions` objects.
   *
   * Compares the `ordered_blocks_flag` between this instance and another
   * `BlockHeaderOffOptions` object to determine if they are equal.
   *
   * @param other The other `BlockHeaderOffOptions` instance to compare with.
   * @return `true` if both instances have the same `ordered_blocks_flag`,
   * `false` otherwise.
   */
  bool operator==(const BlockHeaderOffOptions& other) const;

  /**
   * @brief Constructs a `BlockHeaderOffOptions` object with the specified
   * `ordered_blocks_flag`.
   *
   * Initializes the object with a specified value for the
   * `ordered_blocks_flag`.
   *
   * @param ordered_blocks_flag Boolean indicating whether blocks are ordered.
   */
  explicit BlockHeaderOffOptions(bool ordered_blocks_flag);

  /**
   * @brief Constructs a `BlockHeaderOffOptions` object by reading from a
   * bitstream.
   *
   * Reads the `ordered_blocks_flag` from the given `BitReader` stream,
   * initializing the internal state.
   *
   * @param reader Bitstream reader to extract the `ordered_blocks_flag` from.
   */
  explicit BlockHeaderOffOptions(util::BitReader& reader);

  /**
   * @brief Retrieves the `ordered_blocks_flag` value.
   *
   * Returns the internal value of the `ordered_blocks_flag`, indicating whether
   * the blocks are ordered.
   *
   * @return `true` if blocks are ordered, `false` otherwise.
   */
  [[nodiscard]] bool GetOrderedBlocksFlag() const;

  /**
   * @brief Writes the `BlockHeaderOffOptions` to a bitstream.
   *
   * Serializes the `ordered_blocks_flag` into the specified bitstream writer.
   *
   * @param writer Reference to the `BitWriter` used for output.
   */
  void Write(util::BitWriter& writer) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_OFF_OPTIONS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
