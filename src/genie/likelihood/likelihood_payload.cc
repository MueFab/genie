/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "likelihood_payload.h"

#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace likelihood {

// ---------------------------------------------------------------------------------------------------------------------

LikelihoodPayload::LikelihoodPayload(LikelihoodParameters _parameters, uint32_t _nrows, uint32_t _ncols,
                                     std::vector<uint8_t> _payload, std::vector<uint8_t> _additionalPayload)
    : nrows(_nrows),
      ncols(_ncols),
      transform_flag(false),
      payload(std::move(_payload)),
      additionalPayload(std::move(_additionalPayload)),
      payloadStream{},
      additionalPayloadStream{} {
    (void)_parameters;
}

// ---------------------------------------------------------------------------------------------------------------------

LikelihoodPayload::LikelihoodPayload(genie::likelihood::EncodingBlock& block) {
    nrows = block.nrows;
    ncols = block.ncols;
    payloadStream << block.serialized_mat.rdbuf();
}

// ---------------------------------------------------------------------------------------------------------------------

LikelihoodPayload::LikelihoodPayload(genie::likelihood::LikelihoodParameters parameters, genie::likelihood::EncodingBlock& data)
    : LikelihoodPayload(data)
{
    transform_flag = parameters.getTransformFlag();
    if (transform_flag) {
        additionalPayloadStream << data.serialized_arr.rdbuf();
    }
}

// -----------------------------------------------------------------------------

void LikelihoodPayload::write(core::Writer& writer) const { 
    writer.write(nrows, 32);
    writer.write(ncols, 32);
    if (!payload.empty()) {
        writer.write(payload.size(), 32);
        for(unsigned char i : payload)
            writer.write(i,8);
        if (transform_flag) {
            writer.write(additionalPayload.size(), 32);
            for (size_t i = 0; i < additionalPayload.size(); ++i)
                writer.write(additionalPayload.at(i), 8);
        }
    } else{
        writer.write(payloadStream.str().size(), 32);
        std::istream writestream(payloadStream.rdbuf());
        writer.write(&writestream);
        if (transform_flag) {
            writer.write(additionalPayloadStream.str().size(), 32);
            std::istream additionalWritestream(additionalPayloadStream.rdbuf());
            writer.write(&additionalWritestream);
        }
    }

}

// -----------------------------------------------------------------------------

}  // namespace likelihood
}  // namespace genie

// -----------------------------------------------------------------------------