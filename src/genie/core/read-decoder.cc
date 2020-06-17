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

void ReadDecoder::setQVCoder(QvSelector* coder) {
    qvcoder = coder;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadDecoder::setNameCoder(
    NameSelector* coder) {
    namecoder = coder;
}

void ReadDecoder::setEntropyCoder(EntropySelector* coder) {
    entropycoder = coder;
}

AccessUnit ReadDecoder::entropyCodeAU(EntropySelector* select, AccessUnit&& a) {
    AccessUnit au = std::move(a);
    for(auto &d : au) {
        auto enc = select->process(au.getParameters().getDescriptor(d.getID()), d);
        d = std::get<0>(enc);
        au.getStats().add(std::get<1>(enc));
    }
    return au;
}

AccessUnit ReadDecoder::entropyCodeAU(AccessUnit&& a) {
    return entropyCodeAU(entropycoder, std::move(a));
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------