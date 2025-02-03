/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PAYLOAD_H_
#define SRC_GENIE_CORE_PAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/data_block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class Payload {
 private:
    genie::util::DataBlock block_payload;  //!< @brief

    bool payloadLoaded;       //!< @brief
    int64_t payloadPosition;  //!< @brief
    uint64_t payloadSize;     //!< @brief

    util::BitReader* internal_reader;  //!< @brief

    /**
     * @brief
     * @param reader
     * @return
     */
    genie::util::DataBlock _internal_loadPayload(util::BitReader& reader) const;

 public:
    /**
     * @brief
     * @return
     */
    uint64_t getPayloadSize() const;

    /**
     * @brief
     */
    void loadPayload();

    /**
     * @brief
     */
    void unloadPayload();

    /**
     * @brief
     * @return
     */
    bool isPayloadLoaded() const;

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
     * @param payload
     */
    explicit Payload(genie::util::DataBlock payload);

    /**
     * @brief
     * @param reader
     * @param size
     */
    explicit Payload(util::BitReader& reader, uint64_t size);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const Payload& other) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
