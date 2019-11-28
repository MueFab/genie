#ifndef GENIE_SPLIT_ALIGNMENT_H
#define GENIE_SPLIT_ALIGNMENT_H

#include <cstdint>

namespace util {
    class BitWriter;
    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class SplitAlignment {
        public:
            enum class SplitAlignmentType : uint8_t {
                SAME_REC = 0,
                OTHER_REC = 1,
                UNPAIRED = 2
            };

            explicit SplitAlignment(SplitAlignmentType _split_alignment);

            virtual ~SplitAlignment() = default;

        private:
            SplitAlignmentType split_alignment : 8;

        };
    }
}


#endif //GENIE_SPLIT_ALIGNMENT_H
