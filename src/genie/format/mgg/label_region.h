/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_LABEL_REGION_H_
#define SRC_GENIE_FORMAT_MGG_LABEL_REGION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/core/meta/region.h"
#include "genie/core/record/class-type.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class LabelRegion {
 private:
    uint16_t seq_ID;                                        //!< @brief
    std::vector<genie::core::record::ClassType> class_IDs;  //!< @brief
    uint64_t start_pos;                                     //!< @brief
    uint64_t end_pos;                                       //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    genie::core::meta::Region decapsulate();

    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const LabelRegion& other) const;

    /**
     * @brief
     * @param _seq_ID
     * @param _start_pos
     * @param end_pos
     */
    LabelRegion(uint16_t _seq_ID, uint64_t _start_pos, uint64_t end_pos);

    /**
     * @brief
     * @param reader
     */
    explicit LabelRegion(util::BitReader& reader);

    /**
     * @brief
     * @param _class_ID
     */
    void addClassID(genie::core::record::ClassType _class_ID);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getSeqID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<genie::core::record::ClassType>& getClassIDs() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint64_t getStartPos() const;

    /**
     *
     * @return
     */
    [[nodiscard]] uint64_t getEndPos() const;

    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint64_t sizeInBits() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_LABEL_REGION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
