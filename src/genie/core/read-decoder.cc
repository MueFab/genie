/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "read-decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

void ReadDecoder::setQVCoder(util::SideSelector<QVDecoder, std::vector<std::string>, const parameter::QualityValues&,
                                                const std::vector<std::string>&, AccessUnitRaw::Descriptor&>* coder) {
    qvcoder = coder;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadDecoder::setNameCoder(
    util::SideSelector<NameDecoder, std::vector<std::string>, AccessUnitRaw::Descriptor&>* coder) {
    namecoder = coder;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------