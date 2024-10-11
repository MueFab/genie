/**
 * @file
 * @brief Header file defining the `FileHeader` class, which represents the file header of an MPEG-G format file.
 * @details This file provides the class definition for `FileHeader`, which encapsulates information about the file header,
 *          including the major brand, minor version, and compatible brands of the MPEG-G file. The `FileHeader` class
 *          inherits from `GenInfo` and includes methods for reading, writing, and comparing file header elements.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_FILE_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_FILE_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing the file header for an MPEG-G file.
 * @details The `FileHeader` class stores metadata related to the file format, such as the major brand identifier, the minor
 *          version number, and a list of compatible brands that this file conforms to. It extends `GenInfo` to provide
 *          specialized serialization and deserialization for file header metadata boxes.
 */
class FileHeader : public GenInfo {
 public:
    /**
     * @brief Retrieve the unique key identifier for the file header element.
     * @details The key identifier is used to differentiate this type of metadata box from other types. For `FileHeader`,
     *          this typically corresponds to a string identifier.
     * @return A constant reference to the string representing the key.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Default constructor for the `FileHeader` class.
     * @details Initializes the file header with empty or default values.
     */
    FileHeader();

    /**
     * @brief Construct a new `FileHeader` object with a specified minor version.
     * @details This constructor sets the minor version to the given parameter and leaves other fields empty.
     * @param _minor_version The minor version to set for the file header.
     */
    explicit FileHeader(core::MPEGMinorVersion _minor_version);

    /**
     * @brief Construct a new `FileHeader` object by reading from a `BitReader`.
     * @details Initializes the file header by parsing its fields from the given bitstream reader.
     * @param bitreader The `BitReader` to extract the file header information from.
     */
    explicit FileHeader(genie::util::BitReader& bitreader);

    /**
     * @brief Add a compatible brand to the list of brands supported by this file.
     * @details Compatible brands are used to specify other formats or versions that the file is compatible with.
     * @param brand A string representing the compatible brand to add.
     */
    void addCompatibleBrand(std::string brand);

    /**
     * @brief Retrieve the major brand identifier of the file header.
     * @details The major brand indicates the primary format or specification this file conforms to.
     * @return A constant reference to the string representing the major brand.
     */
    [[nodiscard]] const std::string& getMajorBrand() const;

    /**
     * @brief Retrieve the minor version number of the file header.
     * @details The minor version is used to indicate the specific version of the major brand.
     * @return An enum representing the minor version of the file header.
     */
    [[nodiscard]] core::MPEGMinorVersion getMinorVersion() const;

    /**
     * @brief Retrieve the list of compatible brands for the file.
     * @details The compatible brands provide additional information on which other formats or versions are supported by this file.
     * @return A constant reference to a vector containing the compatible brand strings.
     */
    [[nodiscard]] const std::vector<std::string>& getCompatibleBrands() const;

    /**
     * @brief Write the content of the `FileHeader` object to a `BitWriter`.
     * @details This method serializes the file header data, including the major brand, minor version, and compatible brands,
     *          into the specified `BitWriter` object.
     * @param bitWriter The `BitWriter` to serialize the data into.
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief Compare this `FileHeader` object with another for equality.
     * @details The equality check considers the major brand, minor version, and compatible brands to determine if two
     *          file headers are equivalent.
     * @param info The other `GenInfo` object to compare against.
     * @return True if the two file headers are equal, false otherwise.
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief Print the content of the `FileHeader` object for debugging.
     * @details This method outputs the internal state of the file header, including the major brand, minor version, and
     *          compatible brands, to the specified output stream.
     * @param output The stream to print the debug information to.
     * @param depth The current level of indentation for nested structures.
     * @param max_depth The maximum depth for nested structures to be printed.
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;

 private:
    std::string major_brand;                     //!< @brief The major brand identifier for the file format.
    core::MPEGMinorVersion minor_version;        //!< @brief The minor version of the file format.
    std::vector<std::string> compatible_brands;  //!< @brief A list of compatible brands supported by the file.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_FILE_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
