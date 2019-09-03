#ifndef GENIE_PART2_FORMAT_RAW_REFERENCE_H
#define GENIE_PART2_FORMAT_RAW_REFERENCE_H

#include <vector>
#include <cstdint>

#include "part2_format_data_unit.h"

/**
* ISO 23092-2 Section 3.2 table 5, lines 4 to 7
*/
class Raw_reference_Sequence {
private:
    uint16_t sequence_ID : 16; //!< Line 4
    uint64_t seq_start : 40; //!< Line 5
    uint64_t seq_end : 40; //!< Line 6
    std::vector<char> ref_sequence; //!< Line 7

public:
    Raw_reference_Sequence ();
    virtual void write(BitWriter& writer);
};

/**
* ISO 23092-2 Section 3.2 table 5
*/
class Raw_reference : public Data_unit {
private:
    /**
     * Incorporated (Simplification): ISO 23092-2 Section 3.1 table 3
     * -------------------------------------------------------------------------------------------------------------- */

    uint64_t data_unit_size : 64; //!< Line 4

    /** ------------------------------------------------------------------------------------------------------------- */

    uint16_t seq_count : 16; //!< Line 2
    std::vector<Raw_reference_Sequence> seqs; //!< For loop Lines 3 to 8

public:
    Raw_reference();
    void write(BitWriter* writer) override;
};


#endif //GENIE_PART2_FORMAT_RAW_REFERENCE_H
