/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_BLOCK_H_
#define SRC_GENIE_FORMAT_MGB_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <sstream>
#include <vector>
#include "boost/variant/get.hpp"
#include "boost/variant/variant.hpp"
#include "genie/core/access-unit.h"
#include "genie/core/payload.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * @brief
 */
class Block {
 private:
    uint8_t descriptor_ID;        //!< @brief
    uint32_t block_payload_size;  //!< @brief

    uint8_t count;

    boost::variant<genie::core::Payload, core::AccessUnit::Descriptor> payload;  //!< @brief
 public:
    bool isLoaded() const {
        return (payload.type() == typeid(genie::core::Payload) &&
                boost::get<genie::core::Payload>(payload).isPayloadLoaded()) ||
               payload.type() == typeid(genie::core::AccessUnit::Descriptor);
    }

    bool isParsed() const { return payload.type() == typeid(genie::core::AccessUnit::Descriptor); }

    void load() {
        if (isLoaded()) {
            return;
        }
        boost::get<genie::core::Payload>(payload).loadPayload();
    }

    void unload() {
        if (!isLoaded()) {
            return;
        }
        if (!isParsed()) {
            boost::get<genie::core::Payload>(payload).unloadPayload();
        }
    }

    void parse() {
        if (!isLoaded()) {
            load();
        }
        std::stringstream ss;

        ss.write((char *)boost::get<genie::core::Payload>(payload).getPayload().getData(),
                 boost::get<genie::core::Payload>(payload).getPayload().getRawSize());

        util::BitReader reader(ss);

        payload = core::AccessUnit::Descriptor(core::GenDesc(descriptor_ID), count, block_payload_size, reader);
    }

    void pack() {
        if (!isParsed()) {
            return;
        }
        std::stringstream ss;
        util::BitWriter writer(&ss);

        boost::get<core::AccessUnit::Descriptor>(payload).write(writer);

        util::BitReader reader(ss);

        payload = core::Payload(reader, ss.str().length());
    };

    /**
     * @brief
     */
    Block();

    /**
     * @brief
     * @param _descriptor_ID
     * @param _payload
     */
    Block(uint8_t _descriptor_ID, core::AccessUnit::Descriptor &&_payload);

    Block(genie::core::GenDesc _descriptor_ID, core::Payload _payload)
        : descriptor_ID(uint8_t (_descriptor_ID)),
          block_payload_size(_payload.getPayloadSize()),
          count(0),
          payload(std::move(_payload)) {}

    /**
     * @brief
     * @param qv_count
     * @param reader
     */
    explicit Block(size_t qv_count, util::BitReader &reader);

    /**
     * @brief
     */
    virtual ~Block() = default;

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;

    /**
     * @brief
     * @return
     */
    core::AccessUnit::Descriptor &&movePayload();

    core::Payload movePayloadUnparsed() {
        return std::move(boost::get<core::Payload>(payload));
    }

    core::Payload& getPayloadUnparsed() {
        return boost::get<core::Payload>(payload);
    }

    /**
     * @brief
     * @return
     */
    uint8_t getDescriptorID() const;

    /**
     * @brief
     * @return
     */
    size_t getWrittenSize() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
