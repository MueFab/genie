/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `MasterIndexTable` class, which stores indexing
 * information for MPEG-G data units.
 *
 * The `MasterIndexTable` class represents an indexing table for aligned and
 * unaligned Access Units (AUs) within an MPEG-G dataset. It allows efficient
 * access to descriptor streams and data units, making it easier to locate
 * specific data blocks within a compressed genomic dataset.
 *
 * @copyright
 * This file is part of Genie.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_H_
#define SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_H_

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/core/constants.h"
#include "genie/format/mgg/dataset_header.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/master_index_table/aligned_au_index.h"
#include "genie/format/mgg/master_index_table/unaligned_au_index.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @class MasterIndexTable
 * @brief Represents the master index table for MPEG-G data units.
 *
 * The `MasterIndexTable` class provides an indexing mechanism for both aligned
 * and unaligned Access Units (AUs) within a given dataset. It keeps track of
 * the positions of descriptor streams and individual AUs, enabling fast and
 * efficient random access to specific portions of a genomic dataset.
 */
class MasterIndexTable final : public GenInfo {
  std::vector<std::vector<std::vector<master_index_table::AlignedAuIndex>>>
      aligned_aus_;  //!< @brief Aligned AUs.
  std::vector<master_index_table::UnalignedAuIndex>
      unaligned_aus_;  //!< @brief Unaligned AUs.

 public:
  /**
   * @brief Constructs an empty MasterIndexTable with the specified
   * configuration.
   *
   * Initializes an empty master index table with a given number of sequence
   * entries and data classes. This constructor is used when building a new
   * master index table from scratch.
   *
   * @param seq_count Number of sequences in the dataset.
   * @param num_classes Number of descriptor classes.
   */
  MasterIndexTable(uint16_t seq_count, uint8_t num_classes);

  /**
   * @brief Constructs a `MasterIndexTable` by reading from a bitstream.
   *
   * This constructor reads and parses a master index table from the specified
   * bitstream. It uses the `DatasetHeader` to determine the configuration of
   * the index table.
   *
   * @param reader Bitstream reader to parse the data from.
   * @param hdr Reference to the dataset header containing configuration
   * details.
   */
  MasterIndexTable(util::BitReader& reader, const DatasetHeader& hdr);

  /**
   * @brief Retrieves descriptor stream offsets for a given class and descriptor
   * index.
   *
   * Returns the offsets of descriptor streams based on the specified class
   * index, descriptor index, and whether the Access Units are aligned or
   * unaligned. It uses the `total_size` parameter to adjust offsets.
   *
   * @param class_index Index of the data class.
   * @param desc_index Index of the descriptor.
   * @param is_unaligned Boolean indicating whether to retrieve unaligned AU
   * offsets.
   * @param total_size Total Size of the descriptor streams.
   * @return Vector of 64-bit unsigned offsets corresponding to the descriptor
   * streams.
   */
  [[nodiscard]] std::vector<uint64_t> GetDescriptorStreamOffsets(
      uint8_t class_index, uint8_t desc_index, bool is_unaligned,
      uint64_t total_size) const;

  /**
   * @brief Checks equality between two `MasterIndexTable` objects.
   *
   * Compares the contents of this `MasterIndexTable` with another `GenInfo`
   * object to check for equality. Equality is determined based on the index
   * structure and contents.
   *
   * @param info Reference to the other `GenInfo` object for comparison.
   * @return `true` if the objects are equal, `false` otherwise.
   */
  bool operator==(const GenInfo& info) const override;

  /**
   * @brief Retrieves all aligned Access Units.
   *
   * Provides access to the internal vector of aligned Access Unit indices,
   * organized by sequences and classes.
   *
   * @return Constant reference to the nested vector of aligned AU indices.
   */
  [[nodiscard]] const std::vector<
      std::vector<std::vector<master_index_table::AlignedAuIndex>>>&
  GetAlignedAUs() const;

  /**
   * @brief Retrieves all unaligned Access Units.
   *
   * Provides access to the internal vector of unaligned Access Unit indices.
   *
   * @return Constant reference to the vector of unaligned AU indices.
   */
  [[nodiscard]] const std::vector<master_index_table::UnalignedAuIndex>&
  GetUnalignedAUs() const;

  /**
   * @brief Serializes the `MasterIndexTable` to a bitstream.
   *
   * Writes the internal representation of the master index table to the
   * specified bitstream writer. This method is used when exporting the
   * `MasterIndexTable` to an MPEG-G compatible format.
   *
   * @param bit_writer Reference to the bitstream writer used for serialization.
   */
  void BoxWrite(util::BitWriter& bit_writer) const override;

  /**
   * @brief Retrieves the key identifying this `MasterIndexTable`.
   *
   * The key uniquely identifies the type and purpose of this `GenInfo` object.
   *
   * @return Constant reference to the key string.
   */
  [[nodiscard]] const std::string& GetKey() const override;

  /**
   * @brief Prints the structure of the master index table for debugging.
   *
   * Outputs a hierarchical representation of the master index table, including
   * the aligned and unaligned Access Units and their respective indices. Useful
   * for debugging and analyzing the index table structure.
   *
   * @param output Reference to the output stream for printing.
   * @param depth Current depth level in the hierarchy (used for indentation).
   * @param max_depth Maximum depth for recursive printing.
   */
  void PrintDebug(std::ostream& output, uint8_t depth,
                  uint8_t max_depth) const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
