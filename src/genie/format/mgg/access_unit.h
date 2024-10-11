/**
 * @file
 * @brief Defines the `AccessUnit` class for handling access unit components in the MPEG-G mgg format.
 * @details The `AccessUnit` class manages all components and metadata associated with an access unit,
 *          including headers, optional information and protection data, and associated blocks. This
 *          class provides methods for creating, manipulating, and serializing access unit data in the
 *          mgg format, ensuring compatibility with the MPEG-G standard.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

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
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Represents a complete access unit in the MPEG-G mgg format.
 * @details This class encapsulates an entire access unit, which includes its header, information, protection, and
 *          associated data blocks. The `AccessUnit` class facilitates the creation, manipulation, serialization, and
 *          deserialization of access unit data for MPEG-G formatted files.
 */
class AccessUnit : public GenInfo {
 private:
    AccessUnitHeader header;                      //!< @brief Header for the current access unit.
    std::optional<AUInformation> au_information;  //!< @brief Optional AU information data.
    std::optional<AUProtection> au_protection;    //!< @brief Optional AU protection data.
    std::vector<Block> blocks;                    //!< @brief List of blocks associated with this access unit.

    core::MPEGMinorVersion version;  //!< @brief Version of the MPEG-G standard used.

 public:
    /**
     * @brief Prints a detailed debug representation of the access unit.
     * @param output The stream to write the debug information to.
     * @param depth The depth level of indentation for debug printout.
     * @param max_depth The maximum depth for detailed printing.
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;

    /**
     * @brief Retrieves a mutable reference to the AU information object.
     * @return The mutable AU information.
     */
    AUInformation& getInformation();

    /**
     * @brief Retrieves a mutable reference to the AU protection object.
     * @return The mutable AU protection.
     */
    AUProtection& getProtection();

    /**
     * @brief Converts the current access unit into a format-compatible object for further processing.
     * @return An `mgb::AccessUnit` object representing the encapsulated access unit.
     */
    format::mgb::AccessUnit decapsulate();

    /**
     * @brief Constructs an `AccessUnit` object from an `mgb::AccessUnit` and additional configuration options.
     * @param au The `mgb::AccessUnit` object to convert.
     * @param mit Boolean flag indicating if MIT is enabled.
     * @param _version The version of the MPEG-G standard.
     */
    AccessUnit(format::mgb::AccessUnit au, bool mit, core::MPEGMinorVersion _version);

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
    explicit AccessUnit(AccessUnitHeader h, core::MPEGMinorVersion version);

    /**
     * @brief Constructs an `AccessUnit` object from a BitReader stream.
     * @param reader The BitReader object to extract data from.
     * @param parameterSets Map of encoding parameter sets used in the current MPEG-G dataset.
     * @param mit Boolean flag indicating if MIT is enabled.
     * @param block_header Boolean flag indicating if block headers are included.
     * @param version The MPEG-G version.
     */
    AccessUnit(util::BitReader& reader, const std::map<size_t, core::parameter::EncodingSet>& parameterSets, bool mit,
               bool block_header, core::MPEGMinorVersion version);

    /**
     * @brief Adds a new block to the access unit.
     * @param b The block object to add.
     */
    void addBlock(Block b);

    /**
     * @brief Retrieves a constant reference to the list of blocks in the access unit.
     * @return A constant reference to the vector of blocks.
     */
    [[nodiscard]] const std::vector<Block>& getBlocks() const;

    /**
     * @brief Retrieves a constant reference to the access unit header.
     * @return A constant reference to the access unit header.
     */
    [[nodiscard]] const AccessUnitHeader& getHeader() const;

    /**
     * @brief Checks if AU information is available.
     * @return True if AU information is available, otherwise false.
     */
    [[nodiscard]] bool hasInformation() const;

    /**
     * @brief Checks if AU protection is available.
     * @return True if AU protection is available, otherwise false.
     */
    [[nodiscard]] bool hasProtection() const;

    /**
     * @brief Retrieves a constant reference to the AU information object.
     * @return A constant reference to the AU information.
     */
    [[nodiscard]] const AUInformation& getInformation() const;

    /**
     * @brief Retrieves a constant reference to the AU protection object.
     * @return A constant reference to the AU protection.
     */
    [[nodiscard]] const AUProtection& getProtection() const;

    /**
     * @brief Sets the AU information for the access unit.
     * @param au The AUInformation object to set.
     */
    void setInformation(AUInformation au);

    /**
     * @brief Sets the AU protection for the access unit.
     * @param au The AUProtection object to set.
     */
    void setProtection(AUProtection au);

    /**
     * @brief Writes the access unit data to a BitWriter stream.
     * @param bitWriter The BitWriter object to write the serialized data to.
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief Retrieves the key identifier for the access unit.
     * @return A constant reference to the string representing the key.
     */
    [[nodiscard]] const std::string& getKey() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
