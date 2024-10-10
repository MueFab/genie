/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_BOX_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_BOX_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <vector>
#include "genie/core/record/alignment-split.h"
#include "genie/core/record/alignment.h"
#include "genie/core/record/class-type.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief ExternalAlignment
 */
class AlignmentBox {
    uint64_t mapping_pos{};                                           //!< @brief
    Alignment alignment;                                              //!< @brief
    std::vector<std::unique_ptr<AlignmentSplit>> splitAlignmentInfo;  //!< @brief

 public:
    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer) const;

    /**
     * @brief
     * @param type
     * @param as_depth
     * @param number_of_template_segments
     * @param reader
     */
    explicit AlignmentBox(ClassType type, uint8_t as_depth, uint8_t number_of_template_segments,
                          util::BitReader& reader);

    /**
     * @brief
     * @param _mapping_pos
     * @param _alignment
     */
    AlignmentBox(uint64_t _mapping_pos, Alignment&& _alignment);

    /**
     * @brief
     * @param container
     */
    AlignmentBox(const AlignmentBox& container);

    /**
     * @brief
     * @param container
     */
    AlignmentBox(AlignmentBox&& container) noexcept;

    /**
     * @brief
     */
    ~AlignmentBox() = default;

    /**
     * @brief
     * @param container
     * @return
     */
    AlignmentBox& operator=(const AlignmentBox& container);

    /**
     * @brief
     * @param container
     * @return
     */
    AlignmentBox& operator=(AlignmentBox&& container) noexcept;

    /**
     * @brief
     * @param _alignment
     */
    void addAlignmentSplit(std::unique_ptr<AlignmentSplit> _alignment);

    /**
     * @brief
     */
    AlignmentBox();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint64_t getPosition() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const Alignment& getAlignment() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<std::unique_ptr<AlignmentSplit>>& getAlignmentSplits() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getNumberOfTemplateSegments() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_BOX_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
