#ifndef GENIE_META_ALIGNMENT_H
#define GENIE_META_ALIGNMENT_H

#include <cstdint>
#include <memory>

namespace util {
    class BitWriter;

    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class MetaAlignment {
            uint16_t seq_ID : 16;
            uint8_t as_depth : 8;

        public:
            MetaAlignment();
            MetaAlignment(
                    uint16_t _seq_ID,
                    uint8_t _as_depth
            );

            explicit MetaAlignment(util::BitReader *reader);

            virtual void write(util::BitWriter *write) const;

            uint16_t getSeqID() const;

            uint8_t getAsDepth() const;

            std::unique_ptr<MetaAlignment> clone() const;
        };
    }
}


#endif //GENIE_META_ALIGNMENT_H
