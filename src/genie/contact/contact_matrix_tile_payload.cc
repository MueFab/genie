/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include "contact_matrix_tile_payload.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload::ContactMatrixTilePayload()
    : codec_ID_(core::AlgoID::JBIG), tile_nrows_(0),
      tile_ncols_(0),
      payload_()
{}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload::ContactMatrixTilePayload(
    const ContactMatrixTilePayload& other
)
    : codec_ID_(other.codec_ID_),
      tile_nrows_(other.tile_nrows_),
      tile_ncols_(other.tile_ncols_),
      payload_(other.payload_) {

    if (codec_ID_ == core::AlgoID::JBIG){
      tile_nrows_ = 0;
      tile_ncols_ = 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload::ContactMatrixTilePayload(
    ContactMatrixTilePayload&& other
) noexcept
    : codec_ID_(other.codec_ID_),
      tile_nrows_(other.tile_nrows_),
      tile_ncols_(other.tile_ncols_),
      payload_(std::move(other.payload_))
{

    if (codec_ID_ == core::AlgoID::JBIG){
      tile_nrows_ = 0;
      tile_ncols_ = 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload& ContactMatrixTilePayload::operator=(
    ContactMatrixTilePayload&& other
) noexcept
{
    if (this != &other) {
      codec_ID_ = other.codec_ID_;
      tile_nrows_ = other.tile_nrows_;
      tile_ncols_ = other.tile_ncols_;
      payload_ = std::move(other.payload_);
    }
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload::ContactMatrixTilePayload(util::BitReader &reader, size_t payload_size){
    UTILS_DIE_IF(!reader.IsByteAligned(), "Must be byte-aligned");

    codec_ID_ = reader.ReadAlignedInt<core::AlgoID>();
    payload_size -= sizeof(uint8_t);

    if (codec_ID_ == core::AlgoID::JBIG){
      tile_nrows_ = 0;
      tile_ncols_ = 0;
    } else {
      tile_nrows_ = reader.ReadAlignedInt<uint32_t>();
      tile_ncols_ = reader.ReadAlignedInt<uint32_t>();
        payload_size -= sizeof(uint32_t);
        payload_size -= sizeof(uint32_t);
    }

    payload_.resize(payload_size);
    for (auto &v: payload_){
        v = reader.ReadAlignedInt<uint8_t>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool ContactMatrixTilePayload::operator==(const ContactMatrixTilePayload& other) const {
    if (codec_ID_ != other.codec_ID_)
        return false;
    if (tile_nrows_ != other.tile_nrows_)
        return false;
    if (tile_ncols_ != other.tile_ncols_)
        return false;
    if (GetPayloadSize() != other.GetPayloadSize())
        return false;
    if (payload_ != other.payload_)
        return false;

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload::ContactMatrixTilePayload(
    core::AlgoID _codec_ID,
    uint32_t _tile_nrows,
    uint32_t _tile_ncols,
    std::vector<uint8_t>&& _payload
): codec_ID_(_codec_ID),
      tile_nrows_(_tile_nrows),
      tile_ncols_(_tile_ncols),
      payload_(std::move(_payload))
{
    if (codec_ID_ == core::AlgoID::JBIG){
      tile_nrows_ = 0;
      tile_ncols_ = 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixTilePayload::ContactMatrixTilePayload(
    core::AlgoID _codec_ID,
    uint32_t _tile_nrows,
    uint32_t _tile_ncols,
    uint8_t** _payload,
    size_t _payload_len
): codec_ID_(_codec_ID),
      tile_nrows_(_tile_nrows),
      tile_ncols_(_tile_ncols),
      payload_(*_payload, *(_payload) + _payload_len)
{
    free(*_payload);

    if (codec_ID_ == core::AlgoID::JBIG){
      tile_nrows_ = 0;
      tile_ncols_ = 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

core::AlgoID ContactMatrixTilePayload::GetCodecID() const { return codec_ID_; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixTilePayload::GetTileNRows() const { return tile_nrows_; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixTilePayload::GetTileNCols() const { return tile_ncols_; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint8_t>& ContactMatrixTilePayload::GetPayload() const { return payload_; }

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void ContactMatrixTilePayload::SetCodecID(core::AlgoID id) { codec_ID_ = id; }

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void ContactMatrixTilePayload::SetTileNRows(uint32_t rows) {
  tile_nrows_ = rows; }

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void ContactMatrixTilePayload::SetTileNCols(uint32_t cols) {
  tile_ncols_ = cols; }

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void ContactMatrixTilePayload::SetPayload(const std::vector<uint8_t>& data) {
  payload_ = data; }

// ---------------------------------------------------------------------------------------------------------------------

//void ContactMatrixTilePayload::setJBIGPayload(
//    uint8_t** _payload,
//    size_t _payload_len
//) {
//    payload_ = std::move(*_payload, )
//    codec_ID_ = core::AlgoID::JBIG;
//
//    free(*_payload);
//
//    if (codec_ID_ == core::AlgoID::JBIG){
//        tile_nrows_ = 0;
//        tile_ncols_ = 0;
//    }
//}

// ---------------------------------------------------------------------------------------------------------------------

size_t ContactMatrixTilePayload::GetPayloadSize() const { return payload_.size(); }

// ---------------------------------------------------------------------------------------------------------------------

size_t ContactMatrixTilePayload::GetSize() const {
    size_t size = 0;
    size += sizeof(uint8_t); // codec_ID_

    if (codec_ID_ != core::AlgoID::JBIG){
        // Adds tile_nrows_, tile_ncols_, and payload_size
        size += sizeof(uint32_t); // tile_nrows_
        size += sizeof(uint32_t); // tile_ncols_
    }

    size += GetPayloadSize();
    return size;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixTilePayload::Write(util::BitWriter &writer) const {
    writer.WriteBypassBE(codec_ID_);

    if (codec_ID_ != core::AlgoID::JBIG){
        writer.WriteBypassBE(tile_nrows_);
        writer.WriteBypassBE(tile_ncols_);
    }

    //writer.WriteBypassBE(static_cast<uint32_t>(GetPayloadSize()));
    for (auto v: payload_) writer.WriteBypassBE<uint8_t>(v);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------