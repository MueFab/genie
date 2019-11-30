#ifndef GENIE_SPLIT_ALIGNMENT_UNPAIRED_H
#define GENIE_SPLIT_ALIGNMENT_UNPAIRED_H

#include <format/mpegg_rec/split-alignment.h>

namespace util {
    class BitWriter;
    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class SplitAlignmentUnpaired : public SplitAlignment {
        public:
            SplitAlignmentUnpaired();

            void write(util::BitWriter *writer) const override;

            std::unique_ptr<SplitAlignment> clone () const override;
        };
    }
}


#endif //GENIE_SPLIT_ALIGNMENT_UNPAIRED_H
