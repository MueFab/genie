#ifndef GENIE_SPLIT_ALIGNMENT_SAME_REC_H
#define GENIE_SPLIT_ALIGNMENT_SAME_REC_H

#include <cstdint>
#include <memory>

#include "alignment.h"
#include "split-alignment.h"

namespace util {
    class BitWriter;

    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class SplitAlignmentSameRec : public SplitAlignment {
            int64_t delta : 48;
            std::unique_ptr<Alignment> alignment;

        public:
            SplitAlignmentSameRec(
                    int64_t _delta,
                    std::unique_ptr<Alignment> _alignment
            );
        };
    }
}


#endif //GENIE_SPLIT_ALIGNMENT_SAME_REC_H
