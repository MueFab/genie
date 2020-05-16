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
        extension = ComputedRefExtended(0, 0);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ComputedRef::ComputedRef(util::BitReader &reader) {
    cr_alg_ID = Algorithm(reader.read<uint8_t>());
    if (cr_alg_ID == Algorithm::PUSH_IN || cr_alg_ID == Algorithm::LOCAL_ASSEMBLY) {
        auto pad = reader.read<uint8_t>();
        auto buffer = reader.read<uint32_t>(24);
        extension = ComputedRefExtended(pad, buffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ComputedRef::setExtension(ComputedRefExtended &&_crps_info) {
    if (!extension) {
        UTILS_THROW_RUNTIME_EXCEPTION("Invalid crps mode for crps info");
    }
    extension = std::move(_crps_info);
}

// ---------------------------------------------------------------------------------------------------------------------

const ComputedRefExtended &ComputedRef::getExtension() const { return *extension; }

// ---------------------------------------------------------------------------------------------------------------------

ComputedRef::Algorithm ComputedRef::getAlgorithm() const { return cr_alg_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void ComputedRef::write(util::BitWriter &writer) const {
    writer.write(uint8_t(cr_alg_ID), 8);
    if (extension) {
        extension->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------