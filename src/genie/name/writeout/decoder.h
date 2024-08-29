/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_NAME_WRITEOUT_DECODER_H_
#define SRC_GENIE_NAME_WRITEOUT_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>
#include "genie/core/name-decoder.h"
#include "genie/name/tokenizer/token.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace writeout {

/**
 * @brief
 */
class Decoder : public core::NameDecoder {
 public:
    /**
     * @brief
     * @param desc
     * @return
     */
    std::tuple<std::vector<std::string>, core::stats::PerfStats> process(core::AccessUnit::Descriptor& desc) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace writeout
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_NAME_WRITEOUT_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
