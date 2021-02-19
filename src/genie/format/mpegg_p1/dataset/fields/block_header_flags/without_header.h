#ifndef GENIE_PART1_DATASET_HEADER_ORDERED_BLOCKS_H
#define GENIE_PART1_DATASET_HEADER_ORDERED_BLOCKS_H

#include <genie/util/bitwriter.h>
#include <genie/util/bitreader.h>
#include <genie/util/exception.h>
#include <genie/util/runtime-exception.h>


#include "block_header_flags.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class WithoutHeader: public BlockConfig {
   private:
    bool ordered_blocks_flag;

   public:
    WithoutHeader();

    explicit WithoutHeader(bool _cc_mode_flag);

    void ReadWithoutHeader(util::BitReader& reader, size_t length);

    uint64_t getBitLength() const override;

    void write(util::BitWriter& bit_writer) const override;

   protected:
    bool block_header_flag;
};


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_HEADER_ORDERED_BLOCKS_H
