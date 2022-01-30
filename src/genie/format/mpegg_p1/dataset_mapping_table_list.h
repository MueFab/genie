/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_LIST_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_LIST_H_

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
class DataSetMappingTableList : public GenInfo {
 private:
    uint8_t dataset_group_ID;
    std::vector<uint16_t> dataset_mapping_table_SID;
 public:
    const std::string& getKey() const override {
        static const std::string key = "dmtl";
        return key;
    }

    explicit DataSetMappingTableList(uint8_t _ds_group_id) : dataset_group_ID(_ds_group_id) {
    }

    explicit DataSetMappingTableList(util::BitReader& reader) {
        size_t length = reader.readBypassBE<uint64_t>();
        dataset_group_ID = reader.readBypassBE<uint8_t>();
        size_t num_SIDs = (length - 13) / 2;
        for (size_t i = 0; i < num_SIDs; ++i) {
            dataset_mapping_table_SID.emplace_back(reader.readBypassBE<uint16_t>());
        }
    }

    void addDatasetMappingTableSID(uint16_t sid) {
        dataset_mapping_table_SID.emplace_back(sid);
    }

    /**
     * @brief
     * @param bitWriter
     */
    void write(genie::util::BitWriter& bitWriter) const override {
        GenInfo::write(bitWriter);
        bitWriter.writeBypassBE(dataset_group_ID);
        for (const auto& s : dataset_mapping_table_SID) {
            bitWriter.writeBypassBE(s);
        }
    }

    uint8_t getDatasetGroupID() const {
        return dataset_group_ID;
    }

    const std::vector<uint16_t>& getDatasetMappingTableSIDs() const {
        return dataset_mapping_table_SID;
    }

    uint64_t getSize() const override {
        return GenInfo::getSize() + sizeof(uint8_t) + sizeof(uint16_t) * dataset_mapping_table_SID.size();
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
