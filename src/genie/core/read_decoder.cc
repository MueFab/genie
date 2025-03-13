/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <utility>
#include "genie/core/read_decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

void ReadDecoder::setQVCoder(QvSelector* coder) { qvcoder = coder; }

// ---------------------------------------------------------------------------------------------------------------------

void ReadDecoder::setNameCoder(NameSelector* coder) { namecoder = coder; }

// ---------------------------------------------------------------------------------------------------------------------

void ReadDecoder::setEntropyCoder(EntropySelector* coder) { entropycoder = coder; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit ReadDecoder::entropyCodeAU(EntropySelector* select, AccessUnit&& a, bool mmCoderEnabled) {
    AccessUnit au = std::move(a);
    for (auto& d : au) {
        auto enc = select->Process(au.getParameters().getDescriptor(d.getID()), d, mmCoderEnabled);
        au.set(d.getID(), std::move(std::get<0>(enc)));
        au.getStats().Add(std::get<1>(enc));
    }
    return au;
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit ReadDecoder::entropyCodeAU(AccessUnit&& a, bool mmCoderEnabled) {
    return entropyCodeAU(entropycoder, std::move(a), mmCoderEnabled);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
