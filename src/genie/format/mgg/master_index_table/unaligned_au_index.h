/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright
 * This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 * @brief Defines the `UnalignedAUIndex` class, which represents an unaligned
 * Access Unit (AU) index in the master index table.
 *
 * The `UnalignedAUIndex` class encapsulates metadata related to unaligned
 * Access Units in an MPEG-G file. It includes information about byte offsets,
 * reference configurations, signature configurations, and block-level offsets.
 * This class is used within the master index table to facilitate efficient
 * navigation and retrieval of unaligned Access Units.
 */

#ifndef SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_UNALIGNED_AU_INDEX_H_
#define SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_UNALIGNED_AU_INDEX_H_

// -----------------------------------------------------------------------------

#include <optional>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mgb/signature_cfg.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::master_index_table {

/**
 * @brief Represents the index of an unaligned Access Unit (AU) in the master
 * index table.
 *
 * The `UnalignedAUIndex` class stores the position and metadata for an
 * unaligned AU, including its byte offset, reference configuration, signature
 * configuration, and block-level offsets. It supports serialization and
 * deserialization, allowing it to be read from and written to MPEG-G files.
 */
class UnalignedAuIndex {
  uint64_t au_byte_offset_;  //!< @brief The byte offset of the unaligned AU in
                             //!< the file.
  std::optional<mgb::RefCfg>
      ref_cfg_;  //!< @brief Optional reference configuration data.
  std::optional<genie::format::mgb::SignatureCfg>
      sig_cfg_;  //!< @brief Optional signature configuration data.
  std::vector<uint64_t>
      block_byte_offset_;  //!< @brief Byte offsets for blocks within the AU.

  uint8_t byte_offset_size_;  //!< @brief Size of byte offset fields in bits.
  uint8_t position_size_;     //!< @brief Size of position fields in bits.
  uint8_t signature_size_;    //!< @brief Size of signature fields in bits.

 public:
  /**
   * @brief Checks if two `UnalignedAUIndex` objects are equal.
   * @param other The `UnalignedAUIndex` to compare against.
   * @return `true` if the two objects are equal, `false` otherwise.
   */
  bool operator==(const UnalignedAuIndex& other) const;

  /**
   * @brief Gets the block-level byte offsets within the AU.
   * @return A vector of byte offsets for blocks within the AU.
   */
  [[nodiscard]] const std::vector<uint64_t>& GetBlockOffsets() const;

  /**
   * @brief Constructs an `UnalignedAUIndex` by reading from a bitstream.
   *
   * This constructor reads an `UnalignedAUIndex` from a `BitReader`, using the
   * provided parameters to control the deserialization process.
   *
   * @param reader The bitstream reader to read the index data from.
   * @param byte_offset_size The Size of byte offset fields in bits.
   * @param position_size The Size of position fields in bits.
   * @param dataset_type The type of the dataset containing this AU.
   * @param signature_flag Indicates if a signature is present.
   * @param signature_const_flag Indicates if the signature is constant.
   * @param signature_size The Size of the signature fields in bits.
   * @param block_header_flag Indicates if block headers are present.
   * @param descriptors A vector of descriptors for the AU.
   * @param alphabet The alphabet ID for the data.
   */
  explicit UnalignedAuIndex(util::BitReader& reader, uint8_t byte_offset_size,
                            uint8_t position_size,
                            core::parameter::DataUnit::DatasetType dataset_type,
                            bool signature_flag, bool signature_const_flag,
                            uint8_t signature_size, bool block_header_flag,
                            const std::vector<core::GenDesc>& descriptors,
                            core::AlphabetId alphabet);

  /**
   * @brief Serializes the `UnalignedAUIndex` to a bitstream.
   * @param writer The bitstream writer to serialize the index data to.
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief Gets the byte offset of the AU in the file.
   * @return The byte offset of the AU.
   */
  [[nodiscard]] uint64_t GetAuOffset() const;

  /**
   * @brief Adds a byte offset for a block within the AU.
   * @param offset The byte offset to add.
   */
  void AddBlockOffset(uint64_t offset);

  /**
   * @brief Constructs an `UnalignedAUIndex` with specified parameters.
   *
   * @param au_byte_offset The byte offset of the AU in the file.
   * @param byte_offset_size The Size of byte offset fields in bits.
   * @param position_size The Size of position fields in bits.
   * @param signature_size The Size of signature fields in bits.
   */
  UnalignedAuIndex(uint64_t au_byte_offset, uint8_t byte_offset_size,
                   int8_t position_size, int8_t signature_size);

  /**
   * @brief Checks if the AU has a reference configuration.
   * @return `true` if a reference configuration is present, `false` otherwise.
   */
  [[nodiscard]] bool IsReference() const;

  /**
   * @brief Gets the reference configuration for the AU.
   * @return The reference configuration.
   * @throws `std::runtime_error` if no reference configuration is present.
   */
  [[nodiscard]] const mgb::RefCfg& GetReferenceInfo() const;

  /**
   * @brief Sets the reference configuration for the AU.
   * @param ref_cfg The reference configuration to set.
   */
  void SetReferenceInfo(const mgb::RefCfg& ref_cfg);

  /**
   * @brief Checks if the AU has a signature configuration.
   * @return `true` if a signature configuration is present, `false` otherwise.
   */
  [[nodiscard]] bool HasSignature() const;

  /**
   * @brief Gets the signature configuration for the AU.
   * @return The signature configuration.
   * @throws `std::runtime_error` if no signature configuration is present.
   */
  [[nodiscard]] const mgb::SignatureCfg& GetSignatureInfo() const;

  /**
   * @brief Sets the signature configuration for the AU.
   * @param signature_cfg The signature configuration to set.
   */
  void SetSignatureInfo(const mgb::SignatureCfg& signature_cfg);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::master_index_table

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_UNALIGNED_AU_INDEX_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
