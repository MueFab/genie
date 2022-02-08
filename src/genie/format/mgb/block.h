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

    uint8_t count;  //!< @brief

    boost::variant<genie::core::Payload, core::AccessUnit::Descriptor> payload;  //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    bool isLoaded() const;

    /**
     * @brief
     * @return
     */
    bool isParsed() const;

    /**
     * @brief
     */
    void load();

    /**
     * @brief
     */
    void unload();

    /**
     * @brief
     */
    void parse();

    /**
     * @brief
     */
    void pack();

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

    /**
     * @brief
     * @param _descriptor_ID
     * @param _payload
     */
    Block(genie::core::GenDesc _descriptor_ID, core::Payload _payload);

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

    /**
     * @brief
     * @return
     */
    core::Payload movePayloadUnparsed();

    /**
     * @brief
     * @return
     */
    core::Payload &getPayloadUnparsed();

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
