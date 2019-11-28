#include "external-alignment-other-rec.h"

namespace format {
    namespace mpegg_rec {
        ExternalAlignmentOtherRec::ExternalAlignmentOtherRec(
                uint64_t _next_pos,
                uint16_t _next_seq_ID
        ) : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::OTHER_REC),
            next_pos(_next_pos),
            next_seq_ID(_next_seq_ID) {

        }

        ExternalAlignmentOtherRec::ExternalAlignmentOtherRec(util::BitReader *reader)
                : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::OTHER_REC) {

        }

        void ExternalAlignmentOtherRec::write(util::BitWriter *writer) const {

        }
    }
}