/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "read-encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

void ReadEncoder::setQVCoder(util::SideSelector<QVEncoder, QVEncoder::QVCoded, const record::Chunk&>* coder) {
    qvcoder = coder;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadEncoder::setNameCoder(
    util::SideSelector<NameEncoder, AccessUnit::Descriptor, const record::Chunk&>* coder) {
    namecoder = coder;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------