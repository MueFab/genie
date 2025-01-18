/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 *
 * @brief Implements the Decoder class for decoding tokenized genomic record
 * names.
 *
 * This file is part of the Genie project, designed to handle efficient genomic
 * data compression and decompression. The `decoder.cpp` file focuses on
 * reconstructing original record names from tokenized and encoded sequences.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/name/tokenizer/decoder.h"

#include <string>
#include <tuple>
#include <vector>

#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::name::tokenizer {

// -----------------------------------------------------------------------------

std::string Decoder::inflate(const std::vector<SingleToken>& rec) {
  std::string ret;
  for (const auto& st : rec) {
    switch (st.token) {
      case Tokens::DUP:
      case Tokens::DELTA0:
      case Tokens::DELTA:
      case Tokens::MATCH:
        UTILS_DIE(
            "DUP. DELTA0, DELTA, MATCH should be patched before "
            "inflation");
      case Tokens::DIFF:
        UTILS_DIE_IF(st.param > 1, "Only DIFF <= 1 supported");
        break;
      case Tokens::STRING:
        ret += st.param_string;
        break;
      case Tokens::CHAR:
        ret += static_cast<char>(st.param);
        break;
      case Tokens::DIGITS:
        ret += std::to_string(st.param);
        break;
      case Tokens::DIGITS0:
      case Tokens::DZLEN:
        UTILS_DIE("DIGITS0, DZLEN not supported");
      case Tokens::END:
        break;
      default:
        UTILS_DIE("Invalid token");
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------

uint32_t Pull32BigEndian(core::AccessUnit::Subsequence& seq) {
  uint32_t ret = 0;
  ret |= seq.Pull() << 24;
  ret |= seq.Pull() << 16;
  ret |= seq.Pull() << 8;
  ret |= seq.Pull();
  return ret;
}

// -----------------------------------------------------------------------------

std::tuple<std::vector<std::string>, core::stats::PerfStats> Decoder::Process(
    core::AccessUnit::Descriptor& desc) {
  std::tuple<std::vector<std::string>, core::stats::PerfStats> ret;
  std::vector<SingleToken> old_rec;
  const util::Watch watch;
  while (!desc.GetTokenType(0, kTypeSeq).end()) {
    uint16_t cur_pos = 0;

    std::vector<SingleToken> rec;

    if (std::get<0>(ret).empty()) {
      UTILS_DIE_IF(static_cast<Tokens>(desc.GetTokenType(0, kTypeSeq).Get()) !=
                       Tokens::DIFF,
                   "First token in AU must be DIFF");
      UTILS_DIE_IF(
          desc.GetTokenType(0, static_cast<uint8_t>(Tokens::DIFF)).Get() != 0,
          "First DIFF in AU must be 0");
    }

    auto type =
        static_cast<Tokens>(desc.GetTokenType(cur_pos, kTypeSeq).Pull());

    while (type != Tokens::END) {
      SingleToken t(type, 0, "");

      if (type == Tokens::STRING) {
        auto c = static_cast<char>(
            desc.GetTokenType(cur_pos, static_cast<uint8_t>(Tokens::STRING))
                .Pull());
        while (c != '\0') {
          t.param_string += c;
          c = static_cast<char>(
              desc.GetTokenType(cur_pos, static_cast<uint8_t>(Tokens::STRING))
                  .Pull());
        }
      } else if (type == Tokens::DIGITS || type == Tokens::DIGITS0) {
        t.param = Pull32BigEndian(
            desc.GetTokenType(cur_pos, static_cast<uint8_t>(type)));
      } else if (static_cast<uint8_t>(type) <
                 static_cast<uint8_t>(Tokens::MATCH)) {
        t.param = static_cast<uint32_t>(
            desc.GetTokenType(cur_pos, static_cast<uint8_t>(type)).Pull());
      }

      rec.push_back(t);

      cur_pos++;
      type = static_cast<Tokens>(desc.GetTokenType(cur_pos, kTypeSeq).Pull());
    }

    rec = patch(old_rec, rec);
    old_rec = rec;

    std::get<0>(ret).emplace_back(inflate(rec));
  }
  std::get<1>(ret).AddDouble("time-nametokenize", watch.Check());
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::name::tokenizer

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
