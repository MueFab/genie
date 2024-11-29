/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `BlockHeaderOnOptions` class used within the MPEG-G
 * dataset header.
 *
 * The `BlockHeaderOnOptions` class represents the configuration flags for block
 * headers when the header options are "on." It includes flags such as the
 * `mit_flag` and `cc_mode_flag` which define specific properties of the MPEG-G
 * block headers.
 *
 * @copyright
 * This file is part of Genie.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_ON_OPTIONS_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_ON_OPTIONS_H_

// -----------------------------------------------------------------------------

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

/**
 * @class BlockHeaderOnOptions
 * @brief Represents the options for MPEG-G block headers when "on."
 *
 * The `BlockHeaderOnOptions` class encapsulates flags that define specific
 * attributes of block headers when they are active in the dataset. These
 * options include the Metadata Information Table (MIT) flag and the
 * cross-correlation mode (CC) flag, which influence the interpretation and
 * handling of block data.
 */
class BlockHeaderOnOptions {
  bool mit_flag_;      //!< @brief Flag indicating the presence of a Metadata
                       //!< Information Table (MIT).
  bool cc_mode_flag_;  //!< @brief Flag indicating the use of cross-correlation
                       //!< mode.

 public:
  /**
   * @brief Constructs a `BlockHeaderOnOptions` object with specified flags.
   *
   * Initializes the `BlockHeaderOnOptions` with values for the `mit_flag` and
   * `cc_mode_flag`.
   *
   * @param mit_flag Boolean flag indicating if the Metadata Information Table
   * (MIT) is present.
   * @param cc_mode_flag Boolean flag indicating if cross-correlation mode is
   * active.
   */
  BlockHeaderOnOptions(bool mit_flag, bool cc_mode_flag);

  /**
   * @brief Constructs a `BlockHeaderOnOptions` object by reading from a
   * bitstream.
   *
   * Reads the `mit_flag` and `cc_mode_flag` values from the provided bitstream,
   * initializing the internal state of the `BlockHeaderOnOptions` instance.
   *
   * @param reader Bitstream reader used to extract the flags.
   */
  explicit BlockHeaderOnOptions(util::BitReader& reader);

  /**
   * @brief Writes the `BlockHeaderOnOptions` to a bitstream.
   *
   * Serializes the `mit_flag` and `cc_mode_flag` into the specified bitstream
   * writer.
   *
   * @param writer Reference to the `BitWriter` used for output.
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief Checks for equality between two `BlockHeaderOnOptions` objects.
   *
   * Compares the `mit_flag` and `cc_mode_flag` values between this instance and
   * another `BlockHeaderOnOptions` object to determine if they are equal.
   *
   * @param other The other `BlockHeaderOnOptions` instance to compare with.
   * @return `true` if both instances have the same flag values, `false`
   * otherwise.
   */
  bool operator==(const BlockHeaderOnOptions& other) const;

  /**
   * @brief Retrieves the value of the `mit_flag`.
   *
   * The `mit_flag` indicates whether a Metadata Information Table (MIT) is
   * present.
   *
   * @return `true` if MIT is present, `false` otherwise.
   */
  [[nodiscard]] bool GetMitFlag() const;

  /**
   * @brief Retrieves the value of the `cc_mode_flag`.
   *
   * The `cc_mode_flag` indicates whether cross-correlation mode is enabled.
   *
   * @return `true` if cross-correlation mode is active, `false` otherwise.
   */
  [[nodiscard]] bool GetCcFlag() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_ON_OPTIONS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
