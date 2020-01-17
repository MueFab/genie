/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "parameter_set.h"
#include <genie/util/bitwriter.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace paramqv1 {

// ---------------------------------------------------------------------------------------------------------------------

ParameterSetQvps::ParameterSetQvps() {
    // TODO
}

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSetQvps::addCodeBook(const QvCodebook &book) {
    // TODO
    (void)book;  // Silence warning about unused variable
}

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSetQvps::write(util::BitWriter &writer) const {
    // TODO
    (void)writer;  // Silence warning about unused variable
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<ParameterSetQvps> ParameterSetQvps::clone() const {
    auto ret = util::make_unique<ParameterSetQvps>();
    ret->qv_codebooks = qv_codebooks;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------