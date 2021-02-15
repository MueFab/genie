/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOCALASSEMBLY_DECODER_H_
#define SRC_GENIE_READ_LOCALASSEMBLY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/read-decoder.h"
#include "genie/read/localassembly/local-reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace localassembly {

/**
 *
 */
class Decoder : public core::ReadDecoder {
 private:
 public:
    /**
     *
     * @param t
     * @param id
     */
    void flowIn(core::AccessUnit&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace localassembly
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOCALASSEMBLY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
