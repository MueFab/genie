#include "external-alignment.h"

#include <util/bitreader.h>
#include <util/exceptions.h>
#include <util/make_unique.h>

#include "external_alignment/external-alignment-none.h"
#include "external_alignment/external-alignment-other-rec.h"

namespace format {
    namespace mpegg_rec {
        ExternalAlignment::ExternalAlignment(MoreAlignmentInfoType _moreAlignmentInfoType) : moreAlignmentInfoType(
                _moreAlignmentInfoType) {

        }

        void ExternalAlignment::write(util::BitWriter *writer) const {

        }

        std::unique_ptr<ExternalAlignment> ExternalAlignment::factory(util::BitReader *reader) {
            auto type = MoreAlignmentInfoType(reader->read(8));
            switch (type) {
                case MoreAlignmentInfoType::NONE:
                    return util::make_unique<ExternalAlignmentNone>(reader);
                case MoreAlignmentInfoType::OTHER_REC:
                    return util::make_unique<ExternalAlignmentOtherRec>(reader);
                default:
                    UTILS_DIE("Unknown MoreAlignmentInfoType");
            }
        }
    }
}