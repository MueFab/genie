/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#ifndef GENIE_LIKELIHOOD_LIKELIHOOD_PAYLOAD_H
#define GENIE_LIKELIHOOD_LIKELIHOOD_PAYLOAD_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genie/likelihood/likelihood_coder.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace likelihood {

// -----------------------------------------------------------------------------

class LikelihoodPayload {
 private:
    uint32_t nrows;
    uint32_t ncols;
    bool transform_flag;
    std::vector<uint8_t> payload;
    std::vector<uint8_t> additionalPayload;
    std::stringstream payloadStream;
    std::stringstream additionalPayloadStream;

 public:
     LikelihoodPayload(
         LikelihoodParameters _parameters,
         uint32_t _nrows,
         uint32_t _ncols,
        std::vector<uint8_t> _payload,
        std::vector<uint8_t> _additionalPayload
    );

    LikelihoodPayload(genie::likelihood::EncodingBlock& block);
    LikelihoodPayload(genie::likelihood::LikelihoodParameters parameters, genie::likelihood::EncodingBlock& data);
    void write(core::Writer& writer) const;
};

// -----------------------------------------------------------------------------

}
}

// -----------------------------------------------------------------------------

#endif  // GENIE_LIKELIHOOD_LIKELIHOOD_PAYLOAD_H