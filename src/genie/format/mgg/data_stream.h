/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATA_STREAM_H_
#define SRC_GENIE_FORMAT_MGG_DATA_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class DataStream {
 public:
    /**
     * @brief
     */
    enum class Type : uint8_t {
        DATASET_GROUP_HEADER = 0,
        REFERENCE = 1,
        LABEL_LIST = 2,
        DATASET_HEADER = 3,
        DATASET_PARAMETER_SET = 4,
        DATASET_GROUP_METADATA = 5,
        REFERENCE_METADATA = 6,
        DATASET_METADATA = 7,
        DATASET_GROUP_PROTECTION = 8,
        DATASET_PROTECTION = 9,
        ACCESS_UNIT_P = 10,
        ACCESS_UNIT_N = 11,
        ACCESS_UNIT_M = 12,
        ACCESS_UNIT_I = 13,
        ACCESS_UNIT_HM = 14,
        ACCESS_UNIT_U = 15,
        RESERVED = 16
    };

 private:
    Type data_type;     //!< @brief
    uint8_t reserved;   //!< @brief
    uint16_t data_SID;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const DataStream& other) const;

    /**
     * @brief
     * @param _data_type
     * @param _reserved
     * @param _data_SID
     */
    DataStream(Type _data_type, uint8_t _reserved, uint16_t _data_SID);

    /**
     * @brief
     * @param reader
     */
    explicit DataStream(util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] Type getDataType() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getReserved() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getDataSID() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATA_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
