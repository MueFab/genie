/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "ordered-section.h"

namespace util {

// ---------------------------------------------------------------------------------------------------------------------

OrderedSection::OrderedSection(OrderedLock* _lock, size_t id) : lock(_lock) { lock->wait(id); }

// ---------------------------------------------------------------------------------------------------------------------

OrderedSection::~OrderedSection() { lock->finished(); }

}  // namespace util

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------