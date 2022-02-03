//
// Created by muenteferi on 03.02.22.
//

#ifndef GENIE_PAYLOAD_H
#define GENIE_PAYLOAD_H

#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/data-block.h"

namespace genie {
namespace core {

class Payload {
 private:
    genie::util::DataBlock block_payload;

    bool payloadLoaded;
    int64_t payloadPosition;
    uint64_t payloadSize;

    util::BitReader* internal_reader;

    genie::util::DataBlock _internal_loadPayload(util::BitReader& reader) const {
        auto pos = reader.getPos();
        reader.setPos(payloadPosition);
        genie::util::DataBlock tmp;
        tmp.resize(payloadSize);
        reader.readBypass(tmp.getData(), payloadSize);
        reader.setPos(pos);
        return tmp;
    }

 public:
    uint64_t getPayloadSize() const {
        if (isPayloadLoaded()) {
            return block_payload.getRawSize();
        } else {
            return payloadSize;
        }
    }

    void loadPayload() {
        block_payload = _internal_loadPayload(*internal_reader);
        payloadLoaded = true;
    }

    void unloadPayload() {
        payloadLoaded = false;
        block_payload.clear();
    }

    bool isPayloadLoaded() const { return payloadLoaded; }

    const genie::util::DataBlock& getPayload() const { return block_payload; }

    genie::util::DataBlock&& movePayload() {
        payloadLoaded = false;
        return std::move(block_payload);
    }

    explicit Payload(genie::util::DataBlock payload)
        : block_payload(std::move(payload)),
          payloadLoaded(true),
          payloadPosition(-1),
          payloadSize(payload.getRawSize()),
          internal_reader(nullptr) {}

    explicit Payload(util::BitReader& reader, uint64_t size)
        : block_payload(),
          payloadLoaded(false),
          payloadPosition(reader.getPos()),
          payloadSize(size),
          internal_reader(&reader) {
        reader.skip(size);
    }

    void write(genie::util::BitWriter& writer) const {
        if (!isPayloadLoaded() && internal_reader) {
            auto tmp = _internal_loadPayload(*internal_reader);
            writer.writeBypass(tmp.getData(), tmp.getRawSize());
        } else {
            writer.writeBypass(block_payload.getData(), block_payload.getRawSize());
        }
    }

    bool operator==(const Payload& other) const {
        return payloadSize == other.payloadSize && payloadPosition == other.payloadPosition &&
               payloadLoaded == other.payloadLoaded && block_payload == other.block_payload;
    }
};

}  // namespace core
}  // namespace genie

#endif  // GENIE_PAYLOAD_H
