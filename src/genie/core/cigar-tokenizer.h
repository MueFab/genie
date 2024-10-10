/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_CIGAR_TOKENIZER_H_
#define SRC_GENIE_CORE_CIGAR_TOKENIZER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class CigarTokenizer {
 public:
    /**
     * @brief
     * @tparam Lambda
     * @param cigar
     * @param format
     * @param lambda
     */
    template <typename Lambda>
    static void tokenize(const std::string& cigar, const CigarFormatInfo& format, const Lambda& lambda);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/cigar-tokenizer.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_CIGAR_TOKENIZER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
