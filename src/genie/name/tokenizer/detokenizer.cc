/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "detokenizer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

// ---------------------------------------------------------------------------------------------------------------------

uint32_t NameDecoder::get(size_t i, size_t j) { return seq[i][j].get(position[i][j]); }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t NameDecoder::pull(size_t i, size_t j) {
    auto ret = get(i, j);
    position[i][j]++;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

NameDecoder::NameDecoder(std::vector<std::vector<genie::util::DataBlock>>&& sequences)
    : seq(std::move(sequences)) {
    for (const auto& i : seq) {
        position.emplace_back();
        for (size_t j = 0; j < i.size(); ++j) {
            position.back().push_back(0);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::string NameDecoder::inflate(const std::vector<SingleToken>& rec) {
    std::string ret;
    for (const auto& st : rec) {
        switch (st.token) {
            case Tokens::DUP:
            case Tokens::DELTA0:
            case Tokens::DELTA:
            case Tokens::MATCH:
                UTILS_DIE("DUP. DELTA0, DELTA, MATCH should be patched before inflation");
                break;
            case Tokens::DIFF:
                UTILS_DIE_IF(st.param > 1, "Only DIFF <= 1 supported");
                break;
            case Tokens::STRING:
                ret += st.paramString;
                break;
            case Tokens::CHAR:
                ret += (char)st.param;
                break;
            case Tokens::DIGITS:
                ret += std::to_string(st.param);
                break;
            case Tokens::DIGITS0:
            case Tokens::DZLEN:
                UTILS_DIE("DIGITS0, DZLEN not supported");
                break;
            case Tokens::END:
                break;
            default:
                UTILS_DIE("Invalid token");
        }
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string NameDecoder::decode() {
    size_t cur_pos = 0;

    std::vector<SingleToken> rec;

    if (position[0][TYPE_SEQ] == 0) {
        UTILS_DIE_IF(Tokens(get(0, TYPE_SEQ)) != Tokens::DIFF, "First token in AU must be DIFF");
        UTILS_DIE_IF(get(0, getTokenInfo(Tokens::DIFF).paramSeq) != 0, "First DIFF in AU must be 0");
    }

    auto type = Tokens(pull(cur_pos, TYPE_SEQ));

    while (type != Tokens::END) {
        SingleToken t(type, 0, "");

        if (type == Tokens::STRING) {
            char c = pull(cur_pos, getTokenInfo(Tokens::STRING).paramSeq);
            while (c != '\0') {
                t.paramString += c;
                c = pull(cur_pos, getTokenInfo(Tokens::STRING).paramSeq);
            }
        } else if (getTokenInfo(type).paramSeq > 0) {
            t.param = pull(cur_pos, getTokenInfo(type).paramSeq);
        }

        rec.push_back(t);

        cur_pos++;
        type = Tokens(pull(cur_pos, TYPE_SEQ));
    }

    rec = patch(oldRec, rec);
    oldRec = rec;

    return inflate(rec);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------