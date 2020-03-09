/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

// ---------------------------------------------------------------------------------------------------------------------

std::string Decoder::inflate(const std::vector<SingleToken>& rec) {
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

std::vector<std::string> Decoder::decode(core::AccessUnitRaw::Descriptor& desc)  {
    std::vector<std::string> ret;
    std::vector<SingleToken> oldRec;
    while(!desc.getTokenType(0, TYPE_SEQ).end()) {
        size_t cur_pos = 0;

        std::vector<SingleToken> rec;

        if (ret.empty()) {
            UTILS_DIE_IF(Tokens(desc.getTokenType(0, TYPE_SEQ).get()) != Tokens::DIFF, "First token in AU must be DIFF");
            UTILS_DIE_IF(desc.getTokenType(0, getTokenInfo(Tokens::DIFF).paramSeq).get() != 0, "First DIFF in AU must be 0");
        }

        auto type = Tokens(desc.getTokenType(cur_pos, TYPE_SEQ).pull());

        while (type != Tokens::END) {
            SingleToken t(type, 0, "");

            if (type == Tokens::STRING) {
                char c = desc.getTokenType(cur_pos, getTokenInfo(Tokens::STRING).paramSeq).pull();
                while (c != '\0') {
                    t.paramString += c;
                    c = desc.getTokenType(cur_pos, getTokenInfo(Tokens::STRING).paramSeq).pull();
                }
            } else if (getTokenInfo(type).paramSeq > 0) {
                t.param = desc.getTokenType(cur_pos, getTokenInfo(type).paramSeq).pull();
            }

            rec.push_back(t);

            cur_pos++;
            type = Tokens(desc.getTokenType(cur_pos, TYPE_SEQ).pull());
        }

        rec = patch(oldRec, rec);
        oldRec = rec;

        ret.emplace_back(inflate(rec));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------