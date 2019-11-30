#include "external-alignment-none.h"

#include <util/make_unique.h>

namespace format {
    namespace mpegg_rec {
        ExternalAlignmentNone::ExternalAlignmentNone()
                : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::NONE) {

        }

        void ExternalAlignmentNone::write(util::BitWriter *writer) const {
            ExternalAlignment::write(writer);
        }

        std::unique_ptr<ExternalAlignment> ExternalAlignmentNone::clone () const {
            auto ret = util::make_unique<ExternalAlignmentNone>();
            return ret;
        }

    }
}