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

ContactMatrixTilePayload::ContactMatrixTilePayload(
    core::AlgoID _codec_ID,
    uint32_t _tile_nrows,
    uint32_t _tile_ncols,
    std::vector<uint8_t>&& _payload
): codec_ID(_codec_ID),
   tile_nrows(_tile_nrows),
   tile_ncols(_tile_ncols),
   payload(std::move(_payload))
{}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload::ContactMatrixTilePayload(
    core::AlgoID _codec_ID,
    uint32_t _tile_nrows,
    uint32_t _tile_ncols,
    uint8_t** _payload,
    size_t _payload_len
):  codec_ID(_codec_ID),
    tile_nrows(_tile_nrows),
    tile_ncols(_tile_ncols),
    payload(*_payload, *(_payload) + _payload_len)
{
    free(*_payload);
}


// ---------------------------------------------------------------------------------------------------------------------

size_t ContactMatrixTilePayload::getSize() const {
    if (codec_ID != core::AlgoID::JBIG){
        // Adds the codec_ID, tile_nrows, tile_ncols
        return 1 + 4 + 4 + getPayloadSize();
    } else {
        // Only adds the codec_ID
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