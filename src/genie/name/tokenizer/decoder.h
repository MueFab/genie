/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_TOK_DECODER_H
#define GENIE_TOK_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/data-block.h>
#include <name-decoder.h>
#include "token.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

/**
 *
 */
class Decoder : public core::NameDecoder {
    /**
     *
     * @param rec
     * @return
     */
    static std::string inflate(const std::vector<SingleToken>& rec);

   public:
    /**
     *
     * @param desc
     * @return
     */
    std::tuple<std::vector<std::string>, core::stats::PerfStats> process(core::AccessUnit::Descriptor& desc) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------