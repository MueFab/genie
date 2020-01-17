/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "context-model.h"

namespace genie {
namespace entropy {
namespace gabac {

ContextModel::ContextModel(unsigned char state) : m_state(state) {}

ContextModel::~ContextModel() = default;

}  // namespace gabac
}  // namespace entropy
}  // namespace genie