/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_BLOCK_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_BLOCK_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/format/mpegg_p1/dataset/dataset_header.h>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/dataset/access_unit/access_unit_header.h"
#include "genie/format/mpegg_p1/dataset/class_description.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
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
class BlockHeader {
 private:
    uint8_t descriptor_ID;  //!< @brief
    uint32_t block_payload_size;  //!< @brief

 public:
    /**
     *
     * @param reader
     * @param fhd
     */
    explicit BlockHeader(util::BitReader& reader, FileHeader& fhd);

    /**
     * @brief
     * @return
     */
    uint8_t getDescriptorID() const;

    /**
     * @brief
     * @return
     */
    uint32_t getPayloadSize() const;

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_BLOCK_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
