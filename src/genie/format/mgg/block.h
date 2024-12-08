/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `Block` class for handling MPEG-G data blocks within the
 * mgg format.
 * @details This file provides the class implementation for representing and
 * manipulating data blocks, including metadata through headers and actual
 * payload data.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_BLOCK_H_
#define SRC_GENIE_FORMAT_MGG_BLOCK_H_

// -----------------------------------------------------------------------------

#include "genie/core/payload.h"
#include "genie/format/mgb/block.h"
#include "genie/format/mgg/block_header.h"
#include "genie/format/mgg/box.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing a block in the MPEG-G mgg format.
 * @details A block contains metadata in its header and the actual data in its
 * payload. This class supports creating, reading, writing, and manipulating
 * blocks.
 */
class Block final : public Box {
  BlockHeader header_;     //!< @brief Metadata header of the block.
  core::Payload payload_;  //!< @brief Actual data payload stored in the block.

 public:
  /**
   * @brief Outputs a debug representation of the block.
   * @param output Output stream to write the debug information.
   * @param depth Current depth level for nested structures.
   * @param max_depth Maximum allowed depth for printing nested structures.
   */
  void PrintDebug(std::ostream& output, uint8_t depth,
                   uint8_t max_depth) const override;

  /**
   * @brief Equality comparison operator.
   * @param other Reference to another `Block` object.
   * @return True if all fields are identical, otherwise false.
   */
  bool operator==(const Block& other) const;

  /**
   * @brief Constructs a block by reading from a `BitReader` stream.
   * @param reader Reference to the `BitReader` object to extract data from.
   */
  explicit Block(util::BitReader& reader);

  /**
   * @brief Constructs a block from a given descriptor and payload data.
   * @param desc_id Descriptor identifier for the block.
   * @param payload Raw payload data to be stored in the block.
   */
  Block(core::GenDesc desc_id, util::DataBlock payload);

  /**
   * @brief Constructs a block from an `mgb::Block` object.
   * @param b The `format::mgb::Block` object to be converted.
   */
  explicit Block(mgb::Block b);

  /**
   * @brief Retrieves the Size of the payload.
   * @return The Size of the payload in bytes.
   */
  [[nodiscard]] uint32_t GetPayloadSize() const;

  /**
   * @brief Retrieves the payload data.
   * @return A constant reference to the `DataBlock` containing the payload.
   */
  [[nodiscard]] const util::DataBlock& GetPayload() const;

  /**
   * @brief Moves the payload out of the block.
   * @details This method transfers ownership of the payload, leaving the block
   * empty.
   * @return The `core::Payload` object representing the data.
   */
  core::Payload MovePayload();

  /**
   * @brief Converts the block to an `mgb::Block` object.
   * @return An `mgb::Block` object with the same contents.
   */
  mgb::Block decapsulate();

  /**
   * @brief Calculates the length of the block in bits.
   * @return The total length of the block, including header and payload, in
   * bits.
   */
  [[nodiscard]] uint64_t GetLength() const;

  /**
   * @brief Writes the block to a `BitWriter` stream.
   * @param writer Reference to the `BitWriter` object for writing.
   */
  void Write(util::BitWriter& writer) const override;

  /**
   * @brief Retrieves the descriptor ID of the block.
   * @return The descriptor ID as a `GenDesc` enum value.
   */
  [[nodiscard]] core::GenDesc GetDescId() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_BLOCK_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
