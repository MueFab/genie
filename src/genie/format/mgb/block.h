/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_BLOCK_H_
#define SRC_GENIE_FORMAT_MGB_BLOCK_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <variant>

#include "genie/core/access_unit.h"
#include "genie/core/payload.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class Block final {
  uint8_t descriptor_id_;        //!< @brief
  uint32_t block_payload_size_;  //!< @brief

  uint8_t count_;  //!< @brief

  std::variant<core::Payload, core::AccessUnit::Descriptor>
      payload_;  //!< @brief

 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsLoaded() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsParsed() const;

  /**
   * @brief
   */
  void load();

  /**
   * @brief
   */
  void unload();

  /**
   * @brief
   */
  void parse();

  /**
   * @brief
   */
  void pack();

  /**
   * @brief
   */
  Block();

  /**
   * @brief
   * @param descriptor_id
   * @param payload
   */
  Block(uint8_t descriptor_id, core::AccessUnit::Descriptor&& payload);

  /**
   * @brief
   * @param descriptor_id
   * @param payload
   */
  Block(core::GenDesc descriptor_id, core::Payload payload);

  /**
   * @brief
   * @param qv_count
   * @param reader
   */
  explicit Block(size_t qv_count, util::BitReader& reader);

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @return
   */
  core::AccessUnit::Descriptor&& MovePayload();

  /**
   * @brief
   * @return
   */
  core::Payload MovePayloadUnparsed();

  /**
   * @brief
   * @return
   */
  core::Payload& GetPayloadUnparsed();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetDescriptorId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] size_t GetWrittenSize() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_BLOCK_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
