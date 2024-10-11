/**
 * @file
 * @brief Defines the `AlignedAUIndex` class, which represents an aligned Access Unit (AU) index within the master index table.
 *
 * The `AlignedAUIndex` class is responsible for storing information about an aligned AU, including its byte offset, start
 * and end positions, reference configurations, and other metadata. This class is used within the master index table to
 * efficiently access and navigate through aligned AUs in MPEG-G datasets.
 *
 * @copyright
 * This file is part of GENIE. See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_ALIGNED_AU_INDEX_H_
#define SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_ALIGNED_AU_INDEX_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/extended_au.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::master_index_table {

/**
 * @brief Represents the index of an aligned Access Unit (AU) in the master index table.
 *
 * The `AlignedAUIndex` class encapsulates various properties of an aligned AU, such as its byte offset in the file,
 * its genomic start and end positions, reference configurations, and extended alignment configurations. It also
 * supports serialization and deserialization from a bitstream, enabling it to be written to and read from MPEG-G files.
 */
class AlignedAUIndex {
 private:
    uint64_t au_byte_offset;                                     //!< @brief The byte offset of the AU in the file.
    uint64_t au_start_position;                                  //!< @brief The start genomic position of the AU.
    uint64_t au_end_position;                                    //!< @brief The end genomic position of the AU.
    std::optional<genie::format::mgb::RefCfg> ref_cfg;           //!< @brief Optional reference configuration data.
    std::optional<genie::format::mgb::ExtendedAu> extended_cfg;  //!< @brief Optional extended alignment data.
    std::vector<uint64_t> block_byte_offset;                     //!< @brief Byte offsets for blocks within the AU.

    uint8_t byte_offset_size;  //!< @brief Size of byte offset fields in bits.
    uint8_t position_size;     //!< @brief Size of position fields in bits.

 public:
    /**
     * @brief Checks if two `AlignedAUIndex` objects are equal.
     * @param other The `AlignedAUIndex` to compare against.
     * @return `true` if the two objects are equal, `false` otherwise.
     */
    bool operator==(const AlignedAUIndex& other) const;

    /**
     * @brief Constructs an `AlignedAUIndex` with specified parameters.
     *
     * @param _au_byte_offset The byte offset of the AU in the file.
     * @param _au_start_position The genomic start position of the AU.
     * @param _au_end_position The genomic end position of the AU.
     * @param _byte_offset_size The size of byte offset fields in bits.
     * @param _position_size The size of position fields in bits.
     */
    AlignedAUIndex(uint64_t _au_byte_offset, uint64_t _au_start_position, uint64_t _au_end_position,
                   uint8_t _byte_offset_size, uint8_t _position_size);

    /**
     * @brief Deserializes an `AlignedAUIndex` from a bitstream.
     *
     * This constructor reads an `AlignedAUIndex` object from the provided bitstream reader, using additional
     * parameters to control the deserialization process.
     *
     * @param reader The bitstream reader to read the index data from.
     * @param _byte_offset_size The size of byte offset fields in bits.
     * @param _position_size The size of position fields in bits.
     * @param dataset_type The type of the dataset containing this AU.
     * @param multiple_alignment Indicates if multiple alignments are present.
     * @param block_header_flag Indicates if block headers are present.
     * @param descriptors A vector of descriptors for the AU.
     */
    AlignedAUIndex(genie::util::BitReader& reader, uint8_t _byte_offset_size, uint8_t _position_size,
                   core::parameter::DataUnit::DatasetType dataset_type, bool multiple_alignment, bool block_header_flag,
                   const std::vector<genie::core::GenDesc>& descriptors);

    /**
     * @brief Serializes the `AlignedAUIndex` to a bitstream.
     * @param writer The bitstream writer to serialize the index data to.
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief Gets the byte offset of the AU in the file.
     * @return The byte offset of the AU.
     */
    [[nodiscard]] uint64_t getByteOffset() const;

    /**
     * @brief Gets the genomic start position of the AU.
     * @return The start position of the AU.
     */
    [[nodiscard]] uint64_t getAUStartPos() const;

    /**
     * @brief Gets the genomic end position of the AU.
     * @return The end position of the AU.
     */
    [[nodiscard]] uint64_t getAUEndPos() const;

    /**
     * @brief Checks if the AU has a reference configuration.
     * @return `true` if a reference configuration is present, `false` otherwise.
     */
    [[nodiscard]] bool isReference() const;

    /**
     * @brief Gets the reference configuration for the AU.
     * @return The reference configuration.
     * @throws `std::runtime_error` if no reference configuration is present.
     */
    [[nodiscard]] const genie::format::mgb::RefCfg& getReferenceInfo() const;

    /**
     * @brief Sets the reference configuration for the AU.
     * @param _ref_cfg The reference configuration to set.
     */
    void setReferenceInfo(const genie::format::mgb::RefCfg& _ref_cfg);

    /**
     * @brief Sets the extended alignment configuration for the AU.
     * @param _ext_au The extended alignment configuration to set.
     */
    void setExtended(const genie::format::mgb::ExtendedAu& _ext_au);

    /**
     * @brief Gets the extended alignment configuration for the AU.
     * @return The extended alignment configuration.
     * @throws `std::runtime_error` if no extended configuration is present.
     */
    [[nodiscard]] const genie::format::mgb::ExtendedAu& getExtension() const;

    /**
     * @brief Checks if the AU has an extended configuration.
     * @return `true` if an extended configuration is present, `false` otherwise.
     */
    [[nodiscard]] bool isExtended() const;

    /**
     * @brief Gets the block byte offsets within the AU.
     * @return A vector of byte offsets for blocks within the AU.
     */
    [[nodiscard]] const std::vector<uint64_t>& getBlockOffsets() const;

    /**
     * @brief Adds a byte offset for a block within the AU.
     * @param offset The byte offset to add.
     */
    void addBlockOffset(uint64_t offset);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::master_index_table

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_ALIGNED_AU_INDEX_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
