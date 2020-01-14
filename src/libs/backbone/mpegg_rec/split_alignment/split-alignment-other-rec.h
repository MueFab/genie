/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SPLIT_ALIGNMENT_OTHER_REC_H
#define GENIE_SPLIT_ALIGNMENT_OTHER_REC_H

// ---------------------------------------------------------------------------------------------------------------------

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <cstdint>
#include "split-alignment.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_rec {

/**
 *
 */
class SplitAlignmentOtherRec : public SplitAlignment {
    uint64_t split_pos;     //!<
    uint16_t split_seq_ID;  //!<

   public:
    /**
     *
     * @param _split_pos
     * @param _split_seq_ID
     */
    SplitAlignmentOtherRec(uint64_t _split_pos, uint16_t _split_seq_ID);

    /**
     *
     * @return
     */
    uint64_t getNextPos() const;

    /**
     *
     * @return
     */
    uint16_t getNextSeq() const;

    /**
     *
     * @param reader
     */
    explicit SplitAlignmentOtherRec(util::BitReader& reader);

    /**
     *
     */
    SplitAlignmentOtherRec();

    /**
     *
     * @param writer
     */
    void write(util::BitWriter& writer) const override;

    /**
     *
     * @return
     */
    std::unique_ptr<SplitAlignment> clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SPLIT_ALIGNMENT_OTHER_REC_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------