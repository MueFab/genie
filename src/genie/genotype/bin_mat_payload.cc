/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

// ---------------------------------------------------------------------------------------------------------------------

#include "bin_mat_payload.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/jbig/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::genotype {

// ---------------------------------------------------------------------------------------------------------------------

BinMatPayload::BinMatPayload()
    : codec_ID_(core::AlgoID::UNDEFINED),
      payload_(),
      nrows_(0),
      ncols_(0) {}
//      compressed_payload_() {}

// ---------------------------------------------------------------------------------------------------------------------

BinMatPayload::BinMatPayload(
    core::AlgoID codec_id,
    std::vector<uint8_t>&& payload,
    uint32_t nrows,
    uint32_t ncols)
    : codec_ID_(codec_id),
      payload_(std::move(payload)),
      nrows_(nrows),
      ncols_(ncols) {}
//      compressed_payload_() {}

// ---------------------------------------------------------------------------------------------------------------------

BinMatPayload::BinMatPayload(const BinMatPayload& other) = default;

// ---------------------------------------------------------------------------------------------------------------------

BinMatPayload::BinMatPayload(BinMatPayload&& other) noexcept
    : codec_ID_(other.codec_ID_),
      payload_(std::move(other.payload_)),
      nrows_(other.nrows_),
      ncols_(other.ncols_) {}
//      compressed_payload_(std::move(other.compressed_payload_)) {}

// -----------------------------------------------------------------------------

BinMatPayload::BinMatPayload(
    util::BitReader& reader,
    size_t payload_size,
    core::AlgoID codec_ID)
    : codec_ID_(codec_ID){

    UTILS_DIE_IF(!reader.IsByteAligned(), "reader must be byte aligned!");

    if (codec_ID_ == genie::core::AlgoID::JBIG){
      nrows_ = 0;
      ncols_ = 0;
      payload_.resize(payload_size);
      reader.ReadAlignedBytes(payload_.data(), payload_size);
    } else {
      nrows_ = reader.Read<uint32_t>();
      ncols_ = reader.Read<uint32_t>();
      payload_size -= sizeof(uint32_t) + sizeof(uint32_t);
      payload_.resize(payload_size);
      reader.ReadAlignedBytes(payload_.data(), payload_size);
    }

//    switch (codec_ID_) {
//        case genie::core::AlgoID::JBIG: {
//          //TODO: First 4 bytes are width, 2nd 4 bytes are heights
//          nrows_ = 0;
//          ncols_ = 0;
//          payload_.resize(payload_size);
//          reader.ReadAlignedBytes(payload_.data(), payload_size);
//        } break;
//        case genie::core::AlgoID::ZSTD: {
//          nrows_ = reader.Read<uint32_t>();
//          ncols_ = reader.Read<uint32_t>();
//          payload_size -= sizeof(uint32_t) + sizeof(uint32_t);
//          payload_.resize(payload_size);
//          reader.ReadAlignedBytes(payload_.data(), payload_size);
//        } break;
//        case genie::core::AlgoID::BSC: {
//          nrows_ = reader.Read<uint32_t>();
//          ncols_ = reader.Read<uint32_t>();
//          payload_size -= sizeof(uint32_t) + sizeof(uint32_t);
//          payload_.resize(payload_size);
//          reader.ReadAlignedBytes(payload_.data(), payload_size);
//        } break;
//        case genie::core::AlgoID::LZMA: {
//          nrows_ = reader.Read<uint32_t>();
//          ncols_ = reader.Read<uint32_t>();
//          payload_size -= sizeof(uint32_t) + sizeof(uint32_t);
//          payload_.resize(payload_size);
//          reader.ReadAlignedBytes(payload_.data(), payload_size);
//        } break;
//        default:
//          UTILS_DIE("Invalid codec_ID");
//          break;
//    }
}

// ---------------------------------------------------------------------------------------------------------------------

