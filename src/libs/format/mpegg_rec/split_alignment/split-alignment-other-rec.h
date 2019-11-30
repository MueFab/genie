#ifndef GENIE_SPLIT_ALIGNMENT_OTHER_REC_H
#define GENIE_SPLIT_ALIGNMENT_OTHER_REC_H

#include <cstdint>

#include "split-alignment.h"

namespace util {
    class BitWriter;
    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class SplitAlignmentOtherRec : public SplitAlignment {
            uint64_t split_pos : 40;
            uint16_t split_seq_ID : 16;

        public:
            SplitAlignmentOtherRec(
                    uint64_t _split_pos,
                    uint16_t _split_seq_ID
            );

            explicit SplitAlignmentOtherRec(
                    util::BitReader* reader
            );

            SplitAlignmentOtherRec();

            void write(util::BitWriter *writer) const override;

            virtual std::unique_ptr<SplitAlignment> clone () const;
        };
    }
}


#endif //GENIE_SPLIT_ALIGNMENT_OTHER_REC_H
