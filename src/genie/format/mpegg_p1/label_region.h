/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_DATASET_REGION_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_DATASET_REGION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/record/class-type.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class LabelRegion {
 private:
    uint16_t seq_ID;
    std::vector<genie::core::record::ClassType> class_IDs;
    uint64_t start_pos;
    uint64_t end_pos;

 public:
    /**
     *
     * @param _seq_ID
     * @param _start_pos
     * @param end_pos
     */
    LabelRegion(uint16_t _seq_ID, uint64_t _start_pos, uint64_t end_pos);
    /**
     *
     * @param reader
     */
    explicit LabelRegion(util::BitReader& reader);

    /**
     *
     * @param _class_ID
     */
    void addClassID(genie::core::record::ClassType _class_ID);

    /**
     *
     * @return
     */
    uint16_t getSeqID() const;

    /**
     *
     * @return
     */
    const std::vector<genie::core::record::ClassType>& getClassIDs() const;
    /**
     *
     * @return
     */
    uint64_t getStartPos() const;
    /**
     *
     * @return
     */
    uint64_t getEndPos() const;

    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;


    uint64_t sizeInBits() const {
        return sizeof(uint16_t) * 8  + 4 + 4 * class_IDs.size() + 40 + 40;
    }

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_DATASET_REGION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
