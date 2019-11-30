#ifndef GENIE_EXTERNAL_ALIGNMENT_NONE_H
#define GENIE_EXTERNAL_ALIGNMENT_NONE_H

#include "external-alignment.h"

namespace util {
    class BitWriter;
    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class ExternalAlignmentNone : public ExternalAlignment {
        public:
            ExternalAlignmentNone();

            void write(util::BitWriter *writer) const override;

            virtual std::unique_ptr<ExternalAlignment> clone () const;
        };
    }
}


#endif //GENIE_EXTERNAL_ALIGNMENT_NONE_H
