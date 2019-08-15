#ifndef GENIE_PART2_FORMAT_RAW_REFERENCE_H
#define GENIE_PART2_FORMAT_RAW_REFERENCE_H

#include <vector>
#include <cstdint>

#include "part2_format_data_unit.h"

struct Raw_reference_Sequence {
    uint16_t sequence_ID : 16;
    uint64_t seq_start : 40;
    uint64_t seq_end : 40;
    std::vector<char> ref_sequence;
    Raw_reference_Sequence ();
    virtual void write(BitWriter& writer);
};

struct Raw_reference : public Data_unit {
    uint64_t data_unit_size : 64;
    uint16_t seq_count : 16;
    std::vector<Raw_reference_Sequence> seqs;
    Raw_reference();

    void write(BitWriter* writer) override;
};


#endif //GENIE_PART2_FORMAT_RAW_REFERENCE_H
