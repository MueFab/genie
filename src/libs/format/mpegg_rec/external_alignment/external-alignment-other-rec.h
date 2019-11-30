#ifndef GENIE_EXTERNAL_ALIGNMENT_OTHER_REC_H
#define GENIE_EXTERNAL_ALIGNMENT_OTHER_REC_H

#include <cstdint>

#include "external-alignment.h"

namespace util {
    class BitWriter;
    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class ExternalAlignmentOtherRec : public ExternalAlignment {
            uint64_t next_pos : 40;
            uint16_t next_seq_ID : 16;
        public:
            ExternalAlignmentOtherRec(
                    uint64_t _next_pos,
                    uint16_t _next_seq_ID
            );

            ExternalAlignmentOtherRec();

            explicit ExternalAlignmentOtherRec(util::BitReader *reader);

            void write(util::BitWriter *writer) const override;

            virtual std::unique_ptr<ExternalAlignment> clone () const;
        };
    }
}


#endif //GENIE_EXTERNAL_ALIGNMENT_OTHER_REC_H
