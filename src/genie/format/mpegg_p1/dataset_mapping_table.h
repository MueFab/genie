/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DataSetMappingTable : public GenInfo {
 public:
    class DataStream {
     public:
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
        Type data_type;
        uint8_t reserved;
        uint16_t data_SID;

     public:
        DataStream(Type _data_type, uint8_t _reserved, uint16_t _data_SID)
            : data_type(_data_type), reserved(_reserved), data_SID(_data_SID) {}

        DataStream(util::BitReader& reader) {
            data_type = reader.read<Type>(8);
            reserved = reader.read<uint8_t>(3);
            data_SID = reader.read<uint16_t>(13);
        }

        void write(util::BitWriter& writer) const {
            writer.write(static_cast<uint8_t>(data_type), 8);
            writer.write(reserved, 3);
            writer.write(data_SID, 13);
        }

        Type getDataType() const {
            return data_type;
        }

        uint8_t getReserved() const {
            return reserved;
        }

        uint16_t getDataSID() const {
            return data_SID;
        }
    };

    const std::string& getKey() const override {
        static const std::string key = "dmtb";
        return key;
    }

    /**
     * @brief
     * @param bitWriter
     */
    void write(genie::util::BitWriter& bitWriter) const override {
        GenInfo::write(bitWriter);
        bitWriter.writeBypassBE(dataset_id);
        for (const auto& s : streams) {
            s.write(bitWriter);
        }
    }

    uint64_t getSize() const override {
        return GenInfo::getSize() + sizeof(uint16_t) + (sizeof(uint8_t) + sizeof(uint16_t)) * streams.size();
    }

    uint16_t getDatasetID() const {
        return dataset_id;
    }

    const std::vector<DataStream>& getDataStreams() const {
        return streams;
    }

    void addDataStream(DataStream d) {
        streams.emplace_back(std::move(d));
    }

    explicit DataSetMappingTable(uint16_t _dataset_id) : dataset_id(_dataset_id) {

    }

    explicit DataSetMappingTable(util::BitReader& reader) {
        uint64_t length = reader.readBypassBE<uint64_t>();
        dataset_id = reader.readBypassBE<uint16_t>();
        size_t num_data_streams = (length - 14) / 3;
        for (size_t i = 0; i < num_data_streams; ++i) {
            streams.emplace_back(reader);
        }
    }

 private:
    uint16_t dataset_id;
    std::vector<DataStream> streams;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
