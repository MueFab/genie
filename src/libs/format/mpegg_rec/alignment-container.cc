#include "alignment-container.h"

namespace format {
    namespace mpegg_rec {
        void AlignmentContainer::write(util::BitWriter *writer) const {

        }

        AlignmentContainer::AlignmentContainer(util::BitReader *reader) {

        }

        uint32_t AlignmentContainer::getAsDepth() const {
            return alignment->getAsDepth();
        }
    }
}