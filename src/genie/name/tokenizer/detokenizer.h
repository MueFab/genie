/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DETOKENIZER_H
#define GENIE_DETOKENIZER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/data-block.h>
#include "token.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

/**
 *
 */
class NameDecoder {
    std::vector<std::vector<genie::util::DataBlock>> seq;  //!<
    std::vector<std::vector<size_t>> position;             //!<
    std::vector<SingleToken> oldRec;                       //!<

    /**
     *
     * @param i
     * @param j
     * @return
     */
    uint32_t get(size_t i, size_t j);

    /**
     *
     * @param i
     * @param j
     * @return
     */
    uint32_t pull(size_t i, size_t j);

   public:
    /**
     *
     * @param sequences
     */
    explicit NameDecoder(std::vector<std::vector<genie::util::DataBlock>>&& sequences);

    /**
     *
     * @param rec
     * @return
     */
    std::string inflate(const std::vector<SingleToken>& rec);

    /**
     *
     * @return
     */
    std::string decode();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DETOKENIZER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------