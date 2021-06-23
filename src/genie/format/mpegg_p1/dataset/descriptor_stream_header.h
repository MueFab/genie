/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DescriptorStreamHeader {
 private:
    uint8_t reserved;  //!< @brief u(1)

    uint8_t descriptor_ID;  //!< @brief u(7)

    uint8_t class_ID;  //!< @brief u(4)

    uint32_t num_blocks;  //!< @brief u(32)

 public:
    /**
     * @brief Default
     */
    DescriptorStreamHeader();
    /**
     * @brief
     * @param _res
     * @param _descriptor_ID
     * @param _class_ID
     * @param _num_blocks
     */
    explicit DescriptorStreamHeader(uint8_t _res, uint8_t _descriptor_ID, uint8_t _class_ID, uint32_t _num_blocks);

    /**
     * @brief
     * @param bit_reader
     */
    explicit DescriptorStreamHeader(util::BitReader& bit_reader);

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
