/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BLOCK_H
#define GENIE_BLOCK_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/access-unit.h>
#include <genie/util/bitwriter.h>
#include <cstdint>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 *
 */
class Block {
   private:
    uint8_t descriptor_ID;        //!<
    uint32_t block_payload_size;  //!<

    core::AccessUnit::Descriptor payload;  //!<

   public:
    /**
     *
     */
    Block();

    /**
     *
     * @param _descriptor_ID
     * @param _payload
     */
    Block(uint8_t _descriptor_ID, core::AccessUnit::Descriptor &&_payload);

    /**
     *
     * @param qv_count
     * @param reader
     */
    explicit Block(size_t qv_count, util::BitReader &reader);

    /**
     *
     */
    virtual ~Block() = default;

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;

    /**
     *
     * @return
     */
    core::AccessUnit::Descriptor &&movePayload();

    /**
     *
     * @return
     */
    uint8_t getDescriptorID() const;

    /**
     *
     * @return
     */
    size_t getWrittenSize() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BLOCK_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------