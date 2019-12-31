/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SPLIT_ALIGNMENT_SAME_REC_H
#define GENIE_SPLIT_ALIGNMENT_SAME_REC_H

// ---------------------------------------------------------------------------------------------------------------------

#include <format/mpegg_rec/alignment.h>
#include <format/mpegg_rec/split-alignment.h>
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
class SplitAlignmentSameRec : public SplitAlignment {
    int64_t delta;        //!<
    Alignment alignment;  //!<

   public:
    /**
     *
     */
    SplitAlignmentSameRec();

    /**
     *
     * @param _delta
     * @param _alignment
     */
    SplitAlignmentSameRec(int64_t _delta, Alignment _alignment);

    /**
     *
     * @param as_depth
     * @param reader
     */
    explicit SplitAlignmentSameRec(uint8_t as_depth, util::BitReader& reader);

    /**
     *
     * @param writer
     */
    void write(util::BitWriter& writer) const override;

    /**
     *
     * @return
     */
    const Alignment& getAlignment() const;

    /**
     *
     * @return
     */
    int64_t getDelta() const;

    /**
     *
     * @return
     */
    std::unique_ptr<SplitAlignment> clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SPLIT_ALIGNMENT_SAME_REC_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------