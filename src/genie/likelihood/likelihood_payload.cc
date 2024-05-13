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

uint32_t LikelihoodPayload::getNRows() const { return nrows; }

// -----------------------------------------------------------------------------

uint32_t LikelihoodPayload::getNCols() const { return ncols; }

// -----------------------------------------------------------------------------

bool LikelihoodPayload::getTransformFlag() const { return transform_flag; }

// -----------------------------------------------------------------------------

const std::vector<uint8_t>& LikelihoodPayload::getPayload() const { return payload; }

// -----------------------------------------------------------------------------

const std::vector<uint8_t>& LikelihoodPayload::getAdditionalPayload() const { return additionalPayload; }

// -----------------------------------------------------------------------------

const std::stringstream& LikelihoodPayload::getPayloadStream() const { return payloadStream; }

// -----------------------------------------------------------------------------

const std::stringstream& LikelihoodPayload::getAdditionalPayloadStream() const { return additionalPayloadStream; }

// -----------------------------------------------------------------------------

void LikelihoodPayload::setNRows(uint32_t rows) { nrows = rows; }

// -----------------------------------------------------------------------------

void LikelihoodPayload::setNCols(uint32_t cols) { ncols = cols; }

// -----------------------------------------------------------------------------

void LikelihoodPayload::setTransformFlag(bool flag) { transform_flag = flag; }

// -----------------------------------------------------------------------------

void LikelihoodPayload::setPayload(const std::vector<uint8_t>& _payload) { payload = _payload; }

// -----------------------------------------------------------------------------

void LikelihoodPayload::write(core::Writer& writer) const { 
    writer.write(nrows, 32u);
    writer.write(ncols, 32u);

    if (!payload.empty()) {
        writer.write(payload.size(), 32u);
        for(unsigned char i : payload)
            writer.write(i,8);
        if (transform_flag) {
            writer.write(additionalPayload.size(), 32u);
            for (unsigned char val : additionalPayload)
                writer.write(val, 8);
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