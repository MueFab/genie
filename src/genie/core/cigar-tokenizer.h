/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CIGAR_TOKENIZER_H
#define GENIE_CIGAR_TOKENIZER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/constants.h>
#include <genie/util/stringview.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class CigarTokenizer {
   public:
    /**
     *
     * @tparam Lambda
     * @param cigar
     * @param format
     * @param lambda
     */
    template <typename Lambda>
    static void tokenize(const std::string& cigar, const CigarFormatInfo& format, const Lambda& lambda);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "cigar-tokenizer.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CIGAR_TOKENIZER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------