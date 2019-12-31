/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SPLIT_ALIGNMENT_H
#define GENIE_SPLIT_ALIGNMENT_H

// ---------------------------------------------------------------------------------------------------------------------

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <cstdint>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_rec {

/**
 *
 */
class SplitAlignment {
   public:
    /**
     *
     */
    enum class SplitAlignmentType : uint8_t { SAME_REC = 0, OTHER_REC = 1, UNPAIRED = 2 };

    /**
     *
     * @param _split_alignment
     */
    explicit SplitAlignment(SplitAlignmentType _split_alignment);

    /**
     *
     */
    virtual ~SplitAlignment() = default;

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;

    /**
     *
     * @param as_depth
     * @param reader
     * @return
     */
    static std::unique_ptr<SplitAlignment> factory(uint8_t as_depth, util::BitReader &reader);

    /**
     *
     * @return
     */
    virtual std::unique_ptr<SplitAlignment> clone() const = 0;

    /**
     *
     * @return
     */
    SplitAlignmentType getType() const;

   protected:
    SplitAlignmentType split_alignment;  //!<
};
}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SPLIT_ALIGNMENT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------