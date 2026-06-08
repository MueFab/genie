/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief This file defines the `Dataset` class, which represents a dataset in
 * MPEG-G format.
 * @details The `Dataset` class encapsulates various components such as
 * metadata, protection, parameter sets, descriptor streams, and access units.
 * It supports serialization and deserialization, along with several utility
 * methods for modifying and retrieving dataset information. This class is
 * integral for managing dataset structure and ensuring compatibility with the
 * MPEG-G standard.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_H_

// -----------------------------------------------------------------------------

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "genie/core/meta/block_header/disabled.h"
#include "genie/core/meta/dataset.h"
#include "genie/format/mgb/mgb_file.h"
#include "genie/format/mgg/access_unit.h"
#include "genie/format/mgg/dataset_header.h"
#include "genie/format/mgg/dataset_metadata.h"
#include "genie/format/mgg/dataset_parameter_set.h"
#include "genie/format/mgg/dataset_protection.h"
#include "genie/format/mgg/descriptor_stream.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/master_index_table.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

#define Genie_DEBUG_PRINT_NODETAIL

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing a dataset in MPEG-G format.
 * @details The `Dataset` class encapsulates all the data related to a dataset,
 * such as metadata, protection, parameter sets, and descriptor streams. It
 * allows for modifying and accessing these components, as well as serialization
 * and deserialization to and from bitstreams.
 */
class Dataset final : public GenInfo {
  DatasetHeader
      header_;  //!< @brief Dataset header containing core information.
  std::optional<DatasetMetadata>
      metadata_;  //!< @brief Optional dataset metadata.
  std::optional<DatasetProtection>
      protection_;  //!< @brief Optional dataset protection information.
  std::vector<DatasetParameterSet>
      parameter_sets_;  //!< @brief List of parameter sets used in the dataset.
  std::optional<MasterIndexTable>
      master_index_table_;  //!< @brief Optional master index table for the
                            //!< dataset.
  std::vector<AccessUnit>
      access_units_;  //!< @brief List of access units within the dataset.
  std::vector<DescriptorStream>
      descriptor_streams_;  //!< @brief List of descriptor streams.
  core::MpegMinorVersion
      version_;  //!< @brief MPEG minor version for the dataset.

  std::map<size_t, core::parameter::EncodingSet>
      encoding_sets_;  //!< @brief Map of encoding sets based on parameter IDs.

 public:
  /**
   * @brief Checks if the dataset has metadata.
   * @return True if metadata is present; otherwise, false.
   */
  [[nodiscard]] bool HasMetadata() const;

  /**
   * @brief Checks if the dataset has protection information.
   * @return True if protection is present; otherwise, false.
   */
  [[nodiscard]] bool HasProtection() const;

  /**
   * @brief Retrieves the metadata object.
   * @return Reference to the `DatasetMetadata` object.
   */
  DatasetMetadata& GetMetadata();

  /**
   * @brief Retrieves the protection object.
   * @return Reference to the `DatasetProtection` object.
   */
  DatasetProtection& GetProtection();

  /**
   * @brief Retrieves the list of access units.
   * @return Reference to the vector of access units.
   */
  std::vector<AccessUnit>& GetAccessUnits();

  /**
   * @brief Retrieves the list of descriptor streams.
   * @return Reference to the vector of descriptor streams.
   */
  std::vector<DescriptorStream>& GetDescriptorStreams();

  /**
   * @brief Retrieves the list of parameter sets.
   * @return Reference to the vector of parameter sets.
   */
  std::vector<DatasetParameterSet>& GetParameterSets();

  /**
   * @brief Constructs a `Dataset` object by reading from a bit reader.
   * @param reader Bit reader to extract dataset data from.
   * @param version MPEG minor version to use for the dataset.
   */
  Dataset(util::BitReader& reader, core::MpegMinorVersion version);

  /**
   * @brief Constructs a `Dataset` object from an existing `MgbFile` and
   * metadata.
   * @param file Reference to the `MgbFile` object.
   * @param meta Metadata of the dataset.
   * @param version MPEG minor version to use for the dataset.
   * @param param_ids List of parameter set IDs.
   */
  Dataset(mgb::MgbFile& file, core::meta::Dataset& meta,
          core::MpegMinorVersion version,
          const std::vector<uint8_t>& param_ids);

  /**
   * @brief Patches the group ID and set ID within the dataset.
   * @param group_id New group ID.
   * @param set_id New dataset ID.
   */
  void PatchId(uint8_t group_id, uint16_t set_id);

  /**
   * @brief Patches the reference ID within the dataset.
   * @param old Old reference ID to replace.
   * @param _new New reference ID to set.
   */
  void PatchRefId(uint8_t old, uint8_t _new);

  /**
   * @brief Retrieves the header of the dataset.
   * @return Constant reference to the `DatasetHeader` object.
   */
  [[nodiscard]] const DatasetHeader& GetHeader() const;

  /**
   * @brief Retrieves the header of the dataset.
   * @return Reference to the `DatasetHeader` object.
   */
  DatasetHeader& GetHeader();

  /**
   * @brief Serializes the dataset into a bit writer.
   * @param bit_writer Bit writer to write the dataset to.
   */
  void BoxWrite(util::BitWriter& bit_writer) const override;

  /**
   * @brief Retrieves the unique key for the dataset.
   * @return The key as a string.
   */
  [[nodiscard]] const std::string& GetKey() const override;

  /**
   * @brief Reads and initializes the dataset from a bit reader.
   * @param reader Bit reader to read the dataset from.
   * @param in_offset Indicates whether to account for bit offsets.
   */
  void ReadBox(util::BitReader& reader, bool in_offset);

  /**
   * @brief Prints detailed debugging information for the dataset.
   * @param output Output stream to print to.
   * @param depth Current depth of the debug output.
   * @param max_depth Maximum depth to display.
   */
  void PrintDebug(std::ostream& output, uint8_t depth,
                   uint8_t max_depth) const override;
};

}  // namespace genie::format::mgg

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
