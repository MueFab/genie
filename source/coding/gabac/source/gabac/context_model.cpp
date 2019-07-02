/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "gabac/context_model.h"


namespace gabac {


ContextModel::ContextModel(
        unsigned char state
)
        : m_state(state){
    // Nothing to do here
}


ContextModel::~ContextModel() = default;


}  // namespace gabac
