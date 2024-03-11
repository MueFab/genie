/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include "contact_matrix_tile_payload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

size_t ContactMatrixTilePayload::getSize() const {
    if (codec_ID != core::AlgoID::JBIG){
        return 1 + 4 + 4 + getPayloadSize();
    } else {
        return 1 + getPayloadSize();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t ContactMatrixTilePayload::getPayloadSize() const { return payload.size(); }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixTilePayload::write(core::Writer& writer) const {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------