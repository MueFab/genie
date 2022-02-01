/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_BLOCK_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/format/mpegg_p1/block_header.h"
#include "genie/util/data-block.h"
#include "genie/format/mpegg_p1/box.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Block : public Box {
 private:
    BlockHeader header;                    //!< @brief
    genie::util::DataBlock block_payload;  //!< @brief block_payload_size (implicite), block_payload[]

    bool payloadLoaded;
    int64_t payloadPosition;

    genie::util::DataBlock _internal_loadPayload(util::BitReader& reader) const {
        auto pos = reader.getPos();
        reader.setPos(payloadPosition);
        genie::util::DataBlock tmp;
        tmp.resize(header.getPayloadSize());
        reader.readBypass(tmp.getData(), header.getPayloadSize());
        reader.setPos(pos);
        return tmp;
    }

    util::BitReader* internal_reader;

 public:

    void loadPayload()  {
        block_payload = _internal_loadPayload(*internal_reader);
        payloadLoaded = true;
    }

    void unloadPayload () {
        payloadLoaded = false;
        block_payload.clear();
    }

    bool isPayloadLoaded () const {
        return payloadLoaded;
    }

    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override {
        print_offset(output, depth, max_depth, "* Block");
        print_offset(output, depth + 1, max_depth, "Block descriptor ID: " + genie::core::getDescriptor(header.getDescriptorID()).name);
        print_offset(output, depth + 1, max_depth, "Block payload size: " + std::to_string(header.getPayloadSize()));
    }

    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const Block& other) const;

    /**
     * @brief
     * @param reader
     */
    explicit Block(util::BitReader& reader);

    /**
     * @brief
     * @param _desc_id
     * @param payload
     */
    Block(genie::core::GenDesc _desc_id, genie::util::DataBlock payload);

    /**
     * @brief
     * @return
     */
    uint32_t getPayloadSize() const;

    /**
     * @brief
     * @return
     */
    const genie::util::DataBlock& getPayload() const;

    /**
     * @brief
     * @return
     */
    genie::util::DataBlock&& movePayload();

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const override;

    /**
     * @brief
     * @return
     */
    genie::core::GenDesc getDescID() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
