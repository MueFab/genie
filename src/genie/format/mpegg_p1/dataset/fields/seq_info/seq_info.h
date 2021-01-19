#ifndef GENIE_PART1_DATASET_HEADER_SEQ_INFO_H
#define GENIE_PART1_DATASET_HEADER_SEQ_INFO_H

#include <cstdint>
#include <vector>

#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class SequenceConfig {
   private:
    uint8_t reference_ID;
    std::vector<uint16_t> seq_IDs;
    std::vector<uint16_t> seq_blocks;

    std::vector<uint32_t> thress;

   public:
    SequenceConfig();

    explicit SequenceConfig(uint8_t _ref_ID);

    uint64_t getBitLength() const;

    bool anySeq() const;
    bool isValid() const;

    void setRefID(uint8_t _ref_ID);
    void addEntry(uint16_t seq_ID, uint32_t seq_block);
    void addEntries(std::vector<uint16_t>& _seq_IDs, std::vector<uint32_t>& _seq_blocks);

    uint16_t getSeqCount() const;

    const std::vector<uint16_t>& getSeqBlocks() const;

    void write(genie::util::BitWriter& writer) const;

    void writeThres(genie::util::BitWriter& bit_writer) const;

};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_HEADER_SEQ_INFO_H
