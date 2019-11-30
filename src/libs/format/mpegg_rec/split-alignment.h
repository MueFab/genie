#ifndef GENIE_SPLIT_ALIGNMENT_H
#define GENIE_SPLIT_ALIGNMENT_H

#include <cstdint>
#include <memory>

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

            virtual void write(util::BitWriter *writer) const;

            static std::unique_ptr<SplitAlignment> factory(uint8_t as_depth, util::BitReader *reader);

            virtual std::unique_ptr<SplitAlignment> clone () const = 0;

            SplitAlignmentType getType () const {
                return split_alignment;
            }

        protected:
            SplitAlignmentType split_alignment : 8;

        };
    }
}


#endif //GENIE_SPLIT_ALIGNMENT_H
