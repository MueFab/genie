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
class DatasetGroupHeader {
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
    DatasetGroupHeader();

    /**
     * @brief
     * @param reader
     * @param fhd
     * @param start_pos
     * @param length
     */
    explicit DatasetGroupHeader(genie::util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

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
    std::vector<uint16_t>& getDatasetIDs();

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     * @param empty_length
     */
    void write(genie::util::BitWriter& writer, bool empty_length = false) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
