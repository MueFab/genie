/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `AlignedAUIndex` class, which represents an aligned Access
 * Unit (AU) index within the master index table.
 *
 * The `AlignedAUIndex` class is responsible for storing information about an
 * aligned AU, including its byte offset, start and end positions, reference
 * configurations, and other metadata. This class is used within the master
 * index table to efficiently access and navigate through aligned AUs in MPEG-G
 * datasets.
 *
 * @copyright
 * This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_ALIGNED_AU_INDEX_H_
#define SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_ALIGNED_AU_INDEX_H_

// -----------------------------------------------------------------------------

#include <optional>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/extended_au.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::master_index_table {

/**
 * @brief Represents the index of an aligned Access Unit (AU) in the master
 * index table.
 *
 * The `AlignedAUIndex` class encapsulates various properties of an aligned AU,
 * such as its byte offset in the file, its genomic start and end positions,
 * reference configurations, and extended alignment configurations. It also
 * supports serialization and deserialization from a bitstream, enabling it to
 * be written to and read from MPEG-G files.
 */
class AlignedAuIndex {
  uint64_t au_byte_offset_;  //!< @brief The byte offset of the AU in the file.
  uint64_t
      au_start_position_;     //!< @brief The start genomic position of the AU.
  uint64_t au_end_position_;  //!< @brief The end genomic position of the AU.
  std::optional<mgb::RefCfg>
      ref_cfg_;  //!< @brief Optional reference configuration data.
  std::optional<mgb::ExtendedAu>
      extended_cfg_;  //!< @brief Optional extended alignment data.
  std::vector<uint64_t>
      block_byte_offset_;  //!< @brief Byte offsets for blocks within the AU.

  uint8_t byte_offset_size_;  //!< @brief Size of byte offset fields in bits.
  uint8_t position_size_;     //!< @brief Size of position fields in bits.

 public:
  /**
   * @brief Checks if two `AlignedAUIndex` objects are equal.
   * @param other The `AlignedAUIndex` to compare against.
   * @return `true` if the two objects are equal, `false` otherwise.
   */
  bool operator==(const AlignedAuIndex& other) const;

  /**
   * @brief Constructs an `AlignedAUIndex` with specified parameters.
   *
   * @param au_byte_offset The byte offset of the AU in the file.
   * @param au_start_position The genomic start position of the AU.
   * @param au_end_position The genomic end position of the AU.
   * @param byte_offset_size The Size of byte offset fields in bits.
   * @param position_size The Size of position fields in bits.
   */
  AlignedAuIndex(uint64_t au_byte_offset, uint64_t au_start_position,
                 uint64_t au_end_position, uint8_t byte_offset_size,
                 uint8_t position_size);

  /**
   * @brief Deserializes an `AlignedAUIndex` from a bitstream.
   *
   * This constructor reads an `AlignedAUIndex` object from the provided
   * bitstream reader, using additional parameters to control the
   * deserialization process.
   *
   * @param reader The bitstream reader to read the index data from.
   * @param byte_offset_size The Size of byte offset fields in bits.
   * @param position_size The Size of position fields in bits.
   * @param dataset_type The type of the dataset containing this AU.
   * @param multiple_alignment Indicates if multiple alignments are present.
   * @param block_header_flag Indicates if block headers are present.
   * @param descriptors A vector of descriptors for the AU.
   */
  AlignedAuIndex(util::BitReader& reader, uint8_t byte_offset_size,
                 uint8_t position_size,
                 core::parameter::DataUnit::DatasetType dataset_type,
                 bool multiple_alignment, bool block_header_flag,
                 const std::vector<core::GenDesc>& descriptors);

  /**
   * @brief Serializes the `AlignedAUIndex` to a bitstream.
   * @param writer The bitstream writer to serialize the index data to.
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief Gets the byte offset of the AU in the file.
   * @return The byte offset of the AU.
   */
  [[nodiscard]] uint64_t GetByteOffset() const;

  /**
   * @brief Gets the genomic start position of the AU.
   * @return The start position of the AU.
   */
  [[nodiscard]] uint64_t GetAuStartPos() const;

  /**
   * @brief Gets the genomic end position of the AU.
   * @return The end position of the AU.
   */
  [[nodiscard]] uint64_t GetAuEndPos() const;

  /**
   * @brief Checks if the AU has a reference configuration.
   * @return `true` if a reference configuration is present, `false`
   * otherwise.
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
   * @brief Sets the extended alignment configuration for the AU.
   * @param ext_au The extended alignment configuration to set.
   */
  void SetExtended(const mgb::ExtendedAu& ext_au);

  /**
   * @brief Gets the extended alignment configuration for the AU.
   * @return The extended alignment configuration.
   * @throws `std::runtime_error` if no extended configuration is present.
   */
  [[nodiscard]] const mgb::ExtendedAu& GetExtension() const;

  /**
   * @brief Checks if the AU has an extended configuration.
   * @return `true` if an extended configuration is present, `false`
   * otherwise.
   */
  [[nodiscard]] bool IsExtended() const;

  /**
   * @brief Gets the block byte offsets within the AU.
   * @return A vector of byte offsets for blocks within the AU.
   */
  [[nodiscard]] const std::vector<uint64_t>& GetBlockOffsets() const;

  /**
   * @brief Adds a byte offset for a block within the AU.
   * @param offset The byte offset to add.
   */
  void AddBlockOffset(uint64_t offset);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::master_index_table

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_ALIGNED_AU_INDEX_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
