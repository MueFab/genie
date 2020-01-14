/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "external-alignment.h"
#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/exceptions.h>
#include <util/make_unique.h>
#include "external_alignment/external-alignment-none.h"
#include "external_alignment/external-alignment-other-rec.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_rec {

// ---------------------------------------------------------------------------------------------------------------------

ExternalAlignment::ExternalAlignment(MoreAlignmentInfoType _moreAlignmentInfoType)
    : moreAlignmentInfoType(_moreAlignmentInfoType) {}

// ---------------------------------------------------------------------------------------------------------------------

void ExternalAlignment::write(util::BitWriter &writer) const { writer.write(uint8_t(moreAlignmentInfoType), 8); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<ExternalAlignment> ExternalAlignment::factory(util::BitReader &reader) {
    auto type = MoreAlignmentInfoType(reader.read(8));
    switch (type) {
        case MoreAlignmentInfoType::NONE:
            return util::make_unique<ExternalAlignmentNone>();
        case MoreAlignmentInfoType::OTHER_REC:
            return util::make_unique<ExternalAlignmentOtherRec>(reader);
        default:
            UTILS_DIE("Unknown MoreAlignmentInfoType");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------