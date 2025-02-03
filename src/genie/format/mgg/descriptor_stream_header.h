/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/constants.h"
#include "genie/core/record/alignment/class-type.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

/**
 * @brief
 */
class DescriptorStreamHeader : public GenInfo {
 private:
    bool reserved;                       //!< @brief
    genie::core::GenDesc descriptor_id;  //!< @brief
    core::record::ClassType class_id;    //!< @brief
    uint32_t num_blocks;                 //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    bool getReserved() const;

    /**
     * @brief
     * @return
     */
    genie::core::GenDesc getDescriptorID() const;

    /**
     * @brief
     * @return
     */
    core::record::ClassType getClassType() const;

    /**
     * @brief
     * @return
     */
    uint32_t getNumBlocks() const;

    /**
     * @brief
     */
    void addBlock();

    /**
     * @brief
     */
    DescriptorStreamHeader();

    /**
     * @brief
     * @param _reserved
     * @param _genDesc
     * @param _class_id
     * @param _num_blocks
     */
    explicit DescriptorStreamHeader(bool _reserved, genie::core::GenDesc _genDesc, core::record::ClassType _class_id,
                                    uint32_t _num_blocks);

    /**
     * @brief
     * @param bitWriter
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @param reader
     */
    explicit DescriptorStreamHeader(genie::util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
