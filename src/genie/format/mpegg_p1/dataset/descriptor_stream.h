/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/format/mpegg_p1/file_header.h>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include "descriptor_stream_header.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#include <genie/format/mpegg_p1/dataset/descriptor_stream_header.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DSProtection {
 private:
    std::vector<uint8_t> DS_protection_value;  //!< @brief

 public:
    /**
     * @brief Default
     */
    DSProtection();
    /**
     * @brief
     * @param bit_reader
     * @param length
     */
    DSProtection(genie::util::BitReader& bit_reader, size_t length);

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

/**
 * @brief
 */
class DescriptorStream {
 private:
    /* ----- internal ----- */
    std::string minor_version;

    DescriptorStreamHeader header;  //!< @brief ISO 23092-1 Section 6.6.4.2
    DSProtection DS_protection;  //!< @brief ISO 23092-1 Section 6.6.4.3
    std::list<uint8_t> block_payload;  //!< @brief ISO 23092-2

 public:
    /**
     * @brief Default
     */
    DescriptorStream();

    /**
     * @brief
     * @param bit_reader
     * @param length
     */
    DescriptorStream(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

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

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
