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

void ReadEncoder::setNameCoder(NameSelector* coder) { namecoder = coder; }

// ---------------------------------------------------------------------------------------------------------------------

void ReadEncoder::setEntropyCoder(EntropySelector* coder) { entropycoder = coder; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit ReadEncoder::entropyCodeAU(EntropySelector* entropycoder, AccessUnit&& a) {
    AccessUnit au = std::move(a);
    for (auto& d : au) {
        auto encoded = entropycoder->process(d);
        au.getParameters().setDescriptor(d.getID(), std::move(std::get<0>(encoded)));
        au.set(d.getID(), std::move(std::get<1>(encoded)));
        au.getStats().add(std::get<2>(encoded));
    }
    return au;
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit ReadEncoder::entropyCodeAU(AccessUnit&& a) { return entropyCodeAU(entropycoder, std::move(a)); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------