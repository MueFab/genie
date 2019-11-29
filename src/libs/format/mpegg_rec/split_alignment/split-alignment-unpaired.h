#ifndef GENIE_SPLIT_ALIGNMENT_UNPAIRED_H
#define GENIE_SPLIT_ALIGNMENT_UNPAIRED_H

#include "split-alignment.h"

namespace util {
    class BitWriter;
    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class SplitAlignmentUnpaired : public SplitAlignment {
        public:
            SplitAlignmentUnpaired();
        };
    }
}


#endif //GENIE_SPLIT_ALIGNMENT_UNPAIRED_H
