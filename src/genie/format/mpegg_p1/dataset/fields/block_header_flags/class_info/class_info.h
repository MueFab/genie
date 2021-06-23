/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_BLOCK_HEADER_FLAGS_CLASS_INFO_CLASS_INFO_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_BLOCK_HEADER_FLAGS_CLASS_INFO_CLASS_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/core/record/class-type.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class ClassInfo {
 private:
    core::record::ClassType clid;         //!< @brief
    std::vector<uint8_t> descriptor_IDs;  //!< @brief

 public:
    /**
     * @brief
     * @param _clid
     */
    explicit ClassInfo(core::record::ClassType _clid);

    /**
     * @brief
     * @param reader
     * @param block_header_flag
     */
    void ReadClassInfo(util::BitReader& reader, bool block_header_flag);

    /**
     * @brief
     * @param _desc_ID
     */
    void addDescriptorID(uint8_t _desc_ID);

    /**
     * @brief
     * @param _desc_IDs
     */
    void addDescriptorIDs(std::vector<uint8_t>& _desc_IDs);

    /**
     * @brief
     * @param _desc_IDs
     */
    void setDescriptorIDs(std::vector<uint8_t>&& _desc_IDs);

    /**
     * @brief
     * @return
     */
    core::record::ClassType getClid() const;

    /**
     * @brief
     * @return
     */
    uint8_t getNumDescriptors() const;

    /**
     * @brief
     * @param block_header_flag
     * @return
     */
    uint64_t getBitLength(bool block_header_flag) const;

    /**
     * @brief
     * @param bit_writer
     * @param block_header_flag
     */
    void write(util::BitWriter& bit_writer, bool block_header_flag) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_BLOCK_HEADER_FLAGS_CLASS_INFO_CLASS_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
