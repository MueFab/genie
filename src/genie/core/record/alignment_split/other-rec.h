/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_OTHER_REC_H
#define GENIE_OTHER_REC_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <record/alignment-split.h>
#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace alignment_split {

/**
 *
 */
class OtherRec : public AlignmentSplit {
    uint64_t split_pos;     //!<
    uint16_t split_seq_ID;  //!<

   public:
    /**
     *
     * @param _split_pos
     * @param _split_seq_ID
     */
    OtherRec(uint64_t _split_pos, uint16_t _split_seq_ID);

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
    explicit OtherRec(util::BitReader& reader);

    /**
     *
     */
    OtherRec();

    /**
     *
     * @param writer
     */
    void write(util::BitWriter& writer) const override;

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

#endif  // GENIE_OTHER_REC_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------