#ifndef GENIE_RAW_REFERENCE_H
#define GENIE_RAW_REFERENCE_H

// -----------------------------------------------------------------------------------------------------------------

#include <vector>
#include <cstdint>

#include "data_unit.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {

/**
* ISO 23092-2 Section 3.2 table 5, lines 4 to 7
*/
    class RawReferenceSequence {
    private:
        uint16_t sequence_ID : 16; //!< Line 4
        uint64_t seq_start : 40; //!< Line 5
        uint64_t seq_end : 40; //!< Line 6
        std::vector<char> ref_sequence; //!< Line 7

    public:
        RawReferenceSequence();

        virtual void write(BitWriter &writer);
    };

    // -----------------------------------------------------------------------------------------------------------------

/**
* ISO 23092-2 Section 3.2 table 5
*/
    class RawReference : public DataUnit {
    private:
        /**
         * Incorporated (Simplification): ISO 23092-2 Section 3.1 table 3
         * -------------------------------------------------------------------------------------------------------------- */

        uint64_t data_unit_size : 64; //!< Line 4

        /** ------------------------------------------------------------------------------------------------------------- */

        uint16_t seq_count : 16; //!< Line 2
        std::vector<RawReferenceSequence> seqs; //!< For loop Lines 3 to 8

    public:
        RawReference();

        void write(BitWriter *writer) override;
    };

}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_RAW_REFERENCE_H

