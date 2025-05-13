/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 *
 * @brief Implementation of the tag encoder for the Genie framework.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/demultiplextag/encoder.h"

#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::demultiplex_tag {

// -----------------------------------------------------------------------------

Encoder::Encoder() : desc_(core::GenDesc::kTag) {
  for (const auto& s :
       genie::core::GetDescriptor(core::GenDesc::kTag).sub_seqs) {
    const size_t w_size =
        s.range.second == -1
            ? 8
            : std::ceil(std::log2(static_cast<double>(s.range.second)) / 8);
    size_t realsize = 1;
    while (realsize < w_size) realsize <<= 1;
    desc_.Add(core::AccessUnit::Subsequence(realsize, s.id));
  }
}

// -----------------------------------------------------------------------------

uint16_t Encoder::EncodeTagKey(const std::array<char, 2>& key) {
  if (!std::isalnum(static_cast<unsigned char>(key[0])) ||
      !std::isalnum(static_cast<unsigned char>(key[1]))) {
    throw std::invalid_argument(
        "Tag key must contain alphanumeric ASCII characters.");
  }

  return static_cast<uint8_t>(key[0]) << 8 | static_cast<uint8_t>(key[1]);
}

// -----------------------------------------------------------------------------

void Encoder::EncodeTags(const core::record::TagRecord& tags) {
  desc_.Get(core::gen_sub::kTagsNum.second).Push(tags.All().size());

  for (const auto& tag : tags.All()) {
    const auto type = static_cast<uint8_t>(
        std::visit(core::record::TagDataTypeVisitor{}, tag.value()));
    desc_.Get(core::gen_sub::kTagsType.second).Push(type);
    desc_.Get(core::gen_sub::kTagsKey.second).Push(EncodeTagKey(tag.key()));
    desc_.Get(core::gen_sub::kTagsLength.second)
        .Push(std::visit(core::record::TagDataLengthVisitor{}, tag.value()));

    std::visit(
        [&](const auto& val) {
          using T = std::decay_t<decltype(val)>;

          // Scalars
          if constexpr (std::is_same_v<T, int32_t>)
            desc_.Get(core::gen_sub::kTagsInt32.second)
                .Push(static_cast<uint64_t>(val));
          else if constexpr (std::is_same_v<T, std::string>)
            for (const char c : val)
              desc_.Get(core::gen_sub::kTagsChar.second).Push(c);
          else if constexpr (std::is_same_v<T, uint8_t>)
            desc_.Get(core::gen_sub::kTagsUInt8.second)
                .Push(static_cast<uint64_t>(val));
          else if constexpr (std::is_same_v<T, int8_t>)
            desc_.Get(core::gen_sub::kTagsInt8.second)
                .Push(static_cast<uint64_t>(val));
          else if constexpr (std::is_same_v<T, uint16_t>)
            desc_.Get(core::gen_sub::kTagsUInt16.second)
                .Push(static_cast<uint64_t>(val));
          else if constexpr (std::is_same_v<T, int16_t>)
            desc_.Get(core::gen_sub::kTagsInt16.second)
                .Push(static_cast<uint64_t>(val));
          else if constexpr (std::is_same_v<T, uint32_t>)
            desc_.Get(core::gen_sub::kTagsUInt32.second)
                .Push(static_cast<uint64_t>(val));
          else if constexpr (std::is_same_v<T, util::HexString>)
            for (const char c : val.str())
              desc_.Get(core::gen_sub::kTagsChar.second)
                  .Push(static_cast<uint64_t>(c));
          else if constexpr (std::is_same_v<T, float>) {
            const float tmp = val;
            uint32_t tmp2 = 0;
            std::memcpy(&tmp2, &tmp, sizeof(float));
            desc_.Get(core::gen_sub::kTagsFloat32.second).Push(tmp2);
          } else if constexpr (std::is_same_v<T, double>) {
            const double tmp = val;
            uint64_t tmp2 = 0;
            std::memcpy(&tmp2, &tmp, sizeof(double));
            desc_.Get(core::gen_sub::kTagsFloat64.second).Push(tmp2);
          } else if constexpr (std::is_same_v<T, std::vector<int32_t>>) {
            for (auto x : val)
              desc_.Get(core::gen_sub::kTagsInt32.second)
                  .Push(static_cast<uint64_t>(x));
          } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
            for (auto x : val)
              desc_.Get(core::gen_sub::kTagsUInt8.second)
                  .Push(static_cast<uint64_t>(x));
          } else if constexpr (std::is_same_v<T, std::vector<int8_t>>) {
            for (auto x : val)
              desc_.Get(core::gen_sub::kTagsInt8.second)
                  .Push(static_cast<uint64_t>(x));
          } else if constexpr (std::is_same_v<T, std::vector<uint16_t>>) {
            for (auto x : val)
              desc_.Get(core::gen_sub::kTagsUInt16.second)
                  .Push(static_cast<uint64_t>(x));
          } else if constexpr (std::is_same_v<T, std::vector<int16_t>>) {
            for (auto x : val)
              desc_.Get(core::gen_sub::kTagsInt16.second)
                  .Push(static_cast<uint64_t>(x));
          } else if constexpr (std::is_same_v<T, std::vector<uint32_t>>) {
            for (auto x : val) {
              desc_.Get(core::gen_sub::kTagsUInt32.second)
                  .Push(static_cast<uint64_t>(x));
            }
          } else if constexpr (std::is_same_v<T, std::vector<float>>) {
            for (auto x : val) {
              const float tmp = x;
              uint32_t tmp2 = 0;
              std::memcpy(&tmp2, &tmp, sizeof(float));
              desc_.Get(core::gen_sub::kTagsFloat32.second).Push(tmp2);
            }
          } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (auto x : val) {
              const double tmp = x;
              uint64_t tmp2 = 0;
              std::memcpy(&tmp2, &tmp, sizeof(double));
              desc_.Get(core::gen_sub::kTagsFloat64.second).Push(tmp2);
            }
          } else {
            static_assert(!sizeof(T), "Unsupported TagType in serialization");
          }
        },
        tag.value());
  }
}

// -----------------------------------------------------------------------------

core::AccessUnit::Descriptor&& Encoder::MoveDescriptor() {
  return std::move(desc_);
}

// -----------------------------------------------------------------------------

}  // namespace genie::demultiplex_tag

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
