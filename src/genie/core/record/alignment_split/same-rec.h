/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SAME_REC_H
#define GENIE_SAME_REC_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <cstdint>
#include <memory>
#include "record/alignment-split.h"
#include "record/alignment.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace alignment_split {

/**
 *
 */
class SameRec : public AlignmentSplit {
   private:
    int64_t delta;        //!<
    Alignment alignment;  //!<

   public:
    /**
     *
     */
    SameRec();

    /**
     *
     * @param _delta
     * @param _alignment
     */
    SameRec(int64_t _delta, Alignment _alignment);

    /**
     *
     * @param as_depth
     * @param reader
     */
    explicit SameRec(uint8_t as_depth, util::BitReader& reader);

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
    std::unique_ptr<AlignmentSplit> clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace alignment_split
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SAME_REC_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------