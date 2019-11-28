#include "split-alignment-other-rec.h"

namespace format {
    namespace mpegg_rec {
        SplitAlignmentOtherRec::SplitAlignmentOtherRec(
                uint64_t _split_pos,
                uint16_t _split_seq_ID
        ) : SplitAlignment(SplitAlignment::SplitAlignmentType::OTHER_REC),
            split_pos(_split_pos),
            split_seq_ID(_split_seq_ID) {

        }
    }
}