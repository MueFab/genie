/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_HEADER_H_

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
class DatasetGroupHeader : public GenInfo {
 private:
    uint8_t ID;                         //!< @brief
    uint8_t version;                    //!< @brief
    std::vector<uint16_t> dataset_IDs;  //!< @brief

 public:
    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     */
    DatasetGroupHeader();

    /**
     * @brief
     */
    DatasetGroupHeader(uint8_t _id, uint8_t _version);

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     * @param reader
     */
    explicit DatasetGroupHeader(genie::util::BitReader& reader);

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
    const std::vector<uint16_t>& getDatasetIDs() const;

    /**
     * @brief
     * @param _id
     */
    void addDatasetID(uint16_t _id);

    /**
     * @brief
     * @return
     */
    uint64_t getSize() const override;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
