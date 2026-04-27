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

LikelihoodPayload::LikelihoodPayload()
    : nrows(0), ncols(0), transform_flag(false), payload(), additionalPayload(), payloadStream(), additionalPayloadStream() {}

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

LikelihoodPayload::LikelihoodPayload(detail::LikelihoodEncodingBlock& block) {
    nrows = block.nrows;
    ncols = block.ncols;
    payloadStream << block.serialized_mat.rdbuf();
}

// ---------------------------------------------------------------------------------------------------------------------

LikelihoodPayload::LikelihoodPayload(LikelihoodParameters parameters, detail::LikelihoodEncodingBlock& data)
    : LikelihoodPayload(data)
{
    transform_flag = parameters.GetTransformFlag();
    if (transform_flag) {
        additionalPayloadStream << data.serialized_arr.rdbuf();
    }
}

// -----------------------------------------------------------------------------

LikelihoodPayload::LikelihoodPayload(const LikelihoodPayload& other)
    : nrows(other.nrows),
      ncols(other.ncols),
      transform_flag(other.transform_flag),
      payload(other.payload),
      additionalPayload(other.additionalPayload),
      payloadStream{other.payloadStream.str()},
      additionalPayloadStream{other.additionalPayloadStream.str()} {}

// -----------------------------------------------------------------------------

LikelihoodPayload::LikelihoodPayload(LikelihoodPayload&& other) noexcept
    : nrows(other.nrows),
      ncols(other.ncols),
      transform_flag(other.transform_flag),
      payload(std::move(other.payload)),
      additionalPayload(std::move(other.additionalPayload)),
      payloadStream{std::move(other.payloadStream)},
      additionalPayloadStream{std::move(other.additionalPayloadStream)} {}

// -----------------------------------------------------------------------------

LikelihoodPayload& LikelihoodPayload::operator=(const LikelihoodPayload& other) {
    if (this != &other) {
        nrows = other.nrows;
        ncols = other.ncols;
        transform_flag = other.transform_flag;
        payload = other.payload;
        additionalPayload = other.additionalPayload;
        payloadStream.str(other.payloadStream.str());
        additionalPayloadStream.str(other.additionalPayloadStream.str());
    }
    return *this;
}

// -----------------------------------------------------------------------------

LikelihoodPayload& LikelihoodPayload::operator=(LikelihoodPayload&& other) noexcept {
    if (this != &other) {
        nrows = other.nrows;
        ncols = other.ncols;
        transform_flag = other.transform_flag;
        payload = std::move(other.payload);
        additionalPayload = std::move(other.additionalPayload);
        payloadStream = std::move(other.payloadStream);
        additionalPayloadStream = std::move(other.additionalPayloadStream);
    }
    return *this;
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

void LikelihoodPayload::setAdditionalPayload(const std::vector<uint8_t>& _payload) { additionalPayload = _payload; }

// -----------------------------------------------------------------------------

void LikelihoodPayload::write(util::BitWriter& writer) const {
    writer.WriteBits(nrows, 32);
    writer.WriteBits(ncols, 32);
    writer.WriteBits(transform_flag ? 1 : 0, 8);

    if (!payload.empty()) {
        writer.WriteBits(payload.size(), 32);
        for (unsigned char idx_i : payload) writer.WriteBits(idx_i, 8);
        if (transform_flag) {
            writer.WriteBits(additionalPayload.size(), 32);
            for (unsigned char val : additionalPayload) writer.WriteBits(val, 8);
        }
    } else {
        std::string str = payloadStream.str();
        writer.WriteBits(str.size(), 32);
        for (unsigned char c : str) writer.WriteBits(c, 8);

        if (transform_flag) {
            std::string addStr = additionalPayloadStream.str();
            writer.WriteBits(addStr.size(), 32);
            for (unsigned char c : addStr) writer.WriteBits(c, 8);
        }
    }
}

void LikelihoodPayload::read(util::BitReader& reader) {
    nrows = static_cast<uint32_t>(reader.ReadBits(32));
    ncols = static_cast<uint32_t>(reader.ReadBits(32));
    transform_flag = reader.ReadBits(8) != 0;

    uint32_t payload_size = static_cast<uint32_t>(reader.ReadBits(32));
    payload.resize(payload_size);
    for (uint32_t idx_i = 0; idx_i < payload_size; ++idx_i) {
        payload[idx_i] = static_cast<uint8_t>(reader.ReadBits(8));
    }

    if (transform_flag) {
        uint32_t additional_size = static_cast<uint32_t>(reader.ReadBits(32));
        additionalPayload.resize(additional_size);
        for (uint32_t idx_i = 0; idx_i < additional_size; ++idx_i) {
            additionalPayload[idx_i] = static_cast<uint8_t>(reader.ReadBits(8));
        }
    }
}

// -----------------------------------------------------------------------------

}  // namespace likelihood
}  // namespace genie

// -----------------------------------------------------------------------------