/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Base class definition for general information elements within MPEG-G
 * files.
 * @details This file provides the interface for the `GenInfo` class, which
 * serves as a base class for various types of general information elements
 * (metadata boxes) found in MPEG-G files. It extends the `Box` class and adds
 *          methods to handle serialization, key management, and comparisons of
 * general information elements.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_GEN_INFO_H_
#define SRC_GENIE_FORMAT_MGG_GEN_INFO_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/format/mgg/box.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Abstract base class for general information elements within MPEG-G
 * files.
 * @details The `GenInfo` class provides a base interface for representing
 * general information elements, which are used to encode various types of
 * metadata within MPEG-G files. This class inherits from `Box` and adds virtual
 * methods for writing data to a `BitWriter`, retrieving unique identifiers, and
 * performing equality comparisons.
 */
class GenInfo : public Box {
 protected:
  /**
   * @brief Write the content of the general information element to a
   * `BitWriter`.
   * @details This pure virtual function must be implemented by derived classes
   * to define how the content of a particular general information element is
   * serialized into a bitstream.
   * @param bit_writer The `BitWriter` to serialize the data into.
   */
  virtual void BoxWrite(util::BitWriter& bit_writer) const = 0;

 public:
  /**
   * @brief Retrieve the length of the header for a general information box.
   * @details General information boxes have a standard header length. This
   * method retrieves that length for use in calculating the overall Size of the
   * box.
   * @return The length of the header in bits.
   */
  static uint64_t GetHeaderLength();

  /**
   * @brief Retrieve a unique key identifier for this general information
   * element.
   * @details Each general information element should have a unique key or
   * identifier. This method must be implemented by derived classes to return
   * the appropriate key for the specific type of general information element.
   * @return A constant reference to the key string.
   */
  [[nodiscard]] virtual const std::string& GetKey() const = 0;

  /**
   * @brief Write the entire content of the general information element to a
   * `BitWriter`.
   * @details This method calls the pure virtual `box_write` method to serialize
   * the content-specific data of the general information element, in addition
   * to handling the standard box header.
   * @param bit_writer The `BitWriter` to serialize the entire box data into.
   */
  void Write(util::BitWriter& bit_writer) const final;

  /**
   * @brief Compare this general information element with another for equality.
   * @details This method provides a basic implementation for comparing two
   * general information elements. It can be overridden by derived classes to
   * perform more specific comparisons.
   * @param info The other `GenInfo` object to compare against.
   * @return True if the two elements are considered equal, false otherwise.
   */
  virtual bool operator==(const GenInfo& info) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_GEN_INFO_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
