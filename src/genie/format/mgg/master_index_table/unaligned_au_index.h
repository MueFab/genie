/**
 * @file
 * @copyright
 * This file is part of GENIE. See LICENSE and/or https://github.com/MueFab/genie for more details.
 * @brief Defines the `UnalignedAUIndex` class, which represents an unaligned Access Unit (AU) index in the master index
 * table.
 *
 * The `UnalignedAUIndex` class encapsulates metadata related to unaligned Access Units in an MPEG-G file. It includes
 * information about byte offsets, reference configurations, signature configurations, and block-level offsets. This
 * class is used within the master index table to facilitate efficient navigation and retrieval of unaligned Access
 * Units.
 */

#ifndef SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_UNALIGNED_AU_INDEX_H_
#define SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_UNALIGNED_AU_INDEX_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mgb/signature_cfg.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::master_index_table {

/**
 * @brief Represents the index of an unaligned Access Unit (AU) in the master index table.
 *
 * The `UnalignedAUIndex` class stores the position and metadata for an unaligned AU, including its byte offset,
 * reference configuration, signature configuration, and block-level offsets. It supports serialization and
 * deserialization, allowing it to be read from and written to MPEG-G files.
 */
class UnalignedAUIndex {
 private:
    uint64_t au_byte_offset;                            //!< @brief The byte offset of the unaligned AU in the file.
    std::optional<genie::format::mgb::RefCfg> ref_cfg;  //!< @brief Optional reference configuration data.
    std::optional<genie::format::mgb::SignatureCfg> sig_cfg;  //!< @brief Optional signature configuration data.
    std::vector<uint64_t> block_byte_offset;                  //!< @brief Byte offsets for blocks within the AU.

    uint8_t byte_offset_size;  //!< @brief Size of byte offset fields in bits.
    uint8_t position_size;     //!< @brief Size of position fields in bits.
    uint8_t signature_size;    //!< @brief Size of signature fields in bits.

 public:
    /**
     * @brief Checks if two `UnalignedAUIndex` objects are equal.
     * @param other The `UnalignedAUIndex` to compare against.
     * @return `true` if the two objects are equal, `false` otherwise.
     */
    bool operator==(const UnalignedAUIndex& other) const;

    /**
     * @brief Gets the block-level byte offsets within the AU.
     * @return A vector of byte offsets for blocks within the AU.
     */
    [[nodiscard]] const std::vector<uint64_t>& getBlockOffsets() const;

    /**
     * @brief Constructs an `UnalignedAUIndex` by reading from a bitstream.
     *
     * This constructor reads an `UnalignedAUIndex` from a `BitReader`, using the provided parameters to control
     * the deserialization process.
     *
     * @param reader The bitstream reader to read the index data from.
     * @param _byte_offset_size The size of byte offset fields in bits.
     * @param _position_size The size of position fields in bits.
     * @param dataset_type The type of the dataset containing this AU.
     * @param signature_flag Indicates if a signature is present.
     * @param signature_const_flag Indicates if the signature is constant.
     * @param _signature_size The size of the signature fields in bits.
     * @param block_header_flag Indicates if block headers are present.
     * @param descriptors A vector of descriptors for the AU.
     * @param alphabet The alphabet ID for the data.
     */
    explicit UnalignedAUIndex(util::BitReader& reader, uint8_t _byte_offset_size, uint8_t _position_size,
                              core::parameter::DataUnit::DatasetType dataset_type, bool signature_flag,
                              bool signature_const_flag, uint8_t _signature_size, bool block_header_flag,
                              const std::vector<genie::core::GenDesc>& descriptors, core::AlphabetID alphabet);

    /**
     * @brief Serializes the `UnalignedAUIndex` to a bitstream.
     * @param writer The bitstream writer to serialize the index data to.
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief Gets the byte offset of the AU in the file.
     * @return The byte offset of the AU.
     */
    [[nodiscard]] uint64_t getAUOffset() const;

    /**
     * @brief Adds a byte offset for a block within the AU.
     * @param offset The byte offset to add.
     */
    void addBlockOffset(uint64_t offset);

    /**
     * @brief Constructs an `UnalignedAUIndex` with specified parameters.
     *
     * @param _au_byte_offset The byte offset of the AU in the file.
     * @param _byte_offset_size The size of byte offset fields in bits.
     * @param _position_size The size of position fields in bits.
     * @param _signature_size The size of signature fields in bits.
     */
    UnalignedAUIndex(uint64_t _au_byte_offset, uint8_t _byte_offset_size, int8_t _position_size,
                     int8_t _signature_size);

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
     * @brief Checks if the AU has a signature configuration.
     * @return `true` if a signature configuration is present, `false` otherwise.
     */
    [[nodiscard]] bool hasSignature() const;

    /**
     * @brief Gets the signature configuration for the AU.
     * @return The signature configuration.
     * @throws `std::runtime_error` if no signature configuration is present.
     */
    [[nodiscard]] const genie::format::mgb::SignatureCfg& getSignatureInfo() const;

    /**
     * @brief Sets the signature configuration for the AU.
     * @param sigcfg The signature configuration to set.
     */
    void setSignatureInfo(const genie::format::mgb::SignatureCfg& sigcfg);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::master_index_table

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_UNALIGNED_AU_INDEX_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
