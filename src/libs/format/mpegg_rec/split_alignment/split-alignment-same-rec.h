#ifndef GENIE_SPLIT_ALIGNMENT_SAME_REC_H
#define GENIE_SPLIT_ALIGNMENT_SAME_REC_H

#include <cstdint>
#include <memory>
#include <format/mpegg_rec/alignment.h>
#include <format/mpegg_rec/split-alignment.h>

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
            SplitAlignmentSameRec();

            SplitAlignmentSameRec(
                    int64_t _delta,
                    std::unique_ptr<Alignment> _alignment
            );

            explicit SplitAlignmentSameRec(
                    uint8_t as_depth,
                    util::BitReader* reader
            );

            void write(util::BitWriter *writer) const override;

            const Alignment* getAlignment() const;

            int64_t getDelta() const;

            std::unique_ptr<SplitAlignment> clone () const override;
        };
    }
}


#endif //GENIE_SPLIT_ALIGNMENT_SAME_REC_H
