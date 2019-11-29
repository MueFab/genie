#include "segment.h"

#include <util/exceptions.h>

namespace format {
    namespace mpegg_rec {
        Segment::Segment(
                std::unique_ptr<std::string> _sequence
        ) : sequence(std::move(_sequence)),
            quality_values() {

        }

        Segment::Segment(
                uint32_t length,
                util::BitReader *reader
        ) {

        }

        size_t Segment::getQvDepth() const {
            return quality_values.size();
        }

        size_t Segment::getLength() const {
            return sequence->length();
        }

        void Segment::addQualityValues(std::unique_ptr<std::string> qv) {
            if (qv->length() != sequence->length()) {
                UTILS_DIE("QV and sequence lengths do not match");
            }
            quality_values.push_back(std::move(qv));
        }

        void Segment::write(util::BitWriter *write) const {

        }
    }
}