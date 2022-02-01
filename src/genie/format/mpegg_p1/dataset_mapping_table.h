/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DataSetMappingTable : public GenInfo {
 public:
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
        Type getDataType() const;

        /**
         * @brief
         * @return
         */
        uint8_t getReserved() const;

        /**
         * @brief
         * @return
         */
        uint16_t getDataSID() const;
    };

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     * @param bitWriter
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;


    /**
     * @brief
     * @return
     */
    uint16_t getDatasetID() const;

    /**
     * @brief
     * @return
     */
    const std::vector<DataStream>& getDataStreams() const;

    /**
     * @brief
     * @param d
     */
    void addDataStream(DataStream d);

    /**
     * @brief
     * @param _dataset_id
     */
    explicit DataSetMappingTable(uint16_t _dataset_id);

    /**
     * @brief
     * @param reader
     */
    explicit DataSetMappingTable(util::BitReader& reader);

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

 private:
    uint16_t dataset_id;              //!< @brief
    std::vector<DataStream> streams;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
