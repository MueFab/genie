/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/sam/sam_tag.h"

#include <string>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::format::sam {

// -----------------------------------------------------------------------------

std::string SerializeTagRecord(const core::record::TagRecord& record) {
  std::ostringstream out;
  bool first = true;

  for (const core::record::Tag& tag : record.All()) {
    if (!first) out << '\t';
    first = false;

    const std::string key = {tag.key()[0], tag.key()[1]};
    out << key << ":";

    std::visit(
        [&](const auto& val) {
          using T = std::decay_t<decltype(val)>;

          auto emit_array = [&](const char code, const auto& vec) {
            using ElemT = std::decay_t<decltype(vec[0])>;
            out << "B:" << code;
            for (const auto& x : vec) {
              if constexpr (std::is_same_v<ElemT, int8_t> ||
                            std::is_same_v<ElemT, uint8_t>) {
                out << "," << static_cast<int>(x);
              } else {
                out << "," << x;
              }
            }
          };

          if constexpr (std::is_same_v<T, int32_t>) {
            out << "i:" << val;
          } else if constexpr (std::is_same_v<T, uint8_t>) {
            out << "C:" << static_cast<int>(val);
          } else if constexpr (std::is_same_v<T, int8_t>) {
            out << "c:" << static_cast<int>(val);
          } else if constexpr (std::is_same_v<T, uint16_t>) {
            out << "S:" << val;
          } else if constexpr (std::is_same_v<T, int16_t>) {
            out << "s:" << val;
          } else if constexpr (std::is_same_v<T, uint32_t>) {
            out << "I:" << val;
          } else if constexpr (std::is_same_v<T, float> ||  // NOLINT
                               std::is_same_v<T, double>) {
            out << "f:" << val;
          } else if constexpr (std::is_same_v<T, std::string>) {
            if (val.size() == 1 && std::isprint(val[0])) {
              out << "A:" << val;
            } else {
              out << "Z:" << val;
            }
          } else if constexpr (std::is_same_v<T, util::HexString>) {
            out << "H:" << val.str();
          } else if constexpr (std::is_same_v<T, std::vector<int8_t>>) {
            emit_array('c', val);
          } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
            emit_array('C', val);
          } else if constexpr (std::is_same_v<T, std::vector<int16_t>>) {
            emit_array('s', val);
          } else if constexpr (std::is_same_v<T, std::vector<uint16_t>>) {
            emit_array('S', val);
          } else if constexpr (std::is_same_v<T, std::vector<int32_t>>) {
            emit_array('i', val);
          } else if constexpr (std::is_same_v<T, std::vector<uint32_t>>) {
            emit_array('I', val);
          } else if constexpr (std::is_same_v<T, // NOLINT
                                              std::vector<float>> ||
                               std::is_same_v<T, std::vector<double>>) {
            emit_array('f', val);
          } else {
            static_assert(!sizeof(T), "Unsupported TagType in serialization");
          }
        },
        tag.value());
  }

  return out.str();
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
