/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `DataStream` class for managing different types of data
 * streams in MPEG-G.
 * @details The `DataStream` class provides functionality for handling various
 * data stream types defined in the MPEG-G format. It includes methods for
 * serialization, deserialization, and comparison of data streams, as well as
 * retrieving the properties of each data stream.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATA_STREAM_H_
#define SRC_GENIE_FORMAT_MGG_DATA_STREAM_H_

// -----------------------------------------------------------------------------

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing a single data stream within MPEG-G format.
 * @details The `DataStream` class encapsulates the type, ID, and associated
 * properties of a data stream in the MPEG-G format. It supports reading from
 * and writing to bit streams, and provides methods to access its internal
 * properties.
 */
class DataStream {
 public:
  /**
   * @brief Enum for defining the different types of data streams.
   */
  enum class Type : uint8_t {
    DATASET_GROUP_HEADER =
        0,               //!< @brief Data stream for the Dataset Group Header.
    REFERENCE = 1,       //!< @brief Data stream for Reference data.
    LABEL_LIST = 2,      //!< @brief Data stream for Label List.
    DATASET_HEADER = 3,  //!< @brief Data stream for Dataset Header.
    DATASET_PARAMETER_SET =
        4,  //!< @brief Data stream for Dataset Parameter Set.
    DATASET_GROUP_METADATA =
        5,                   //!< @brief Data stream for Dataset Group Metadata.
    REFERENCE_METADATA = 6,  //!< @brief Data stream for Reference Metadata.
    DATASET_METADATA = 7,    //!< @brief Data stream for Dataset Metadata.
    DATASET_GROUP_PROTECTION =
        8,  //!< @brief Data stream for Dataset Group Protection.
    DATASET_PROTECTION = 9,  //!< @brief Data stream for Dataset Protection.
    ACCESS_UNIT_P = 10,      //!< @brief Data stream for Access Unit of class P.
    ACCESS_UNIT_N = 11,      //!< @brief Data stream for Access Unit of class N.
    ACCESS_UNIT_M = 12,      //!< @brief Data stream for Access Unit of class M.
    ACCESS_UNIT_I = 13,      //!< @brief Data stream for Access Unit of class I.
    ACCESS_UNIT_HM = 14,  //!< @brief Data stream for Access Unit of class HM.
    ACCESS_UNIT_U = 15,   //!< @brief Data stream for Access Unit of class U.
    RESERVED = 16         //!< @brief Reserved data stream type.
  };

 private:
  Type data_type_;     //!< @brief Type of the data stream.
  uint8_t reserved_;   //!< @brief Reserved field, typically set to zero.
  uint16_t data_sid_;  //!< @brief Unique identifier for the data stream.

 public:
  /**
   * @brief Equality operator for comparing two data stream objects.
   * @param other The other `DataStream` object to compare with.
   * @return True if both `DataStream` objects are equivalent, false otherwise.
   */
  bool operator==(const DataStream& other) const;

  /**
   * @brief Constructor to initialize a `DataStream` with specific values.
   * @param data_type Type of the data stream.
   * @param reserved Reserved field (typically set to zero).
   * @param data_sid Unique stream identifier.
   */
  DataStream(Type data_type, uint8_t reserved, uint16_t data_sid);

  /**
   * @brief Constructs a `DataStream` by reading values from a bit reader.
   * @param reader Bit reader to extract data stream information from.
   */
  explicit DataStream(util::BitReader& reader);

  /**
   * @brief Serializes the `DataStream` into a bit writer.
   * @param writer Bit writer to write the `DataStream` to.
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief Retrieves the type of the data stream.
   * @return The type of the data stream as a `Type` enum.
   */
  [[nodiscard]] Type GetDataType() const;

  /**
   * @brief Retrieves the reserved field value.
   * @return The reserved field value as an 8-bit unsigned integer.
   */
  [[nodiscard]] uint8_t GetReserved() const;

  /**
   * @brief Retrieves the unique stream identifier.
   * @return The unique stream identifier as a 16-bit unsigned integer.
   */
  [[nodiscard]] uint16_t GetDataSid() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATA_STREAM_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
