/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ALIGNMENT_CONTAINER_H
#define GENIE_ALIGNMENT_CONTAINER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <format/mpegg_rec/split_alignment/split-alignment-same-rec.h>
#include <format/mpegg_rec/split_alignment/split-alignment-unpaired.h>
#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/exceptions.h>
#include <util/make_unique.h>
#include <cstdint>
#include <memory>
#include <vector>
#include "alignment.h"
#include "class-type.h"
#include "split-alignment.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_rec {

/**
 *ExternalAlignment
 */
class AlignmentContainer {
    uint64_t mapping_pos{};                                           //!<
    Alignment alignment;                                              //!<
    std::vector<std::unique_ptr<SplitAlignment>> splitAlignmentInfo;  //!<

   public:
    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     *
     * @param type
     * @param as_depth
     * @param number_of_template_segments
     * @param reader
     */
    explicit AlignmentContainer(ClassType type, uint8_t as_depth, uint8_t number_of_template_segments,
                                util::BitReader& reader);

    /**
     *
     * @param _mapping_pos
     * @param _alignment
     */
    AlignmentContainer(uint64_t _mapping_pos, Alignment&& _alignment);

    /**
     *
     * @param container
     */
    AlignmentContainer(const AlignmentContainer& container);

    /**
     *
     * @param container
     */
    AlignmentContainer(AlignmentContainer&& container) noexcept;

    /**
     *
     */
    ~AlignmentContainer() = default;

    /**
     *
     * @param container
     * @return
     */
    AlignmentContainer& operator=(const AlignmentContainer& container);

    /**
     *
     * @param container
     * @return
     */
    AlignmentContainer& operator=(AlignmentContainer&& container) noexcept;

    /**
     *
     * @param _alignment
     */
    void addSplitAlignment(std::unique_ptr<SplitAlignment> _alignment);

    /**
     *
     */
    AlignmentContainer();

    /**
     *
     * @return
     */
    uint64_t getPosition() const;

    /**
     *
     * @return
     */
    const Alignment& getAlignment() const;

    /**
     *
     * @return
     */
    const std::vector<std::unique_ptr<SplitAlignment>>& getSplitAlignments() const;

    /**
     *
     * @return
     */
    uint8_t getNumberOfTemplateSegments() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ALIGNMENT_CONTAINER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------