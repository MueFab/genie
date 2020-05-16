/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "context-model.h"

namespace genie {
namespace entropy {
namespace gabac {

ContextModel::ContextModel(unsigned char initState) {
    unsigned char valMps = (initState >= 64);
    m_state = ((valMps ? (initState - 64) : (63 - initState)) << 1) + valMps;
}

ContextModel::~ContextModel() = default;

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
