/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `EncapsulatedFile` structure for managing multiple MPEG-G
 * dataset groups.
 *
 * The `EncapsulatedFile` class is a core component of the MPEG-G encapsulation
 * module. It handles the encapsulation of a collection of dataset groups,
 * providing methods for grouping input files, managing encapsulated dataset
 * groups, and assembling a complete MPEG-G file.
 *
 * @copyright
 * This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_FILE_H_
#define SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_FILE_H_

// -----------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>

#include "genie/format/mgg/encapsulator/encapsulated_dataset_group.h"
#include "genie/format/mgg/mgg_file.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

/**
 * @brief Manages a collection of encapsulated MPEG-G dataset groups.
 *
 * The `EncapsulatedFile` class is responsible for encapsulating multiple
 * dataset groups, each represented by an `EncapsulatedDatasetGroup` object. It
 * provides functionality to organize input files into groups based on common
 * properties, encapsulate them, and assemble a final MPEG-G file (`MggFile`)
 * that contains all the groups.
 */
struct EncapsulatedFile {
  std::vector<EncapsulatedDatasetGroup>
      groups;  //!< @brief Collection of encapsulated dataset groups.

  /**
   * @brief Groups input files by their respective group identifiers.
   *
   * This function takes a list of input files and categorizes them into groups
   * based on common group identifiers. The resulting map associates each group
   * identifier with a list of input files that belong to that group.
   *
   * @param input_files A vector of file paths to group.
   * @return A map where each key is a group identifier (`uint8_t`) and each
   * value is a vector of input file paths.
   */
  [[nodiscard]] static std::map<uint8_t, std::vector<std::string>>
  GroupInputFiles(const std::vector<std::string>& input_files);

  /**
   * @brief Constructs an `EncapsulatedFile` from a collection of input files.
   *
   * This constructor reads the specified input files, organizes them into
   * groups, and encapsulates them into `EncapsulatedDatasetGroup` objects. Each
   * group is managed separately, allowing for flexible manipulation and
   * assembly into a final MPEG-G file.
   *
   * @param input_files A vector of file paths representing the input datasets
   * to be encapsulated.
   * @param version The MPEG-G minor version for the encapsulated format.
   * @throws `std::runtime_error` if any file cannot be read or if the group
   * organization fails.
   */
  EncapsulatedFile(const std::vector<std::string>& input_files,
                   genie::core::MpegMinorVersion version);

  /**
   * @brief Assembles all encapsulated dataset groups into a complete MPEG-G
   * file.
   *
   * This function combines the encapsulated dataset groups into a single
   * `MggFile` object, ready for writing to a file or further processing. The
   * resulting MPEG-G file contains all dataset groups and their metadata.
   *
   * @param version The MPEG-G minor version to use for assembling the file.
   * @return An `MggFile` object representing the assembled MPEG-G file.
   */
  genie::format::mgg::MggFile assemble(genie::core::MpegMinorVersion version);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_FILE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
