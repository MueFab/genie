/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(std::string _name, size_t length, Manager* m)
    : core::Reference(std::move(_name), 0, length), mgr(m) {}

// ---------------------------------------------------------------------------------------------------------------------

std::string Reference::getSequence(uint64_t _start, uint64_t _end) { return mgr->getRef(getName(), _start, _end); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------