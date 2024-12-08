/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/name/tokenizer/token.h"

#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::name::tokenizer {

// -----------------------------------------------------------------------------
SingleToken::SingleToken(const Tokens t, const uint32_t p, std::string ps)
    : token(t), param(p), param_string(std::move(ps)) {}

// -----------------------------------------------------------------------------
bool SingleToken::operator==(const SingleToken& t) const {
  return token == t.token && param == t.param && param_string == t.param_string;
}

// -----------------------------------------------------------------------------
bool SingleToken::operator!=(const SingleToken& t) const {
  return !(*this == t);
}

// -----------------------------------------------------------------------------
const TokenInfo& GetTokenInfo(Tokens t) {
  static auto info = []() -> std::vector<TokenInfo> {
    std::vector<TokenInfo> ret = {{"DUP", sizeof(uint32_t)},
                                  {"DIFF", sizeof(uint32_t)},
                                  {"STRING", sizeof(uint8_t)},
                                  {"CHAR", sizeof(uint8_t)},
                                  {"DIGITS", sizeof(uint32_t)},
                                  {"DELTA", sizeof(uint8_t)},
                                  {"DIGITS0", sizeof(uint32_t)},
                                  {"DELTA0", sizeof(uint8_t)},
                                  {"MATCH", 0},
                                  {"DZLEN", sizeof(uint8_t)},
                                  {"END", 0},
                                  {"NONE", 0}};
    return ret;
  }();
  return info[static_cast<uint8_t>(t)];
}

// -----------------------------------------------------------------------------
std::vector<SingleToken> patch(const std::vector<SingleToken>& old_string,
                               const std::vector<SingleToken>& new_string) {
  std::vector ret(new_string.size(), SingleToken(Tokens::DUP, 0, ""));
  if (new_string.front().token == Tokens::DUP) {
    UTILS_DIE_IF(new_string.front().param > 1, "DUP > 1 not supported");
    UTILS_DIE_IF(new_string.size() > 1, "Found DUP but more than one token");
    return old_string;
  }
  if (new_string.front().token != Tokens::DIFF) {
    UTILS_DIE("First token is neither DUP nor DIFF");
  }
  for (size_t i = 0; i < new_string.size(); ++i) {
    if (i >= old_string.size()) {
      UTILS_DIE_IF(new_string[i].token == Tokens::DELTA ||
                       new_string[i].token == Tokens::DELTA0 ||
                       new_string[i].token == Tokens::MATCH,
                   "Found MATCH/DELTA but no token to match");
      ret[i] = new_string[i];
      continue;
    }
    switch (new_string[i].token) {
      case Tokens::MATCH:
        UTILS_DIE_IF(old_string[i].token == Tokens::DUP ||
                         old_string[i].token == Tokens::DELTA ||
                         old_string[i].token == Tokens::DELTA0 ||
                         old_string[i].token == Tokens::MATCH ||
                         old_string[i].token == Tokens::MATCH,
                     "Found MATCH but no token to match");
        ret[i] = old_string[i];
        break;
      case Tokens::DELTA:
        UTILS_DIE_IF(old_string[i].token != Tokens::DIGITS,
                     "Found DELTA but no digits");
        ret[i] = SingleToken(Tokens::DIGITS,
                             old_string[i].param + new_string[i].param, "");
        break;
      case Tokens::DELTA0:
        UTILS_DIE_IF(old_string[i].token != Tokens::DIGITS0,
                     "Found DELTA0 but no digits");
        ret[i] = SingleToken(Tokens::DIGITS0,
                             old_string[i].param + new_string[i].param, "");
        break;
      case Tokens::DIFF:
      case Tokens::DUP:
        UTILS_DIE_IF(i != 0,
                     "Found DIFF or DUP after first token");  // Fall through
      default:
        ret[i] = new_string[i];
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::name::tokenizer

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
