#ifndef GENIE_PART1_DATASET_HEADER_SEQ_DATA_H
#define GENIE_PART1_DATASET_HEADER_SEQ_DATA_H

#include <cstdint>
#include <vector>

#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class SequenceInfo {
   private:
    uint8_t reference_ID;
    std::vector<uint16_t> seq_IDs;
    std::vector<uint16_t> seq_blocks;

    // Additional Info tflag, thres
    std::vector<uint32_t> thress;

   public:
    SequenceInfo();

    explicit SequenceInfo(uint8_t _ref_ID);

    bool anySeq() const;
    bool isValid() const;

    void setRefID(uint8_t _ref_ID);
    void addEntry(uint16_t seq_ID, uint32_t seq_block);
    void addEntries(std::vector<uint16_t>& _seq_IDs, std::vector<uint32_t>& _seq_blocks);

    uint16_t getSeqCount() const;


    void write(genie::util::BitWriter& bit_writer) const;

    void writeThres(genie::util::BitWriter& bit_writer) const;

};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_HEADER_SEQ_DATA_H
