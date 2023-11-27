/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include "likelihood_payload.h"

#include <utility>

// -----------------------------------------------------------------------------

namespace genie {
namespace likelihood {

// -----------------------------------------------------------------------------

LikelihoodPayload::LikelihoodPayload(LikelihoodParameters _parameters, uint32_t _nrows, uint32_t _ncols,
                                     std::vector<uint8_t> _payload, uint32_t _additionalPayloadSize,
                                     std::vector<uint8_t> _additionalPayload)
    : nrows(_nrows),
      ncols(_ncols),
      payload(std::move(_payload)),
      additionalPayloadSize(_additionalPayloadSize),
      additionalPayload(std::move(_additionalPayload)) {};


// -----------------------------------------------------------------------------

void LikelihoodPayload::write(core::Writer& writer) const {};

// -----------------------------------------------------------------------------

}
}

// -----------------------------------------------------------------------------