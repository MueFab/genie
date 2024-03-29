/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/alignment-external.h"
#include "genie/core/record/alignment_external/none.h"
#include "genie/core/record/alignment_external/other-rec.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

AlignmentExternal::AlignmentExternal(Type _moreAlignmentInfoType) : moreAlignmentInfoType(_moreAlignmentInfoType) {}

// ---------------------------------------------------------------------------------------------------------------------

void AlignmentExternal::write(util::BitWriter &writer) const { writer.writeBypassBE(moreAlignmentInfoType); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentExternal> AlignmentExternal::factory(util::BitReader &reader) {
    Type type = reader.readBypassBE<Type>();
    switch (type) {
        case Type::NONE:
            return util::make_unique<alignment_external::None>();
        case Type::OTHER_REC:
            return util::make_unique<alignment_external::OtherRec>(reader);
        default:
            UTILS_DIE("Unknown MoreAlignmentInfoType");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
