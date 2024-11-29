/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `MITClassConfig` class used within the MPEG-G dataset
 * header.
 *
 * The `MITClassConfig` class represents the configuration for a specific class
 * of the Metadata Information Table (MIT). It holds an ID representing the
 * class type and a set of descriptor IDs used to describe the class. This class
 * provides methods for reading and writing its contents from/to a bitstream, as
 * well as utilities for adding and querying the descriptor IDs.
 *
 * @copyright
 * This file is part of Genie.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_MIT_CLASS_CONFIG_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_MIT_CLASS_CONFIG_H_

// -----------------------------------------------------------------------------

#include <vector>

#include "genie/core/constants.h"
#include "genie/core/record/class_type.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

/**
 * @class MitClassConfig
 * @brief Represents the configuration for a class in the Metadata Information
 * Table (MIT).
 *
 * The `MITClassConfig` class stores information related to a specific class in
 * the MIT, including its class type ID and a set of associated descriptor IDs.
 * Each class type may have one or more descriptors that define its properties
 * within the MPEG-G data structure.
 */
class MitClassConfig {
  core::record::ClassType
      id_;  //!< @brief Class type identifier for the MIT class.
  std::vector<core::GenDesc>
      descriptor_ids_;  //!< @brief List of descriptor IDs associated with the
                        //!< class.

 public:
  /**
   * @brief Checks for equality between two `MITClassConfig` objects.
   *
   * Compares the class type ID and descriptor IDs of this instance with another
   * `MITClassConfig` object to determine if they are equal.
   *
   * @param other The other `MITClassConfig` instance to compare with.
   * @return `true` if both instances have the same class ID and descriptor IDs,
   * `false` otherwise.
   */
  bool operator==(const MitClassConfig& other) const;

  /**
   * @brief Constructs an `MITClassConfig` object with the specified class ID.
   *
   * Initializes the object with a given class ID, representing the type of the
   * class in the MIT.
   *
   * @param id The class ID for the MIT class.
   */
  explicit MitClassConfig(core::record::ClassType id);

  /**
   * @brief Constructs an `MITClassConfig` object by reading from a bitstream.
   *
   * Reads the class ID and descriptor IDs from the given bitstream. If
   * `block_header_flag` is set, it adjusts the parsing logic accordingly.
   *
   * @param reader Bitstream reader used to extract the class and descriptor
   * information.
   * @param block_header_flag Boolean flag indicating special block header
   * handling.
   */
  MitClassConfig(util::BitReader& reader, bool block_header_flag);

  /**
   * @brief Writes the `MITClassConfig` to a bitstream.
   *
   * Serializes the class ID and descriptor IDs into the specified bitstream
   * writer. This method is used when exporting the `MITClassConfig` to an
   * MPEG-G compatible format.
   *
   * @param writer Reference to the `BitWriter` used for output.
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief Adds a new descriptor ID to the class configuration.
   *
   * Appends the given descriptor ID to the internal list of descriptors,
   * associating it with this class type.
   *
   * @param desc The descriptor ID to add.
   */
  void AddDescriptorId(core::GenDesc desc);

  /**
   * @brief Retrieves the class type ID.
   *
   * Returns the class ID of this MIT class, indicating its type within the
   * MPEG-G structure.
   *
   * @return The class type ID.
   */
  [[nodiscard]] core::record::ClassType GetClassId() const;

  /**
   * @brief Retrieves the list of descriptor IDs associated with the class.
   *
   * Provides access to the internal vector of descriptor IDs, which define the
   * properties of the class type.
   *
   * @return Constant reference to the vector of descriptor IDs.
   */
  [[nodiscard]] const std::vector<core::GenDesc>& GetDescriptorIDs() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_MIT_CLASS_CONFIG_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
