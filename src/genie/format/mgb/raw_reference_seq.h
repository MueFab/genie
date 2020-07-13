/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_RAW_REFERENCE_SEQ_H
#define GENIE_RAW_REFERENCE_SEQ_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <memory>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * ISO 23092-2 Section 3.2 table 5, lines 4 to 7
 */
class RawReferenceSequence {
   private:
    uint16_t sequence_ID : 16;  //!< Line 4
    uint64_t seq_start : 40;    //!< Line 5
    //!< uint64_t seq_end : 40; //!< Line 6 currently computed by length of string
    std::string ref_sequence;  //!< Line 7

   public:
    /**
     *
     * @param _sequence_ID
     * @param _seq_start
     * @param _ref_sequence
     */
    RawReferenceSequence(uint16_t _sequence_ID, uint64_t _seq_start, std::string &&_ref_sequence);


    explicit RawReferenceSequence(util::BitReader& reader) {
        sequence_ID = reader.read<uint16_t>();
        seq_start = reader.read(40);
        size_t seq_end = reader.read(40);
        ref_sequence.resize(seq_end - seq_start + 1);
        reader.readBuffer(&ref_sequence[0], seq_end - seq_start + 1);
    }

    /**
     *
     */
    virtual ~RawReferenceSequence() = default;

    /**
     *
     * @param s
     * @return
     */
    bool isIdUnique(const RawReferenceSequence &s) const;

    /**
     *
     * @param writer
     */
    void write(util::BitWriter &writer) const;

    /**
     *
     * @return
     */
    uint64_t getTotalSize() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_RAW_REFERENCE_SEQ_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------