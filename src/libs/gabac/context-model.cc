/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "context-model.h"

namespace genie {
namespace gabac {

ContextModel::ContextModel(unsigned char state) : m_state(state) {}

ContextModel::~ContextModel() = default;

}  // namespace gabac
}  // namespace genie
