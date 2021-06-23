/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/name/tokenizer/tokenizer.h"
#include <utility>
#include "genie/core/access-unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

// ---------------------------------------------------------------------------------------------------------------------

bool TokenState::more() const { return cur_it != end_it; }

// ---------------------------------------------------------------------------------------------------------------------

void TokenState::step() { cur_it++; }

// ---------------------------------------------------------------------------------------------------------------------

char TokenState::get() { return *cur_it; }

// ---------------------------------------------------------------------------------------------------------------------

void TokenState::pushToken(Tokens t, uint32_t param) {
    curRec.emplace_back(t, param, "");
    token_pos++;
}

// ---------------------------------------------------------------------------------------------------------------------

void TokenState::pushToken(const SingleToken& t) {
    curRec.emplace_back(t);
    token_pos++;
}

// ---------------------------------------------------------------------------------------------------------------------

void TokenState::pushTokenString(const std::string& param) {
    curRec.emplace_back(Tokens::STRING, 0, param);
    token_pos++;
}

// ---------------------------------------------------------------------------------------------------------------------

const SingleToken& TokenState::getOldToken() {
    static const SingleToken INVALID(Tokens::NONE, 0, "");
    if (oldRec.size() > token_pos) {
        return oldRec[token_pos];
    } else {
        return INVALID;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TokenState::alphabetic() {
    SingleToken tok(Tokens::STRING, 0, "");
    while (more() && isalpha(get())) {
        tok.paramString += get();
        step();
    }
    if (tok == getOldToken()) {
        // The token is the same as last ID
        // Encode a token_type ID_MATCH
        pushToken(Tokens::MATCH);

    } else {
        pushToken(tok);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TokenState::zeros() {
    SingleToken tok(Tokens::STRING, 0, "");
    while (more() && get() == '0') {
        tok.paramString += '0';
        step();
    }
    if (getOldToken() == tok) {
        // The token is the same as last ID
        // Encode a token_type ID_MATCH
        pushToken(Tokens::MATCH);
    } else {
        pushToken(tok);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TokenState::number() {
    const uint32_t MAX_NUMBER = 1 << 26;  // 8 digits + 1 digit fetched below
    SingleToken tok(Tokens::DIGITS, 0, "");
    while (more() && isdigit(get()) && tok.param < MAX_NUMBER) {
        tok.param *= 10;
        tok.param += get() - '0';
        step();
    }

    auto delta = tok.param - getOldToken().param;
    if (getOldToken() == tok) {
        pushToken(Tokens::MATCH);
    } else if (getOldToken().token == Tokens::DIGITS && tok.param > getOldToken().param &&
               delta <= std::numeric_limits<uint8_t>::max()) {
        pushToken(Tokens::DELTA, tok.param - getOldToken().param);
    } else {
        pushToken(tok);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TokenState::character() {
    SingleToken tok(Tokens::CHAR, *cur_it, "");
    step();
    if (tok == getOldToken()) {
        pushToken(Tokens::MATCH);

    } else {
        // Encode a token type ID_CHAR and the char
        pushToken(tok);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TokenState::TokenState(const std::vector<SingleToken>& old, const std::string& input)
    : token_pos(0), oldRec(old), cur_it(input.begin()), end_it(input.end()) {}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<SingleToken>&& TokenState::run() {
    if (getOldToken().token == Tokens::DIFF) {
        pushToken(Tokens::DIFF, 1);
    } else {
        pushToken(Tokens::DIFF, 0);
    }
    while (more()) {
        // Check if the token is an alphabetic word
        if (isalpha(get())) {
            alphabetic();
        } else if (get() == '0') {  // check if the token is a run of zeros
            zeros();
        } else if (isdigit(get())) {  // Check if the token is a number smaller than (1<<29)
            number();
        } else {
            character();
        }
    }

    pushToken(Tokens::END);
    return std::move(curRec);
}

// ---------------------------------------------------------------------------------------------------------------------

void push32bigEndian(core::AccessUnit::Subsequence& seq, uint32_t value) {
    seq.push((value >> 24) & 0xff);
    seq.push((value >> 16) & 0xff);
    seq.push((value >> 8) & 0xff);
    seq.push((value)&0xff);
}

// ---------------------------------------------------------------------------------------------------------------------

void TokenState::encode(const std::vector<SingleToken>& tokens, core::AccessUnit::Descriptor& streams) {
    for (uint16_t i = 0; i < tokens.size(); ++i) {
        //   while (streams.size() <= i) {
        //       streams.emplace_back(getTokenInfo(Tokens::DZLEN).paramSeq + 1, util::DataBlock(0, 4));
        //   }
        streams.getTokenType(i, TYPE_SEQ).push((uint8_t)tokens[i].token);
        if (getTokenInfo(tokens[i].token).paramSeq == 0) {
            continue;
        }

        if (tokens[i].token == Tokens::STRING) {
            for (const auto& c : tokens[i].paramString) {
                streams.getTokenType(i, (uint8_t)tokens[i].token).push(c);
            }
            streams.getTokenType(i, (uint8_t)tokens[i].token).push('\0');
        } else if (getTokenInfo(tokens[i].token).paramSeq == sizeof(uint32_t)) {
            push32bigEndian(streams.getTokenType(i, (uint8_t)tokens[i].token), tokens[i].param);
        } else {
            streams.getTokenType(i, (uint8_t)tokens[i].token).push(tokens[i].param);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
