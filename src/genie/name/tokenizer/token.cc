/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "token.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

// ---------------------------------------------------------------------------------------------------------------------

SingleToken::SingleToken(Tokens t, uint32_t p, std::string ps) : token(t), param(p), paramString(std::move(ps)) {}

// ---------------------------------------------------------------------------------------------------------------------

bool SingleToken::operator==(const SingleToken& t) const {
    return token == t.token && param == t.param && paramString == t.paramString;
}

// ---------------------------------------------------------------------------------------------------------------------

bool SingleToken::operator!=(const SingleToken& t) const { return !((*this) == t); }

// ---------------------------------------------------------------------------------------------------------------------

const TokenInfo& getTokenInfo(Tokens t) {
    static auto info = []() -> std::vector<TokenInfo> {
        std::vector<TokenInfo> ret = {{"DUP", 1},    {"DIFF", 1},  {"STRING", 2},  {"CHAR", 3},
                                      {"DIGITS", 4}, {"DELTA", 5}, {"DIGITS0", 6}, {"DELTA0", 7},
                                      {"MATCH", -1}, {"DZLEN", 8}, {"END", -1},    {"NONE", -1}};
        return ret;
    }();
    return info[uint8_t(t)];
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<SingleToken> patch(const std::vector<SingleToken>& oldString, const std::vector<SingleToken>& newString) {
    std::vector<SingleToken> ret(newString.size(), SingleToken(Tokens::DUP, 0, ""));
    if (newString.front().token == Tokens::DUP) {
        UTILS_DIE_IF(newString.front().param > 1, "DUP > 1 not supported");
        UTILS_DIE_IF(newString.size() > 1, "Found DUP but more than one token");
        return oldString;
    } else if (newString.front().token != Tokens::DIFF) {
        UTILS_DIE("First token is neither DUP nor DIFF");
    }
    for (size_t i = 0; i < newString.size(); ++i) {
        if (i >= oldString.size()) {
            UTILS_DIE_IF(newString[i].token == Tokens::DELTA || newString[i].token == Tokens::DELTA0 ||
                             newString[i].token == Tokens::MATCH,
                         "Found MATCH/DELTA but no token to match");
            ret[i] = newString[i];
            continue;
        }
        switch (newString[i].token) {
            case Tokens::MATCH:
                UTILS_DIE_IF(oldString[i].token == Tokens::DUP || oldString[i].token == Tokens::DELTA ||
                                 oldString[i].token == Tokens::DELTA0 || oldString[i].token == Tokens::MATCH ||
                                 oldString[i].token == Tokens::MATCH,
                             "Found MATCH but no token to match");
                ret[i] = oldString[i];
                break;
            case Tokens::DELTA:
                UTILS_DIE_IF(oldString[i].token != Tokens::DIGITS, "Found DELTA but no digits");
                ret[i] = SingleToken(Tokens::DIGITS, oldString[i].param + newString[i].param, "");
                break;
            case Tokens::DELTA0:
                UTILS_DIE_IF(oldString[i].token != Tokens::DIGITS0, "Found DELTA0 but no digits");
                ret[i] = SingleToken(Tokens::DIGITS0, oldString[i].param + newString[i].param, "");
                break;
            case Tokens::DIFF:
            case Tokens::DUP:
                UTILS_DIE_IF(i != 0, "Found DIFF or DUP after first token");  // Fall through
            default:
                ret[i] = newString[i];
        }
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------