/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include <genie/format/mpegg_p1/file_header.h>

#include <genie/core/constants.h>
#include <genie/core/record/class-type.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DescriptorStreamHeader {
 private:
    /* ----- internal ----- */
    std::string minor_version;

    uint8_t descriptor_ID;
    core::record::ClassType class_ID;
    uint32_t num_blocks;

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
//    explicit DescriptorStreamHeader(uint8_t _res, uint8_t _descriptor_ID, uint8_t _class_ID, uint32_t _num_blocks);

    /**
     * @brief
     * @param bit_reader
     */
    explicit DescriptorStreamHeader(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer, bool zero_length=false) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
