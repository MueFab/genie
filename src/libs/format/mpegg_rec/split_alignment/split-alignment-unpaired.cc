#include "split-alignment-unpaired.h"

#include <util/make_unique.h>

namespace format {
    namespace mpegg_rec {
        SplitAlignmentUnpaired::SplitAlignmentUnpaired() : SplitAlignment(
                SplitAlignment::SplitAlignmentType::UNPAIRED) {

        }

        void SplitAlignmentUnpaired::write(util::BitWriter *writer) const {
            SplitAlignment::write(writer);
        }

        std::unique_ptr<SplitAlignment> SplitAlignmentUnpaired::clone () const {
            auto ret = util::make_unique<SplitAlignmentUnpaired>();
            ret->split_alignment = this->split_alignment;
            return ret;
        }
    }
}