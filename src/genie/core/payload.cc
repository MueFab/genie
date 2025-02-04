/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/payload.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

genie::util::DataBlock Payload::_internal_loadPayload(util::BitReader& reader) const {
    auto pos = reader.GetStreamPosition();
    reader.SetStreamPosition(payloadPosition);
    genie::util::DataBlock tmp;
    tmp.Resize(payloadSize);
    reader.ReadAlignedBytes(tmp.GetData(), payloadSize);
    reader.SetStreamPosition(pos);
    return tmp;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Payload::getPayloadSize() const {
    if (isPayloadLoaded()) {
        return block_payload.GetRawSize();
    } else {
        return payloadSize;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Payload::loadPayload() {
    block_payload = _internal_loadPayload(*internal_reader);
    payloadLoaded = true;
}

// ---------------------------------------------------------------------------------------------------------------------

void Payload::unloadPayload() {
    payloadLoaded = false;
    block_payload.Clear();
}

// ---------------------------------------------------------------------------------------------------------------------

bool Payload::isPayloadLoaded() const { return payloadLoaded; }

// ---------------------------------------------------------------------------------------------------------------------

const genie::util::DataBlock& Payload::getPayload() const { return block_payload; }

// ---------------------------------------------------------------------------------------------------------------------

genie::util::DataBlock&& Payload::movePayload() {
    payloadLoaded = false;
    return std::move(block_payload);
}

// ---------------------------------------------------------------------------------------------------------------------

Payload::Payload(genie::util::DataBlock payload)
    : block_payload(std::move(payload)),
      payloadLoaded(true),
      payloadPosition(-1),
      payloadSize(payload.GetRawSize()),
      internal_reader(nullptr) {}

// ---------------------------------------------------------------------------------------------------------------------

Payload::Payload(util::BitReader& reader, uint64_t size)
    : block_payload(),
      payloadLoaded(false),
      payloadPosition(reader.GetStreamPosition()),
      payloadSize(size),
      internal_reader(&reader) {
    reader.SkipAlignedBytes(size);
}

// ---------------------------------------------------------------------------------------------------------------------

void Payload::write(genie::util::BitWriter& writer) const {
    if (!isPayloadLoaded() && internal_reader) {
        auto tmp = _internal_loadPayload(*internal_reader);
        writer.WriteAlignedBytes(tmp.GetData(), tmp.GetRawSize());
    } else {
        writer.WriteAlignedBytes(block_payload.GetData(), block_payload.GetRawSize());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Payload::operator==(const Payload& other) const {
    return payloadSize == other.payloadSize && payloadPosition == other.payloadPosition &&
           payloadLoaded == other.payloadLoaded && block_payload == other.block_payload;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
