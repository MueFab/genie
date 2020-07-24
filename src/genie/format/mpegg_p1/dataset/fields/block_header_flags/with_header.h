#ifndef GENIE_PART1_DATASET_HEADER_MIT_H
#define GENIE_PART1_DATASET_HEADER_MIT_H

#include "block_header_flags.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class WithHeader : public BlockHeader {
   private:
    bool CC_mode_flag;

   public:
    WithHeader();

    WithHeader(bool _mit_flag, bool _cc_mode_flag);

    void write(genie::util::BitWriter& bit_writer) const override;

   protected:
    bool block_header_flag;
    bool getMITFlag() const;
};


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_HEADER_MIT_H
