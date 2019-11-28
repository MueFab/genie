#include "external-alignment-none.h"

namespace format {
    namespace mpegg_rec {
        ExternalAlignmentNone::ExternalAlignmentNone()
                : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::NONE) {

        }

        ExternalAlignmentNone::ExternalAlignmentNone(util::BitReader *reader)
                : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::NONE) {

        }

        void ExternalAlignmentNone::write(util::BitWriter *writer) const {

        }

    }
}