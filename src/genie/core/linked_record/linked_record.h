/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CORE_RECORD_LINKED_RECORD_LINKED_RECORD_H_
#define GENIE_CORE_RECORD_LINKED_RECORD_LINKED_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Class representing a record link to an external reference
 *
 * Stores information about linked records, including their name and reference box ID.
 */
class LinkRecord {
 private:
  std::string link_name_;     //!< @brief
  uint8_t reference_box_id_;  //!< @brief
 public:
  /**
   * @brief Constructs a link record by reading from a bit stream
   * @param bitreader The bit reader to read from
   */
  explicit LinkRecord(util::BitReader& bitreader);

  /**
   * @brief Gets the length of the link name
   * @return The length of the link name
   */
  [[nodiscard]] uint8_t GetLinkNameLen() const;

  /**
   * @brief Gets the link name
   * @return Constant reference to the link name
   */
  [[nodiscard]] const std::string& GetLinkName() const;

  /**
   * @brief Gets the reference box ID
   * @return The reference box ID
   */
  [[nodiscard]] uint8_t GetReferenceBoxId() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CORE_RECORD_LINKED_RECORD_LINKED_RECORD_H_
