/**
 * @file
 * @brief Defines the `AccessUnitHeader` class for handling access unit header information in the MPEG-G mgg format.
 * @details The `AccessUnitHeader` class is responsible for representing, reading, and writing access unit headers.
 *          It manages details about the configuration of access units, such as MPEG-G AU headers and associated flags.
 *          This class interacts with various parameter sets and helps maintain metadata for access units in MPEG-G data.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class for handling access unit header information in the MPEG-G mgg format.
 * @details This class encapsulates the MPEG-G access unit header information, including details from the AUHeader class
 *          in the mgb format. It provides methods to manage, serialize, and deserialize header data, ensuring proper
 *          handling of access unit configurations and metadata within an MPEG-G file.
 */
class AccessUnitHeader : public GenInfo {
 private:
    genie::format::mgb::AUHeader header;  //!< @brief MPEG-G AU header for the current access unit.
    bool mit_flag;                        //!< @brief Flag indicating if MIT (Master Index Table) is enabled.

 public:
    /**
     * @brief Outputs a detailed debug representation of the access unit header.
     * @param output The stream to write the debug information to.
     * @param depth The depth level of indentation for debug printout.
     * @param max_depth The maximum depth for detailed printing.
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;

    /**
     * @brief Checks equality between two `AccessUnitHeader` objects.
     * @param info Reference to another `GenInfo` object for comparison.
     * @return True if both objects have identical header and flag values, otherwise false.
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief Retrieves a constant reference to the MPEG-G AUHeader.
     * @return The `AUHeader` object representing the MPEG-G header.
     */
    [[nodiscard]] const genie::format::mgb::AUHeader& getHeader() const;

    /**
     * @brief Retrieves a mutable reference to the MPEG-G AUHeader.
     * @return A mutable reference to the `AUHeader` object representing the MPEG-G header.
     */
    genie::format::mgb::AUHeader& getHeader();

    /**
     * @brief Default constructor for creating an empty `AccessUnitHeader` object.
     */
    AccessUnitHeader();

    /**
     * @brief Constructs the `AccessUnitHeader` object from a BitReader stream.
     * @param reader The BitReader object to extract data from.
     * @param parameterSets Map of encoding parameter sets used in the current MPEG-G dataset.
     * @param mit Flag indicating whether MIT (Master Index Table) is enabled.
     */
    explicit AccessUnitHeader(util::BitReader& reader,
                              const std::map<size_t, core::parameter::EncodingSet>& parameterSets, bool mit);

    /**
     * @brief Constructs the `AccessUnitHeader` object with specific AU header and MIT flag values.
     * @param _header The `AUHeader` object representing the MPEG-G header.
     * @param _mit_flag Boolean flag indicating if MIT is enabled.
     */
    explicit AccessUnitHeader(genie::format::mgb::AUHeader _header, bool _mit_flag);

    /**
     * @brief Writes the access unit header information to a BitWriter stream.
     * @param bitWriter The BitWriter to write the serialized data to.
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief Retrieves the key identifier for the `AccessUnitHeader` object.
     * @return A reference to the string representing the key.
     */
    [[nodiscard]] const std::string& getKey() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
