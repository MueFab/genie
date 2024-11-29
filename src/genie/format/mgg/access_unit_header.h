/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `AccessUnitHeader` class for handling access unit header
 * information in the MPEG-G mgg format.
 * @details The `AccessUnitHeader` class is responsible for representing,
 * reading, and writing access unit headers. It manages details about the
 * configuration of access units, such as MPEG-G AU headers and associated
 * flags. This class interacts with various parameter sets and helps maintain
 * metadata for access units in MPEG-G data.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_HEADER_H_

// -----------------------------------------------------------------------------

#include <map>
#include <string>

#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class for handling access unit header information in the MPEG-G mgg
 * format.
 * @details This class encapsulates the MPEG-G access unit header information,
 * including details from the AUHeader class in the mgb format. It provides
 * methods to manage, serialize, and deserialize header data, ensuring proper
 *          handling of access unit configurations and metadata within an MPEG-G
 * file.
 */
class AccessUnitHeader final : public GenInfo {
  mgb::AuHeader
      header_;     //!< @brief MPEG-G AU header for the current access unit.
  bool mit_flag_;  //!< @brief Flag indicating if MIT (Master Index Table) is
                   //!< enabled.

 public:
  /**
   * @brief Outputs a detailed debug representation of the access unit header.
   * @param output The stream to write the debug information to.
   * @param depth The depth level of indentation for debug printout.
   * @param max_depth The maximum depth for detailed printing.
   */
  void PrintDebug(std::ostream& output, uint8_t depth,
                   uint8_t max_depth) const override;

  /**
   * @brief Checks equality between two `AccessUnitHeader` objects.
   * @param info Reference to another `GenInfo` object for comparison.
   * @return True if both objects have identical header and flag values,
   * otherwise false.
   */
  bool operator==(const GenInfo& info) const override;

  /**
   * @brief Retrieves a constant reference to the MPEG-G AUHeader.
   * @return The `AUHeader` object representing the MPEG-G header.
   */
  [[nodiscard]] const mgb::AuHeader& GetHeader() const;

  /**
   * @brief Retrieves a mutable reference to the MPEG-G AUHeader.
   * @return A mutable reference to the `AUHeader` object representing the
   * MPEG-G header.
   */
  mgb::AuHeader& GetHeader();

  /**
   * @brief Default constructor for creating an empty `AccessUnitHeader`
   * object.
   */
  AccessUnitHeader();

  /**
   * @brief Constructs the `AccessUnitHeader` object from a BitReader stream.
   * @param reader The BitReader object to extract data from.
   * @param parameter_sets Map of encoding parameter sets used in the current
   * MPEG-G dataset.
   * @param mit Flag indicating whether MIT (Master Index Table) is enabled.
   */
  explicit AccessUnitHeader(
      util::BitReader& reader,
      const std::map<size_t, core::parameter::EncodingSet>& parameter_sets,
      bool mit);

  /**
   * @brief Constructs the `AccessUnitHeader` object with specific AU header
   * and MIT flag values.
   * @param header The `AUHeader` object representing the MPEG-G header.
   * @param mit_flag Boolean flag indicating if MIT is enabled.
   */
  explicit AccessUnitHeader(mgb::AuHeader header, bool mit_flag);

  /**
   * @brief Writes the access unit header information to a BitWriter stream.
   * @param bit_writer The BitWriter to write the serialized data to.
   */
  void BoxWrite(util::BitWriter& bit_writer) const override;

  /**
   * @brief Retrieves the key identifier for the `AccessUnitHeader` object.
   * @return A reference to the string representing the key.
   */
  [[nodiscard]] const std::string& GetKey() const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_HEADER_H_

// -----------------------------------------------------------------------------
