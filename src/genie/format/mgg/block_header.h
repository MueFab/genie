/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MGG_BLOCK_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_BLOCK_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/constants.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

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
    [[nodiscard]] bool getReserved1() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getReserved2() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] genie::core::GenDesc getDescriptorID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint32_t getPayloadSize() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] static uint64_t getLength();

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_BLOCK_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
