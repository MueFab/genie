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
    const ContactMatrixTilePayload& other
)
    : codec_ID(other.codec_ID),
      tile_nrows(other.tile_nrows),
      tile_ncols(other.tile_ncols),
      payload(other.payload) {

    if (codec_ID == core::AlgoID::JBIG){
        tile_nrows = 0;
        tile_ncols = 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload::ContactMatrixTilePayload(
    ContactMatrixTilePayload&& other
) noexcept
    : codec_ID(other.codec_ID),
      tile_nrows(other.tile_nrows),
      tile_ncols(other.tile_ncols),
      payload(std::move(other.payload))
{

    if (codec_ID == core::AlgoID::JBIG){
        tile_nrows = 0;
        tile_ncols = 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload& ContactMatrixTilePayload::operator=(
    ContactMatrixTilePayload&& other
) noexcept
{
    if (this != &other) {
        codec_ID = other.codec_ID;
        tile_nrows = other.tile_nrows;
        tile_ncols = other.tile_ncols;
        payload = std::move(other.payload);
    }
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload::ContactMatrixTilePayload(util::BitReader &reader) noexcept{
    codec_ID = reader.readBypassBE<core::AlgoID>();
    if (codec_ID == core::AlgoID::JBIG){
        tile_nrows = 0;
        tile_ncols = 0;
    } else {
        tile_nrows = reader.readBypassBE<uint32_t>();
        tile_ncols = reader.readBypassBE<uint32_t>();
    }
    size_t payload_size = reader.readBypassBE<uint32_t>();
    payload.resize(payload_size);
    for (auto &v: payload){
        v = reader.readBypassBE<uint8_t>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool ContactMatrixTilePayload::operator==(const ContactMatrixTilePayload& other) const {
    if (codec_ID != other.codec_ID)
        return false;
    if (tile_nrows != other.tile_nrows)
        return false;
    if (tile_ncols != other.tile_ncols)
        return false;
    if (getPayloadSize() != other.getPayloadSize())
        return false;
    if (payload != other.payload)
        return false;

    return true;
}

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
{
    if (codec_ID == core::AlgoID::JBIG){
        tile_nrows = 0;
        tile_ncols = 0;
    }
}

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

    if (codec_ID == core::AlgoID::JBIG){
        tile_nrows = 0;
        tile_ncols = 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

core::AlgoID ContactMatrixTilePayload::getCodecID() const { return codec_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixTilePayload::getTileNRows() const { return tile_nrows; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixTilePayload::getTileNCols() const { return tile_ncols; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint8_t>& ContactMatrixTilePayload::getPayload() const { return payload; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixTilePayload::setCodecID(core::AlgoID id) { codec_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixTilePayload::setTileNRows(uint32_t rows) { tile_nrows = rows; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixTilePayload::setTileNCols(uint32_t cols) { tile_ncols = cols; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixTilePayload::setPayload(const std::vector<uint8_t>& data) {payload = data; }

// ---------------------------------------------------------------------------------------------------------------------

//void ContactMatrixTilePayload::setJBIGPayload(
//    uint8_t** _payload,
//    size_t _payload_len
//) {
//    payload = std::move(*_payload, )
//    codec_ID = core::AlgoID::JBIG;
//
//    free(*_payload);
//
//    if (codec_ID == core::AlgoID::JBIG){
//        tile_nrows = 0;
//        tile_ncols = 0;
//    }
//}

// ---------------------------------------------------------------------------------------------------------------------

size_t ContactMatrixTilePayload::getPayloadSize() const { return payload.size(); }

// ---------------------------------------------------------------------------------------------------------------------

size_t ContactMatrixTilePayload::getSize() const {
    if (codec_ID != core::AlgoID::JBIG){
        // Adds the codec_ID, tile_nrows, tile_ncols, and payload_size
        return 1 + 4 + 4 + 4+ getPayloadSize();
    } else {
        // Adds the codec_ID and payload size
        return 1 + 4 + getPayloadSize();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixTilePayload::write(util::BitWriter &writer) const {
    writer.writeBypassBE(codec_ID);

    if (codec_ID != core::AlgoID::JBIG){
        writer.writeBypassBE(tile_nrows);
        writer.writeBypassBE(tile_ncols);
    }

    writer.writeBypassBE(static_cast<uint32_t>(getPayloadSize()));
    for (auto v: payload)
        writer.writeBypassBE<uint8_t>(v);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------