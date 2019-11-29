#include "split-alignment-same-rec.h"

namespace format {
    namespace mpegg_rec {
        SplitAlignmentSameRec::SplitAlignmentSameRec(
                int64_t _delta,
                std::unique_ptr<Alignment> _alignment
        ) : SplitAlignment(SplitAlignment::SplitAlignmentType::SAME_REC),
            delta(_delta),
            alignment(std::move(_alignment)) {

        }
    }
}