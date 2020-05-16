/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LAE_DECODER_H
#define GENIE_LAE_DECODER_H

#include <genie/core/read-decoder.h>
#include "local-reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace localassembly {

// ---------------------------------------------------------------------------------------------------------------------

class Decoder : public core::ReadDecoder {
   private:
   public:
    void flowIn(core::AccessUnitRaw&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace localassembly
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
