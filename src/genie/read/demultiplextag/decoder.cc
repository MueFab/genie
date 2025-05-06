/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 *
 * @brief Implementation of the tag decoder for the Genie framework.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/demultiplextag/decoder.h"

#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::demultiplex_tag {

// -----------------------------------------------------------------------------

Decoder::Decoder(core::AccessUnit::Descriptor&& desc) : desc_(desc) {}

// -----------------------------------------------------------------------------

core::record::TagRecord Decoder::DecodeTags() {
  core::record::TagRecord record;
  const size_t num_tags = desc_.Get(core::gen_sub::kTagsNum.second).Pull();

  for (size_t i = 0; i < num_tags; ++i) {
    const auto type = static_cast<core::record::TagDataType>(
        desc_.Get(core::gen_sub::kTagsType.second).Pull());
    const uint16_t raw_key = desc_.Get(core::gen_sub::kTagsKey.second).Pull();
    const size_t length = desc_.Get(core::gen_sub::kTagsLength.second).Pull();

    std::array<char, 2> key = {static_cast<char>(raw_key >> 8),
                               static_cast<char>(raw_key & 0xFF)};

    core::record::TagData value;

    switch (type) {
      case core::record::TagDataType::kInt32: {  // int32_t
        if (length == 1) {
          value = static_cast<int32_t>(
              desc_.Get(core::gen_sub::kTagsInt32.second).Pull());
        } else {
          std::vector<int32_t> vec(length);
          for (auto& x : vec)
            x = static_cast<int32_t>(
                desc_.Get(core::gen_sub::kTagsInt32.second).Pull());
          value = std::move(vec);
        }
        break;
      }
      case core::record::TagDataType::kChar: {  // string or char
        if (length == 1) {
          value = static_cast<char>(
              desc_.Get(core::gen_sub::kTagsChar.second).Pull());
        } else {
          std::string str;
          str.reserve(length);
          for (size_t j = 0; j < length; ++j)
            str += static_cast<char>(
                desc_.Get(core::gen_sub::kTagsChar.second).Pull());
          value = std::move(str);
        }
        break;
      }
      case core::record::TagDataType::kUInt8: {
        if (length == 1) {
          value = static_cast<uint8_t>(
              desc_.Get(core::gen_sub::kTagsUInt8.second).Pull());
        } else {
          std::vector<uint8_t> vec(length);
          for (auto& x : vec)
            x = static_cast<uint8_t>(
                desc_.Get(core::gen_sub::kTagsUInt8.second).Pull());
          value = std::move(vec);
        }
        break;
      }
      case core::record::TagDataType::kInt8: {
        if (length == 1) {
          value = static_cast<int8_t>(
              desc_.Get(core::gen_sub::kTagsInt8.second).Pull());
        } else {
          std::vector<int8_t> vec(length);
          for (auto& x : vec)
            x = static_cast<int8_t>(
                desc_.Get(core::gen_sub::kTagsInt8.second).Pull());
          value = std::move(vec);
        }
        break;
      }
      case core::record::TagDataType::kUInt16: {
        if (length == 1) {
          value = static_cast<uint16_t>(
              desc_.Get(core::gen_sub::kTagsUInt16.second).Pull());
        } else {
          std::vector<uint16_t> vec(length);
          for (auto& x : vec)
            x = static_cast<uint16_t>(
                desc_.Get(core::gen_sub::kTagsUInt16.second).Pull());
          value = std::move(vec);
        }
        break;
      }
      case core::record::TagDataType::kInt16: {
        if (length == 1) {
          value = static_cast<int16_t>(
              desc_.Get(core::gen_sub::kTagsInt16.second).Pull());
        } else {
          std::vector<int16_t> vec(length);
          for (auto& x : vec)
            x = static_cast<int16_t>(
                desc_.Get(core::gen_sub::kTagsInt16.second).Pull());
          value = std::move(vec);
        }
        break;
      }
      case core::record::TagDataType::kUInt32: {
        if (length == 1) {
          value = static_cast<uint32_t>(
              desc_.Get(core::gen_sub::kTagsUInt32.second).Pull());
        } else {
          std::vector<uint32_t> vec(length);
          for (auto& x : vec)
            x = static_cast<uint32_t>(
                desc_.Get(core::gen_sub::kTagsUInt32.second).Pull());
          value = std::move(vec);
        }
        break;
      }
      case core::record::TagDataType::kHexChar: {  // HexString
        std::string hex;
        hex.reserve(length);
        for (size_t j = 0; j < length; ++j)
          hex += static_cast<char>(
              desc_.Get(core::gen_sub::kTagsChar.second).Pull());
        value = util::HexString{hex};
        break;
      }
      case core::record::TagDataType::kFloat:
        if (length == 1) {
          value = static_cast<float>(
              desc_.Get(core::gen_sub::kTagsFloat32.second).Pull());
        } else {
          std::vector<float> vec(length);
          for (auto& x : vec)
            x = static_cast<float>(
                desc_.Get(core::gen_sub::kTagsFloat32.second).Pull());
          value = std::move(vec);
        }
        break;
      case core::record::TagDataType::kDouble: {  // float or double
        if (length == 1) {
          value = static_cast<double>(
              desc_.Get(core::gen_sub::kTagsFloat64.second).Pull());
        } else {
          std::vector<double> vec(length);
          for (auto& x : vec)
            x = static_cast<double>(
                desc_.Get(core::gen_sub::kTagsFloat32.second).Pull());
          value = std::move(vec);
        }
        break;
      }
      default:
        UTILS_DIE("Unsupported tag type index: " +
                  std::to_string(static_cast<uint8_t>(type)));
    }

    record.Set(core::record::Tag{key, std::move(value)});
  }

  return record;
}

// -----------------------------------------------------------------------------

}  // namespace genie::demultiplex_tag

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
