/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encoder.h"

/// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace base {

void Encoder::setInput(uint8_t inVarID, const core::record::annotation_access_unit::TypedData& data) {
    if (inVarID >= inputs.size()) {
        inputs.resize(inVarID + 1);
    }
    inputs[inVarID] = data;
}

const core::record::annotation_access_unit::TypedData& Encoder::getOutput(uint8_t outVarID) const {
    if (outVarID >= outputs.size()) {
        throw std::out_of_range("Output variable ID out of range");
    }
    return outputs[outVarID];
}

}  // namespace base
}  // namespace entropy
}  // namespace genie
