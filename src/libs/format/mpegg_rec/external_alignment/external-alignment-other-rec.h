/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_EXTERNAL_ALIGNMENT_OTHER_REC_H
#define GENIE_EXTERNAL_ALIGNMENT_OTHER_REC_H

// ---------------------------------------------------------------------------------------------------------------------

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <cstdint>
#include "external-alignment.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_rec {

/**
 *
 */
class ExternalAlignmentOtherRec : public ExternalAlignment {
    uint64_t next_pos;     //!<
    uint16_t next_seq_ID;  //!<

   public:
    /**
     *
     * @param _next_pos
     * @param _next_seq_ID
     */
    ExternalAlignmentOtherRec(uint64_t _next_pos, uint16_t _next_seq_ID);

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
    ExternalAlignmentOtherRec();

    /**
     *
     * @param reader
     */
    explicit ExternalAlignmentOtherRec(util::BitReader &reader);

    /**
     *
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     *
     * @return
     */
    std::unique_ptr<ExternalAlignment> clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_EXTERNAL_ALIGNMENT_OTHER_REC_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------