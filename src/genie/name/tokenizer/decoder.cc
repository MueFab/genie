/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decoder.h"
#include <genie/util/watch.h>

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

uint32_t pull32bigEndian(core::AccessUnit::Subsequence& seq) {
    uint32_t ret = 0;
    ret |= seq.pull() << 24;
    ret |= seq.pull() << 16;
    ret |= seq.pull() << 8;
    ret |= seq.pull();
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<std::vector<std::string>, core::stats::PerfStats> Decoder::process(core::AccessUnit::Descriptor& desc) {
    std::tuple<std::vector<std::string>, core::stats::PerfStats> ret;
    std::vector<SingleToken> oldRec;
    util::Watch watch;
    while (!desc.getTokenType(0, TYPE_SEQ).end()) {
        size_t cur_pos = 0;

        std::vector<SingleToken> rec;

        if (std::get<0>(ret).empty()) {
            UTILS_DIE_IF(Tokens(desc.getTokenType(0, TYPE_SEQ).get()) != Tokens::DIFF,
                         "First token in AU must be DIFF");
            UTILS_DIE_IF(desc.getTokenType(0, (uint8_t)Tokens::DIFF).get() != 0, "First DIFF in AU must be 0");
        }

        auto type = Tokens(desc.getTokenType(cur_pos, TYPE_SEQ).pull());

        while (type != Tokens::END) {
            SingleToken t(type, 0, "");

            if (type == Tokens::STRING) {
                char c = desc.getTokenType(cur_pos, (uint8_t)Tokens::STRING).pull();
                while (c != '\0') {
                    t.paramString += c;
                    c = desc.getTokenType(cur_pos, (uint8_t)Tokens::STRING).pull();
                }
            } else if (type == Tokens::DIGITS || type == Tokens::DIGITS0) {
                t.param = pull32bigEndian(desc.getTokenType(cur_pos, (uint8_t)type));
            } else if ((uint8_t)type < uint8_t(Tokens::MATCH)) {
                t.param = desc.getTokenType(cur_pos, (uint8_t)type).pull();
            }

            rec.push_back(t);

            cur_pos++;
            type = Tokens(desc.getTokenType(cur_pos, TYPE_SEQ).pull());
        }

        rec = patch(oldRec, rec);
        oldRec = rec;

        std::get<0>(ret).emplace_back(inflate(rec));
    }
    std::get<1>(ret).addDouble("time-nametokenize", watch.check());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------