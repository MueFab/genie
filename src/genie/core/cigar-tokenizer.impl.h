/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CIGAR_TOKENIZER_IMPL_H
#define GENIE_CIGAR_TOKENIZER_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cctype>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

template <typename Lambda>
void CigarTokenizer::tokenize(const std::string& cigar, const CigarFormatInfo& format, const Lambda& lambda) {
    size_t size = 0;
    size_t pos_bases = 0;
    size_t pos_ref = 0;
    for (const auto& c : cigar) {
        if (format.lut_ignore[c]) {
            continue;
        }
        if (std::isdigit(c)) {
            size *= 10;
            size += (c - '0');
            continue;
        }

        auto old_pos_bases = pos_bases;
        auto old_pos_ref = pos_bases;
        if (size == 0) {
            pos_bases += 1;
            pos_ref += 1;
        } else {
            pos_bases += size * format.lut_step_bases[c];
            pos_ref += size * format.lut_step_ref[c];
        }

        if (!lambda(c, util::StringView(old_pos_bases, pos_bases), util::StringView(old_pos_ref, pos_ref))) {
            return;
        }

        size = 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CIGAR_TOKENIZER_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------