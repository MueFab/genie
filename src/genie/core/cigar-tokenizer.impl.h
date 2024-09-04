/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_CIGAR_TOKENIZER_IMPL_H_
#define SRC_GENIE_CORE_CIGAR_TOKENIZER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cctype>
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

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
        auto old_pos_ref = pos_ref;
        if (size == 0) {
            pos_bases += 1;
            pos_ref += 1;
        } else {
            pos_bases += size * format.lut_step_bases[c];
            pos_ref += size * format.lut_step_ref[c];
        }

        if (!lambda(c, std::make_pair(old_pos_bases, pos_bases - old_pos_bases),
                    std::make_pair(old_pos_ref, pos_ref - old_pos_ref))) {
            return;
        }

        size = 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_CIGAR_TOKENIZER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
