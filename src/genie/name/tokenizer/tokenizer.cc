/**
 * Copyright 2018-2024 The Genie Authors.
 * @file tokenizer.cc
 *
 * @brief Implements the TokenState class for tokenizing and encoding genomic
 * data names.
 *
 * This file is part of the Genie project, which focuses on efficient genomic
 * data compression and processing. The `tokenizer.cpp` file provides essential
 * functionalities to tokenize genomic data names into different patterns such
 * as alphabetic sequences, numbers, and special characters, while supporting
 * matching and delta encoding for optimization.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/name/tokenizer/tokenizer.h"

#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/access_unit.h"

// -----------------------------------------------------------------------------

namespace genie::name::tokenizer {

// -----------------------------------------------------------------------------

bool TokenState::more() const { return cur_it_ != end_it_; }

// -----------------------------------------------------------------------------

void TokenState::step() { ++cur_it_; }

// -----------------------------------------------------------------------------

char TokenState::get() const { return *cur_it_; }

// -----------------------------------------------------------------------------

void TokenState::PushToken(Tokens t, uint32_t param) {
  cur_rec_.emplace_back(t, param, "");
  token_pos_++;
}

// -----------------------------------------------------------------------------

void TokenState::PushToken(const SingleToken& t) {
  cur_rec_.emplace_back(t);
  token_pos_++;
}

// -----------------------------------------------------------------------------

void TokenState::PushTokenString(const std::string& param) {
  cur_rec_.emplace_back(Tokens::STRING, 0, param);
  token_pos_++;
}

// -----------------------------------------------------------------------------

const SingleToken& TokenState::GetOldToken() const {
  static const SingleToken invalid(Tokens::NONE, 0, "");
  if (old_rec_.size() > token_pos_) {
    return old_rec_[token_pos_];
  }
  return invalid;
}

// -----------------------------------------------------------------------------

void TokenState::alphabetic() {
  SingleToken tok(Tokens::STRING, 0, "");
  while (more() && isalpha(get())) {
    tok.param_string += get();
    step();
  }
  if (tok == GetOldToken()) {
    // The token is the same as last ID
    // Encode a token_type ID_MATCH
    PushToken(Tokens::MATCH);

  } else {
    PushToken(tok);
  }
}

// -----------------------------------------------------------------------------

void TokenState::zeros() {
  SingleToken tok(Tokens::STRING, 0, "");
  while (more() && get() == '0') {
    tok.param_string += '0';
    step();
  }
  if (GetOldToken() == tok) {
    // The token is the same as last ID
    // Encode a token_type ID_MATCH
    PushToken(Tokens::MATCH);
  } else {
    PushToken(tok);
  }
}

// -----------------------------------------------------------------------------

void TokenState::number() {
  constexpr uint32_t max_number = 1 << 26;  // 8 digits + 1 digit fetched below
  SingleToken tok(Tokens::DIGITS, 0, "");
  while (more() && isdigit(get()) && tok.param < max_number) {
    tok.param *= 10;
    tok.param += get() - '0';
    step();
  }

  const auto delta = tok.param - GetOldToken().param;
  if (GetOldToken() == tok) {
    PushToken(Tokens::MATCH);
  } else if (GetOldToken().token == Tokens::DIGITS &&
             tok.param > GetOldToken().param &&
             delta <= std::numeric_limits<uint8_t>::max()) {
    PushToken(Tokens::DELTA, tok.param - GetOldToken().param);
  } else {
    PushToken(tok);
  }
}

// -----------------------------------------------------------------------------

void TokenState::character() {
  const SingleToken tok(Tokens::CHAR, *cur_it_, "");
  step();
  if (tok == GetOldToken()) {
    PushToken(Tokens::MATCH);

  } else {
    // Encode a token type ID_CHAR and the char
    PushToken(tok);
  }
}

// -----------------------------------------------------------------------------

TokenState::TokenState(const std::vector<SingleToken>& old,
                       const std::string& input)
    : token_pos_(0),
      old_rec_(old),
      cur_it_(input.begin()),
      end_it_(input.end()) {}

// -----------------------------------------------------------------------------

std::vector<SingleToken>&& TokenState::run() {
  if (GetOldToken().token == Tokens::DIFF) {
    PushToken(Tokens::DIFF, 1);
  } else {
    PushToken(Tokens::DIFF, 0);
  }
  while (more()) {
    // Check if the token is an alphabetic word
    if (isalpha(get())) {
      alphabetic();
    } else if (get() == '0') {  // check if the token is a run of zeros
      zeros();
    } else if (isdigit(get())) {  // Check if the token is a number smaller than
                                  // (1<<29)
      number();
    } else {
      character();
    }
  }

  PushToken(Tokens::END);
  return std::move(cur_rec_);
}

// -----------------------------------------------------------------------------

void Push32BigEndian(core::AccessUnit::Subsequence& seq, const uint32_t value) {
  seq.Push(value >> 24 & 0xff);
  seq.Push(value >> 16 & 0xff);
  seq.Push(value >> 8 & 0xff);
  seq.Push(value & 0xff);
}

// -----------------------------------------------------------------------------

void TokenState::encode(const std::vector<SingleToken>& tokens,
                        core::AccessUnit::Descriptor& streams) {
  UTILS_DIE_IF(tokens.size() > std::numeric_limits<uint16_t>::max(),
               "Too many tokens");
  for (uint16_t i = 0; i < static_cast<uint16_t>(tokens.size()); ++i) {
    //   while (streams.Size() <= i) {
    //       streams.emplace_back(getTokenInfo(Tokens::DZLEN).paramSeq + 1,
    //       util::DataBlock(0, 4));
    //   }
    streams.GetTokenType(i, kTypeSeq)
        .Push(static_cast<uint8_t>(tokens[i].token));
    if (GetTokenInfo(tokens[i].token).param_seq == 0) {
      continue;
    }

    if (tokens[i].token == Tokens::STRING) {
      for (const auto& c : tokens[i].param_string) {
        streams.GetTokenType(i, static_cast<uint8_t>(tokens[i].token)).Push(c);
      }
      streams.GetTokenType(i, static_cast<uint8_t>(tokens[i].token)).Push('\0');
    } else if (GetTokenInfo(tokens[i].token).param_seq == sizeof(uint32_t)) {
      Push32BigEndian(
          streams.GetTokenType(i, static_cast<uint8_t>(tokens[i].token)),
          tokens[i].param);
    } else {
      streams.GetTokenType(i, static_cast<uint8_t>(tokens[i].token))
          .Push(tokens[i].param);
    }
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::name::tokenizer

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
