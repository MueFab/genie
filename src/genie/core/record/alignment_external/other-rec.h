/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ALIGNMENT_EXT_OTHER_REC_H
#define GENIE_ALIGNMENT_EXT_OTHER_REC_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <record/alignment-external.h>
#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace alignment_external {

/**
 *
 */
class OtherRec : public AlignmentExternal {
    uint64_t next_pos;     //!<
    uint16_t next_seq_ID;  //!<

   public:
    /**
     *
     * @param _next_pos
     * @param _next_seq_ID
     */
    OtherRec(uint64_t _next_pos, uint16_t _next_seq_ID);

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
     */
    OtherRec();

    /**
     *
     * @param reader
     */
    explicit OtherRec(util::BitReader &reader);

    /**
     *
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     *
     * @return
     */
    std::unique_ptr<AlignmentExternal> clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace alignment_external
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_OTHER_REC_H_EXT

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------