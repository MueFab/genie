/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include "contact_matrix_tile_payload.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::contact {

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
    core::AlgoID codec_ID,
    uint32_t tile_nrows,
    uint32_t tile_ncols,
    uint8_t** payload,
    size_t payload_len
): codec_ID_(codec_ID),
   tile_nrows_(tile_nrows),
   tile_ncols_(tile_ncols),
   payload_(*payload, *(payload) + payload_len)
{
    free(*payload);

    if (codec_ID == core::AlgoID::JBIG){
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

//std::pair<uint32_t, uint32_t> ContactMatrixTilePayload::GetTileDimensions() const {
//  if (codec_ID_ != core::AlgoID::JBIG) {
//    return {tile_nrows_, tile_ncols_};
//  }
//
//  // JBIG payload contains BIE (Binary Image Data) segments
//  // We need to parse the BIE segment to get the image dimensions
//  const uint8_t* payload_ptr = payload_.data();
//  size_t payload_size = payload_.size();
//
//  // Skip SOI (Start of Image) marker
//  if (payload_size < 2 || payload_ptr[0] != 0xFF || payload_ptr[1] != 0xD8) {
////    throw util::RuntimeException("Invalid JBIG payload - missing SOI marker");
//    UTILS_DIE("Invalid JBIG payload - missing SOI marker");
//  }
//  payload_ptr += 2;
//  payload_size -= 2;
//
//  // Skip BTH (Binary Thumbnail) segment if present
//  while (payload_size >= 2 && payload_ptr[0] == 0xFF) {
//    uint8_t marker = payload_ptr[1];
//    if (marker == 0xD9) {  // EOI marker
//      break;
//    }
//    if (marker == 0x02) {  // BTH marker
//      // Skip BTH segment
//      payload_ptr += 2;
//      if (payload_size < 4) {
//        UTILS_DIE("Invalid JBIG payload - incomplete BTH segment");
//      }
//      uint16_t bth_length = (payload_ptr[0] << 8) | payload_ptr[1];
//      payload_ptr += bth_length;
//      payload_size -= bth_length + 2;
//    } else {
//      // Skip other markers
//      payload_ptr += 2;
//      payload_size -= 2;
//    }
//  }
//
//  // Now we're at the BIE (Binary Image Data) segment
//  if (payload_size < 8) {
//    UTILS_DIE("Invalid JBIG payload - incomplete BTH segment");
//  }
//
//  // BIE segment format:
//  // - 1 byte: Image width (lsb)
//  // - 1 byte: Image width (msb)
//  // - 1 byte: Image height (lsb)
//  // - 1 byte: Image height (msb)
//  // - 4 bytes: Image data (not needed for dimensions)
//
//  uint32_t width = (payload_ptr[1] << 8) | payload_ptr[0];
//  uint32_t height = (payload_ptr[3] << 8) | payload_ptr[2];
//
//  return {height, width};
//}

// ---------------------------------------------------------------------------------------------------------------------

size_t ContactMatrixTilePayload::GetPayloadSize() const { return payload_.size(); }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixTilePayload::GetNumRows() {
  if (codec_ID_ == core::AlgoID::JBIG) {
    // Check if the payload has enough data to extract the height
    if (payload_.size() < 12) {
      return 0;
    }

    // Extract height from bytes 8-11 (little-endian)
    uint32_t height = 0;
    height |= static_cast<uint32_t>(payload_[8] << 24);
    height |= static_cast<uint32_t>(payload_[9] << 16);
    height |= static_cast<uint32_t>(payload_[10] << 8);
    height |= payload_[11];

    return height;
  } else {
    return tile_nrows_;
  }
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixTilePayload::GetNumCols() {
  if (codec_ID_ == core::AlgoID::JBIG) {
    // Check if the payload has enough data to extract the width
    if (payload_.size() < 8) {
      return 0;
    }

    // Extract width from bytes 4-7 (little-endian)
    uint32_t width = 0;
    width |= static_cast<uint32_t>(payload_[4] << 24);
    width |= static_cast<uint32_t>(payload_[5] << 16);
    width |= static_cast<uint32_t>(payload_[6] << 8);
    width |= payload_[7];

    return width;
  } else {
    return tile_ncols_;
  }
}

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

}  // namespace genie::contact

// ---------------------------------------------------------------------------------------------------------------------