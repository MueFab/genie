#include "meta-alignment.h"

namespace format {
    namespace mpegg_rec {

        MetaAlignment::MetaAlignment(
                uint16_t _seq_ID,
                uint8_t _as_depth
        ) : seq_ID(_seq_ID),
            as_depth(_as_depth) {
        }

        void MetaAlignment::write(util::BitWriter *write) const {

        }

        MetaAlignment::MetaAlignment(util::BitReader *reader) {

        }

        uint16_t MetaAlignment::getSeqID() const {
            return seq_ID;
        }

        uint8_t MetaAlignment::getAsDepth() const {
            return as_depth;
        }
    }
}