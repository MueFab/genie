/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Base class for handling boxes in the MPEG-G format.
 * @details The `Box` class serves as an abstract base class for different box
 * types in the MPEG-G format. Derived classes need to implement methods for
 * serialization, deserialization, and debugging.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_BOX_H_
#define SRC_GENIE_FORMAT_MGG_BOX_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Abstract base class representing a generic box in MPEG-G format.
 * @details Provides basic functionality for managing the structure of boxes and
 * printing debug information. All derived classes must implement the `write`
 * method to define how the box is serialized.
 */
class Box {
 protected:
  /**
   * @brief Helper function to print an offset message for debugging.
   * @param output Output stream to print the message to.
   * @param depth Current depth level in the hierarchy.
   * @param max_depth Maximum depth level for printing.
   * @param msg Message to print at the given offset.
   */
  virtual void print_offset(std::ostream& output, uint8_t depth,
                            uint8_t max_depth, const std::string& msg) const;

 public:
  /**
   * @brief Virtual destructor for `Box` class.
   */
  virtual ~Box() = default;

  /**
   * @brief Pure virtual function for writing the box data to a bit stream.
   * @param bitWriter Bit writer to serialize the box contents to.
   */
  virtual void Write(genie::util::BitWriter& bitWriter) const = 0;

  /**
   * @brief Prints a debug representation of the box to an output stream.
   * @param output Output stream to print the debug information to.
   * @param depth Current depth level in the hierarchy.
   * @param max_depth Maximum depth level for printing.
   */
  virtual void PrintDebug(std::ostream& output, uint8_t depth,
                           uint8_t max_depth) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_BOX_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
