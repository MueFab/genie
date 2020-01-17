/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "computed_ref.h"
#include <genie/util/bitwriter.h>
#include <genie/util/exceptions.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

// ---------------------------------------------------------------------------------------------------------------------

ComputedRef::ComputedRef(Algorithm _cr_alg_ID) : cr_alg_ID(_cr_alg_ID) {
    if (cr_alg_ID == Algorithm::PUSH_IN || cr_alg_ID == Algorithm::LOCAL_ASSEMBLY) {
        extension = util::make_unique<ComputedRefExtended>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ComputedRef::ComputedRef(util::BitReader &reader) {
    cr_alg_ID = Algorithm(reader.read(8));
    if (cr_alg_ID == Algorithm::PUSH_IN || cr_alg_ID == Algorithm::LOCAL_ASSEMBLY) {
        auto pad = reader.read(8);
        auto buffer = reader.read(24);
        extension = util::make_unique<ComputedRefExtended>(pad, buffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ComputedRef::setExtension(std::unique_ptr<ComputedRefExtended> _crps_info) {
    if (!extension) {
        UTILS_THROW_RUNTIME_EXCEPTION("Invalid crps mode for crps info");
    }
    extension = std::move(_crps_info);
}

// ---------------------------------------------------------------------------------------------------------------------

const ComputedRefExtended &ComputedRef::getExtension() const { return *extension; }

// ---------------------------------------------------------------------------------------------------------------------

void ComputedRef::write(util::BitWriter &writer) const {
    writer.write(uint8_t(cr_alg_ID), 8);
    if (extension) {
        extension->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<ComputedRef> ComputedRef::clone() const {
    auto ret = util::make_unique<ComputedRef>(cr_alg_ID);
    ret->setExtension(extension->clone());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------