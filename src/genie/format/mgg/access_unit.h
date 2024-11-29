/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `AccessUnit` class for handling access unit components in
 * the MPEG-G mgg format.
 * @details The `AccessUnit` class manages all components and metadata
 * associated with an access unit, including headers, optional information and
 * protection data, and associated blocks. This class provides methods for
 * creating, manipulating, and serializing access unit data in the mgg format,
 * ensuring compatibility with the MPEG-G standard.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_H_

// -----------------------------------------------------------------------------

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mgg/access_unit_header.h"
#include "genie/format/mgg/au_information.h"
#include "genie/format/mgg/au_protection.h"
#include "genie/format/mgg/block.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Represents a complete access unit in the MPEG-G mgg format.
 * @details This class encapsulates an entire access unit, which includes its
 * header, information, protection, and associated data blocks. The `AccessUnit`
 * class facilitates the creation, manipulation, serialization, and
 *          deserialization of access unit data for MPEG-G formatted files.
 */
class AccessUnit final : public GenInfo {
  AccessUnitHeader header_;  //!< @brief Header for the current access unit.
  std::optional<AuInformation>
      au_information_;  //!< @brief Optional AU information data.
  std::optional<AuProtection>
      au_protection_;  //!< @brief Optional AU protection data.
  std::vector<Block>
      blocks_;  //!< @brief List of blocks associated with this access unit.

  core::MpegMinorVersion
      version_;  //!< @brief Version of the MPEG-G standard used.

 public:
  /**
   * @brief Prints a detailed debug representation of the access unit.
   * @param output The stream to write the debug information to.
   * @param depth The depth level of indentation for debug printout.
   * @param max_depth The maximum depth for detailed printing.
   */
  void PrintDebug(std::ostream& output, uint8_t depth,
                   uint8_t max_depth) const override;

  /**
   * @brief Retrieves a mutable reference to the AU information object.
   * @return The mutable AU information.
   */
  AuInformation& GetInformation();

  /**
   * @brief Retrieves a mutable reference to the AU protection object.
   * @return The mutable AU protection.
   */
  AuProtection& GetProtection();

  /**
   * @brief Converts the current access unit into a format-compatible object
   * for further processing.
   * @return An `mgb::AccessUnit` object representing the encapsulated access
   * unit.
   */
  mgb::AccessUnit Decapsulate();

  /**
   * @brief Constructs an `AccessUnit` object from an `mgb::AccessUnit` and
   * additional configuration options.
   * @param au The `mgb::AccessUnit` object to convert.
   * @param mit Boolean flag indicating if MIT is enabled.
   * @param version The version of the MPEG-G standard.
   */
  AccessUnit(mgb::AccessUnit au, bool mit, core::MpegMinorVersion version);

  /**
   * @brief Equality operator for comparing access unit objects.
   * @param info Reference to another `GenInfo` object.
   * @return True if both objects have identical attributes, otherwise false.
   */
  bool operator==(const GenInfo& info) const override;

  /**
   * @brief Constructs an `AccessUnit` object from an `AccessUnitHeader`.
   * @param h The header object for the access unit.
   * @param version The MPEG-G version.
   */
  explicit AccessUnit(AccessUnitHeader h, core::MpegMinorVersion version);

  /**
   * @brief Constructs an `AccessUnit` object from a BitReader stream.
   * @param reader The BitReader object to extract data from.
   * @param parameter_sets Map of encoding parameter sets used in the current
   * MPEG-G dataset.
   * @param mit Boolean flag indicating if MIT is enabled.
   * @param block_header Boolean flag indicating if block headers are
   * included.
   * @param version The MPEG-G version.
   */
  AccessUnit(
      util::BitReader& reader,
      const std::map<size_t, core::parameter::EncodingSet>& parameter_sets,
      bool mit, bool block_header, core::MpegMinorVersion version);

  /**
   * @brief Adds a new block to the access unit.
   * @param b The block object to add.
   */
  void AddBlock(Block b);

  /**
   * @brief Retrieves a constant reference to the list of blocks in the access
   * unit.
   * @return A constant reference to the vector of blocks.
   */
  [[nodiscard]] const std::vector<Block>& GetBlocks() const;

  /**
   * @brief Retrieves a constant reference to the access unit header.
   * @return A constant reference to the access unit header.
   */
  [[nodiscard]] const AccessUnitHeader& GetHeader() const;

  /**
   * @brief Checks if AU information is available.
   * @return True if AU information is available, otherwise false.
   */
  [[nodiscard]] bool HasInformation() const;

  /**
   * @brief Checks if AU protection is available.
   * @return True if AU protection is available, otherwise false.
   */
  [[nodiscard]] bool HasProtection() const;

  /**
   * @brief Retrieves a constant reference to the AU information object.
   * @return A constant reference to the AU information.
   */
  [[nodiscard]] const AuInformation& GetInformation() const;

  /**
   * @brief Retrieves a constant reference to the AU protection object.
   * @return A constant reference to the AU protection.
   */
  [[nodiscard]] const AuProtection& GetProtection() const;

  /**
   * @brief Sets the AU information for the access unit.
   * @param au The AUInformation object to set.
   */
  void SetInformation(AuInformation au);

  /**
   * @brief Sets the AU protection for the access unit.
   * @param au The AUProtection object to set.
   */
  void SetProtection(AuProtection au);

  /**
   * @brief Writes the access unit data to a BitWriter stream.
   * @param bit_writer The BitWriter object to write the serialized data to.
   */
  void BoxWrite(util::BitWriter& bit_writer) const override;

  /**
   * @brief Retrieves the key identifier for the access unit.
   * @return A constant reference to the string representing the key.
   */
  [[nodiscard]] const std::string& GetKey() const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
