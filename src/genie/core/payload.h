/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PAYLOAD_H_
#define SRC_GENIE_CORE_PAYLOAD_H_

// -----------------------------------------------------------------------------

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class Payload {
  util::DataBlock block_payload_;  //!< @brief

  bool payload_loaded_;        //!< @brief
  uint64_t payload_position_;  //!< @brief
  uint64_t payload_size_;      //!< @brief

  util::BitReader* internal_reader_;  //!< @brief

  /**
   * @brief
   * @param reader
   * @return
   */
  util::DataBlock InternalLoadPayload(util::BitReader& reader) const;

 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetPayloadSize() const;

  /**
   * @brief
   */
  void LoadPayload();

  /**
   * @brief
   */
  void UnloadPayload();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsPayloadLoaded() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const util::DataBlock& GetPayload() const;

  /**
   * @brief
   * @return
   */
  util::DataBlock&& MovePayload();

  /**
   * @brief
   * @param payload
   */
  explicit Payload(util::DataBlock payload);

  /**
   * @brief
   * @param reader
   * @param size
   */
  explicit Payload(util::BitReader& reader, uint64_t size);

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @param other
   * @return
   */
  bool operator==(const Payload& other) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PAYLOAD_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
