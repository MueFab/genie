/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "computed_ref_extended.h"
#include <genie/util/bitwriter.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

// ---------------------------------------------------------------------------------------------------------------------

ComputedRefExtended::ComputedRefExtended(uint8_t _cr_pad_size, uint32_t _cr_buf_max_size)
    : cr_pad_size(_cr_pad_size), cr_buf_max_size(_cr_buf_max_size) {}

// ---------------------------------------------------------------------------------------------------------------------

ComputedRefExtended::ComputedRefExtended() : ComputedRefExtended(0, 0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ComputedRefExtended::getBufMaxSize() const { return cr_buf_max_size; }

// ---------------------------------------------------------------------------------------------------------------------

void ComputedRefExtended::write(util::BitWriter &writer) const {
    writer.write(cr_pad_size, 8);
    writer.write(cr_buf_max_size, 24);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<ComputedRefExtended> ComputedRefExtended::clone() const {
    auto ret = util::make_unique<ComputedRefExtended>();
    *ret = *this;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------