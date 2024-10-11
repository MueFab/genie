/**
 * @file
 * @brief Defines the `BlockHeader` class for managing block headers in the MPEG-G format.
 * @details The `BlockHeader` class represents a header structure used to store metadata about data blocks within an MPEG-G file.
 *          This includes information about the block type, payload size, and reserved fields.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
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
 * @brief Class representing a block header in the MPEG-G format.
 * @details Stores and handles metadata for a data block, including a descriptor ID, payload size, and reserved fields.
 */
class BlockHeader {
 private:
    bool reserved1;                      //!< @brief First reserved field (1 bit)
    genie::core::GenDesc descriptor_ID;  //!< @brief Descriptor identifier for the block type
    uint8_t reserved2;                   //!< @brief Second reserved field (7 bits)
    uint32_t block_payload_size;         //!< @brief Size of the payload in bytes

 public:
    /**
     * @brief Equality operator for comparing two `BlockHeader` objects.
     * @param other Reference to another `BlockHeader` object.
     * @return True if all fields of both headers match, otherwise false.
     */
    bool operator==(const BlockHeader& other) const;

    /**
     * @brief Constructs a `BlockHeader` object by reading from a `BitReader` stream.
     * @param reader The `BitReader` object to extract data from.
     */
    explicit BlockHeader(util::BitReader& reader);

    /**
     * @brief Constructs a `BlockHeader` object with specified parameters.
     * @param _reserved1 First reserved field.
     * @param _desc_id Descriptor identifier.
     * @param _reserved2 Second reserved field.
     * @param payload_size Size of the block payload in bytes.
     */
    BlockHeader(bool _reserved1, genie::core::GenDesc _desc_id, uint8_t _reserved2, uint32_t payload_size);

    /**
     * @brief Retrieves the value of the first reserved field.
     * @return True if the field is set, otherwise false.
     */
    [[nodiscard]] bool getReserved1() const;

    /**
     * @brief Retrieves the value of the second reserved field.
     * @return The value of the second reserved field.
     */
    [[nodiscard]] uint8_t getReserved2() const;

    /**
     * @brief Retrieves the descriptor ID for this block.
     * @return Descriptor ID as a `GenDesc` enum value.
     */
    [[nodiscard]] genie::core::GenDesc getDescriptorID() const;

    /**
     * @brief Retrieves the size of the block payload.
     * @return The size of the payload in bytes.
     */
    [[nodiscard]] uint32_t getPayloadSize() const;

    /**
     * @brief Gets the length of the block header.
     * @details Returns the fixed size of the block header in bits.
     * @return Length of the block header in bits.
     */
    [[nodiscard]] static uint64_t getLength();

    /**
     * @brief Writes the block header data to a `BitWriter` stream.
     * @param writer Reference to the `BitWriter` object.
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_BLOCK_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