BinMatPayload& BinMatPayload::operator=(const BinMatPayload& other) {
  if (this != &other) {
    codec_ID_ = other.codec_ID_;
    payload_ = other.payload_;
    nrows_ = other.nrows_;
    ncols_ = other.ncols_;
//    compressed_payload_ = other.compressed_payload_;
  }
  return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

BinMatPayload& BinMatPayload::operator=(BinMatPayload&& other) noexcept {
  if (this != &other) {
    codec_ID_ = other.codec_ID_;
    payload_ = std::move(other.payload_);
    nrows_ = other.nrows_;
    ncols_ = other.ncols_;
//    compressed_payload_ = std::move(other.compressed_payload_);
  }
  return *this;
}

// ---------------------------------------------------------------------------------------------------------------------
bool BinMatPayload::operator==(const BinMatPayload& other) const {
  if (codec_ID_ != other.codec_ID_){
    return false;
  }

  if (codec_ID_ != core::AlgoID::JBIG){
      if (nrows_ != other.nrows_ || ncols_ != other.ncols_) {
          return false;
      }
  }

  return payload_ == other.payload_;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] core::AlgoID BinMatPayload::GetCodecID() const {
  return codec_ID_;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] const std::vector<uint8_t>& BinMatPayload::GetPayload() const {
  return payload_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t BinMatPayload::GetNRows() const {
  if (codec_ID_ == core::AlgoID::JBIG) {
    // Check if the payload has enough data to extract the height
    if (payload_.size() < 12) {
      return 0;
    }

    // Extract height from bytes 8-11 (little-endian)
    uint32_t height = 0;
    height |= static_cast<uint32_t>(payload_[8]) << 24;
    height |= static_cast<uint32_t>(payload_[9]) << 16;
    height |= static_cast<uint32_t>(payload_[10]) << 8;
    height |= payload_[11];

    return height;
  } else {
    return nrows_;
  }
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t BinMatPayload::GetNCols() const {
  if (codec_ID_ == core::AlgoID::JBIG) {
    // Check if the payload has enough data to extract the width
    if (payload_.size() < 8) {
      return 0;
    }

    // Extract width from bytes 4-7 (little-endian)
    uint32_t width = 0;
    width |= static_cast<uint32_t>(payload_[4]) << 24;
    width |= static_cast<uint32_t>(payload_[5]) << 16;
    width |= static_cast<uint32_t>(payload_[6]) << 8;
    width |= payload_[7];

    return width;
  } else {
    return ncols_;
  }
}

// ---------------------------------------------------------------------------------------------------------------------

//[[maybe_unused]] const std::vector<uint8_t>& BinMatPayload::GetCompressedPayload() const {
//  return compressed_payload_;
//}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void BinMatPayload::SetCodecID(core::AlgoID codec_id) {
  codec_ID_ = codec_id;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void BinMatPayload::SetPayload(std::vector<uint8_t>&& payload) {
  payload_ = std::move(payload);
}

// ---------------------------------------------------------------------------------------------------------------------

//[[maybe_unused]] void BinMatPayload::SetNRows(uint32_t nrows) {
//  nrows_ = nrows;
//}

// ---------------------------------------------------------------------------------------------------------------------

//[[maybe_unused]] void BinMatPayload::SetNCols(uint32_t ncols) {
//  ncols_ = ncols;
//}

// ---------------------------------------------------------------------------------------------------------------------

//[[maybe_unused]] void BinMatPayload::SetCompressedPayload(std::vector<uint8_t>&& compressed_payload) {
//  compressed_payload_ = std::move(compressed_payload);
//}

size_t BinMatPayload::GetPayloadSize() const { return payload_.size(); }

// ---------------------------------------------------------------------------------------------------------------------

size_t BinMatPayload::GetSize() const {
  size_t size = 0u;
  if (codec_ID_ != genie::core::AlgoID::JBIG){
    size += sizeof(uint32_t); // u(32)
    size += sizeof(uint32_t); // u(32)
  }

  size += GetPayloadSize();

  return size;
}

// ---------------------------------------------------------------------------------------------------------------------

void BinMatPayload::Write(core::Writer& writer) const {
  if (codec_ID_ != genie::core::AlgoID::JBIG) {
    writer.Write(GetNRows(), 32);
    writer.Write(GetNCols(), 32);
  }
  for (auto byte : payload_) writer.Write(byte, 8);
}

// -----------------------------------------------------------------------------

void BinMatPayload::Write(util::BitWriter writer) const {
  if (codec_ID_ != genie::core::AlgoID::JBIG){
    writer.WriteBypassBE(GetNRows());
    writer.WriteBypassBE(GetNCols());
  }
  writer.WriteAlignedBytes(payload_.data(), GetPayloadSize());
}

// ---------------------------------------------------------------------------------------------------------------------

//TODO: Move to somewhere else
//void BinMatPayload::WriteCompressed(core::Writer& writer) const {
//  std::stringstream payloadStream;
//  for (auto byte : payload_) payloadStream.write((char*)&byte, 1);
//
//  std::stringstream compressedStream;
//  if (codec_ID_ != genie::core::AlgoID::JBIG) {
//    writer.Write(GetNRows(), 32);
//    writer.Write(GetNCols(), 32);
//  }
//
//  switch (codec_ID_) {
//    case genie::core::AlgoID::LZMA: {
//      genie::entropy::lzma::LZMAEncoder encoder;
//      encoder.encode(payloadStream, compressedStream);
//      writer.Write(&compressedStream);
//    } break;
//    case genie::core::AlgoID::ZSTD: {
//      genie::entropy::zstd::ZSTDEncoder encoder;
//      encoder.encode(payloadStream, compressedStream);
//      writer.Write(&compressedStream);
//    } break;
//    case genie::core::AlgoID::BSC: {
//      genie::entropy::bsc::BSCEncoder encoder;
//      encoder.encode(payloadStream, compressedStream);
//      writer.Write(&compressedStream);
//    } break;
//    case genie::core::AlgoID::JBIG: {
//      genie::entropy::jbig::JBIGEncoder encoder;
//      encoder.encode(payloadStream, compressedStream, GetNCols(), GetNRows());
//      writer.Write(&compressedStream);
//    } break;
//    default:
//      writer.Write(&payloadStream);
//      break;
//  }
//}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::genotype

// ---------------------------------------------------------------------------------------------------------------------