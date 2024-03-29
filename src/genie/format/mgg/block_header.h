/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MGG_BLOCK_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_BLOCK_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

/**
 * @brief
 */
class BlockHeader {
 private:
    bool reserved1;                      //!< @brief
    genie::core::GenDesc descriptor_ID;  //!< @brief
    uint8_t reserved2;                   //!< @brief
    uint32_t block_payload_size;         //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const BlockHeader& other) const;

    /**
     * @brief
     * @param reader
     */
    explicit BlockHeader(util::BitReader& reader);

    /**
     * @brief
     * @param _reserved1
     * @param _desc_id
     * @param _reserved2
     * @param payload_size
     */
    BlockHeader(bool _reserved1, genie::core::GenDesc _desc_id, uint8_t _reserved2, uint32_t payload_size);

    /**
     * @brief
     * @return
     */
    bool getReserved1() const;

    /**
     * @brief
     * @return
     */
    uint8_t getReserved2() const;

    /**
     * @brief
     * @return
     */
    genie::core::GenDesc getDescriptorID() const;

    /**
     * @brief
     * @return
     */
    uint32_t getPayloadSize() const;

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_BLOCK_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
