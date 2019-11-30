#include "meta-alignment.h"

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/make_unique.h>

namespace format {
    namespace mpegg_rec {

        MetaAlignment::MetaAlignment(
        ) : seq_ID(0),
            as_depth(0) {

        }

        MetaAlignment::MetaAlignment(
                uint16_t _seq_ID,
                uint8_t _as_depth
        ) : seq_ID(_seq_ID),
            as_depth(_as_depth) {
        }

        void MetaAlignment::write(util::BitWriter *writer) const {
            writer->write(seq_ID, 16);
            writer->write(as_depth, 8);
        }

        MetaAlignment::MetaAlignment(util::BitReader *reader) {
            seq_ID = reader->read(16);
            as_depth = reader->read(8);
        }

        uint16_t MetaAlignment::getSeqID() const {
            return seq_ID;
        }

        uint8_t MetaAlignment::getAsDepth() const {
            return as_depth;
        }

        std::unique_ptr<MetaAlignment> MetaAlignment::clone() const {
            auto ret = util::make_unique<MetaAlignment>();

            ret->seq_ID = this->seq_ID;
            ret->as_depth = this->as_depth;

            return ret;
        }
    }
}