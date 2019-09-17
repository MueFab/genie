#ifndef GENIE_RAW_REFERENCE_H
#define GENIE_RAW_REFERENCE_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "data_unit.h"
#include "raw_reference_sequence.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
/**
 * ISO 23092-2 Section 3.2 table 5
 */
class RawReference : public DataUnit {
   private:
    /**
     * Incorporated (Simplification): ISO 23092-2 Section 3.1 table 3
     * -------------------------------------------------------------------------------------------------------------- */

    //!< uint64_t data_unit_size : 64; //!< Line 4, currently computed on the fly

    /** ------------------------------------------------------------------------------------------------------------- */

    //!< uint16_t seq_count : 16; //!< Line 2, currently computed by vector
    std::vector<std::unique_ptr<RawReferenceSequence>> seqs;  //!< For loop Lines 3 to 8

   public:
    RawReference();

    void addSequence(std::unique_ptr<RawReferenceSequence> ref);

    std::unique_ptr<RawReference> clone() const;

    void write(BitWriter *writer) const override;
};

}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_RAW_REFERENCE_H
