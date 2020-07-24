#ifndef GENIE_PART1_DATASET_HEADER_ORDERED_BLOCKS_H
#define GENIE_PART1_DATASET_HEADER_ORDERED_BLOCKS_H

#include "block_header_flags.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class WithoutHeader: public BlockHeader {
   private:
    bool ordered_blocks_flag;

   public:
    WithoutHeader();

    explicit WithoutHeader(bool _cc_mode_flag);

    void write(genie::util::BitWriter& bit_writer) const override;

   protected:
    bool block_header_flag;
};


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_HEADER_ORDERED_BLOCKS_H
