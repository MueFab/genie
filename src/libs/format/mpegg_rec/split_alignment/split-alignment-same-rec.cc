#include "split-alignment-same-rec.h"

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/make_unique.h>
#include <format/mpegg_rec/split-alignment.h>

namespace format {
    namespace mpegg_rec {
        SplitAlignmentSameRec::SplitAlignmentSameRec(
        ) : SplitAlignment(SplitAlignment::SplitAlignmentType::SAME_REC),
            delta(0),
            alignment(nullptr) {
        }

        SplitAlignmentSameRec::SplitAlignmentSameRec(
                int64_t _delta,
                std::unique_ptr<Alignment> _alignment
        ) : SplitAlignment(SplitAlignment::SplitAlignmentType::SAME_REC),
            delta(_delta),
            alignment(std::move(_alignment)) {

        }

        SplitAlignmentSameRec::SplitAlignmentSameRec(
                uint8_t as_depth,
                util::BitReader *reader
        ) : SplitAlignment(SplitAlignment::SplitAlignmentType::SAME_REC) {
            delta = reader->read(48);
            alignment = util::make_unique<Alignment>(as_depth, reader);
        }

        void SplitAlignmentSameRec::write(util::BitWriter *writer) const {
            SplitAlignment::write(writer);
            writer->write(delta, 48);
            alignment->write(writer);
        }

        const Alignment* SplitAlignmentSameRec::getAlignment() const {
            return alignment.get();
        }

        int64_t SplitAlignmentSameRec::getDelta() const {
            return delta;
        }

        std::unique_ptr<SplitAlignment> SplitAlignmentSameRec::clone() const {
            auto ret = util::make_unique<SplitAlignmentSameRec>();
            ret->delta = this->delta;
            ret->alignment = this->alignment->clone();
            return ret;
        }
    }
}