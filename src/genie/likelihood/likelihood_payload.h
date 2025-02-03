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
#include "genie/likelihood/likelihood_coder.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

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

    // Getters
    uint32_t getNRows() const;
    uint32_t getNCols() const;
    bool getTransformFlag() const;
    const std::vector<uint8_t>& getPayload() const;
    const std::vector<uint8_t>& getAdditionalPayload() const;
    const std::stringstream& getPayloadStream() const;
    const std::stringstream& getAdditionalPayloadStream() const;

    // Setters
    void setNRows(uint32_t rows);
    void setNCols(uint32_t cols);
    void setTransformFlag(bool flag);
    void setPayload(const std::vector<uint8_t>& _payload);

    void write(core::Writer& writer) const;
};

// -----------------------------------------------------------------------------

}
}

// -----------------------------------------------------------------------------

#endif  // GENIE_LIKELIHOOD_LIKELIHOOD_PAYLOAD_H
