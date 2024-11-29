/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief This file defines the `DescriptorStreamHeader` class, which
 * encapsulates the header information of a descriptor stream.
 * @details The `DescriptorStreamHeader` class is responsible for storing and
 * handling metadata related to a descriptor stream in an MPEG-G file. This
 * includes the descriptor ID, the class type, the number of blocks, and a
 * reserved flag. It provides functionalities to read and write this information
 * to bitstreams, manipulate the number of blocks, and compare headers.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_HEADER_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/core/constants.h"
#include "genie/core/record/class_type.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing the header of a descriptor stream.
 * @details This class stores metadata about the descriptor stream, such as the
 * descriptor ID, class type, number of blocks, and a reserved flag. It provides
 * methods to access these properties and to serialize/deserialize the data from
 * and to bitstreams.
 */
class DescriptorStreamHeader final : public GenInfo {
  bool reserved_;  //!< @brief Reserved flag for future use or special handling.
  core::GenDesc descriptor_id_;  //!< @brief Descriptor ID identifying the
                                 //!< type of genomic descriptor.
  core::record::ClassType
      class_id_;  //!< @brief Class type associated with the descriptor stream.
  uint32_t num_blocks_;  //!< @brief Number of blocks present in the descriptor
                         //!< stream.

 public:
  /**
   * @brief Retrieves the reserved flag value.
   * @details Returns the reserved flag, which might be used for special
   * handling or reserved for future use.
   * @return True if the reserved flag is set; otherwise, false.
   */
  [[nodiscard]] bool GetReserved() const;

  /**
   * @brief Retrieves the descriptor ID.
   * @details Returns the descriptor ID, which indicates the type of genomic
   * data being represented by the stream.
   * @return The descriptor ID as a `GenDesc` enumeration value.
   */
  [[nodiscard]] core::GenDesc GetDescriptorId() const;

  /**
   * @brief Retrieves the class type associated with the descriptor stream.
   * @details Returns the class type, which categorizes the type of data within
   * the stream.
   * @return The class type as a `core::record::ClassType` enumeration value.
   */
  [[nodiscard]] core::record::ClassType GetClassType() const;

  /**
   * @brief Retrieves the number of blocks in the descriptor stream.
   * @details Returns the number of blocks that are part of the descriptor
   * stream.
   * @return The number of blocks as a 32-bit unsigned integer.
   */
  [[nodiscard]] uint32_t GetNumBlocks() const;

  /**
   * @brief Increments the number of blocks in the stream by one.
   * @details This function increases the internal block count, indicating that
   * a new block has been added to the stream.
   */
  void AddBlock();

  /**
   * @brief Default constructor for an empty `DescriptorStreamHeader`.
   * @details Initializes an empty descriptor stream header with default values
   * for each member.
   */
  DescriptorStreamHeader();

  /**
   * @brief Constructs a `DescriptorStreamHeader` with the given parameters.
   * @details Initializes the descriptor stream header using the provided values
   * for the reserved flag, descriptor ID, class type, and number of blocks.
   * @param reserved The reserved flag.
   * @param gen_desc The descriptor ID.
   * @param class_id The class type associated with the descriptor stream.
   * @param num_blocks The initial number of blocks in the descriptor stream.
   */
  explicit DescriptorStreamHeader(bool reserved, genie::core::GenDesc gen_desc,
                                  core::record::ClassType class_id,
                                  uint32_t num_blocks);

  /**
   * @brief Serializes the descriptor stream header to a `BitWriter`.
   * @details Writes the descriptor stream header data into the specified
   * `BitWriter` object, using the format defined by the MPEG-G standard.
   * @param bit_writer The `BitWriter` to write the serialized data to.
   */
  void BoxWrite(util::BitWriter& bit_writer) const override;

  /**
   * @brief Constructs a `DescriptorStreamHeader` by reading from a `BitReader`.
   * @details Reads the descriptor stream header data from the specified
   * `BitReader`, initializing the object with the extracted values.
   * @param reader The `BitReader` to read the descriptor stream header data
   * from.
   */
  explicit DescriptorStreamHeader(util::BitReader& reader);

  /**
   * @brief Retrieves the unique key identifying the `DescriptorStreamHeader`
   * object.
   * @details The key is used to identify this type of metadata box within an
   * MPEG-G file.
   * @return A constant reference to the string representing the key identifier.
   */
  [[nodiscard]] const std::string& GetKey() const override;

  /**
   * @brief Compares two `DescriptorStreamHeader` objects for equality.
   * @details Checks if the descriptor ID, class type, number of blocks, and
   * reserved flag match between the two objects.
   * @param info The other `GenInfo` object to compare against.
   * @return True if both objects have identical values; otherwise, false.
   */
  bool operator==(const GenInfo& info) const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_HEADER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
