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

ContactMatrixTilePayload::ContactMatrixTilePayload()
    : codec_ID(core::AlgoID::JBIG),
      tile_nrows(0),
      tile_ncols(0),
      payload()
{}

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

ContactMatrixTilePayload::ContactMatrixTilePayload(ContactMatrixTilePayload&& other) noexcept
    : codec_ID(other.codec_ID),
      tile_nrows(other.tile_nrows),
      tile_ncols(other.tile_ncols),
      payload(std::move(other.payload)) {}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload& ContactMatrixTilePayload::operator=(ContactMatrixTilePayload&& other) noexcept {
    if (this != &other) {
        codec_ID = other.codec_ID;
        tile_nrows = other.tile_nrows;
        tile_ncols = other.tile_ncols;
        payload = std::move(other.payload);
    }
    return *this;
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

void ContactMatrixTilePayload::write(util::BitWriter &writer) const {
    writer.writeBypassBE(codec_ID);

    if (codec_ID != core::AlgoID::JBIG){
        writer.writeBypassBE(tile_nrows);
        writer.writeBypassBE(tile_ncols);
    }

    for (auto v: payload)
        writer.writeBypassBE(v);
}


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------