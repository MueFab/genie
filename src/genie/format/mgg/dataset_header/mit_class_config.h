/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_MIT_CLASS_CONFIG_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_MIT_CLASS_CONFIG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/core/constants.h"
#include "genie/core/record/class-type.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace dataset_header {

/**
 * @brief
 */
class MITClassConfig {
 private:
    genie::core::record::ClassType id;                 //!< @brief
    std::vector<genie::core::GenDesc> descriptor_ids;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const MITClassConfig& other) const;

    /**
     * @brief
     * @param _id
     */
    explicit MITClassConfig(genie::core::record::ClassType _id);

    /**
     * @brief
     * @param reader
     * @param block_header_flag
     */
    MITClassConfig(genie::util::BitReader& reader, bool block_header_flag);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @param desc
     */
    void addDescriptorID(genie::core::GenDesc desc);

    /**
     * @brief
     * @return
     */
    genie::core::record::ClassType getClassID() const;

    /**
     * @brief
     * @return
     */
    const std::vector<genie::core::GenDesc>& getDescriptorIDs() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace dataset_header
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_MIT_CLASS_CONFIG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
