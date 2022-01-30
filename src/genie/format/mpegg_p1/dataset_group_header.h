/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/file_header.h"
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
class DatasetGroupHeader : public GenInfo {
 private:
    /** ------------------------------------------------------------------------------------------------------------
     * ISO 23092-1 Section 6.5.1.2 table 9 - dataset_group_header
     * ------------------------------------------------------------------------------------------------------------- */
    uint8_t ID;                         //!< @brief
    uint8_t version;                    //!< @brief
    std::vector<uint16_t> dataset_IDs;  //!< @brief

 public:
    /**
     * @brief
     */
    DatasetGroupHeader(uint8_t _id, uint8_t _version) : ID(_id), version(_version) {

    }

    const std::string& getKey() const override {
        static const std::string key = "dghd";
        return key;
    }


    explicit DatasetGroupHeader(genie::util::BitReader& reader) {
        auto length = reader.readBypassBE<uint64_t>();
        auto num_datasets = (length - 14) / 2;
        dataset_IDs.resize(num_datasets);
        ID = reader.readBypassBE<uint8_t>();
        version = reader.readBypassBE<uint8_t>();
        for(auto& d : dataset_IDs) {
            d = reader.readBypassBE<uint16_t>();
        }
    }

    /**
     * @brief
     * @return
     */
    uint8_t getID() const;

    /**
     * @brief
     * @param _ID
     */
    void setID(uint8_t _ID);

    /**
     * @brief
     * @return
     */
    uint8_t getVersion() const;

    /**
     * @brief
     * @return
     */
    const std::vector<uint16_t>& getDatasetIDs() const {
        return dataset_IDs;
    }

    void addDatasetID(uint16_t _id) {
        ID = _id;
    }

    /**
     * @brief
     * @return
     */
    uint64_t getSize() const override {
        return GenInfo::getSize() + sizeof(uint8_t) * 2 + sizeof(uint16_t) * dataset_IDs.size();
    }

    /**
     * @brief
     * @param writer
     * @param empty_length
     */
    void write(genie::util::BitWriter& writer) const override {
        GenInfo::write(writer);
        writer.writeBypassBE<uint8_t>(ID);
        writer.writeBypassBE<uint8_t>(version);
        for(auto& d : dataset_IDs) {
            writer.writeBypassBE<uint16_t>(d);
        }
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
