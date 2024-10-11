/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `Version` class for representing the version information of a reference in MPEG-G files.
 *
 * The `Version` class encapsulates versioning details of a reference, including the major, minor, and patch versions.
 * It provides methods for constructing, serializing, and deserializing version data, making it suitable for managing
 * version information in the context of MPEG-G reference metadata.
 *
 * This class is designed to support the MPEG-G standard and ensure compatibility between different versions of references.
 * It facilitates reading and writing version data to and from bitstreams, ensuring seamless integration in MPEG-G files.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_VERSION_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_VERSION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference {

/**
 * @brief Represents the version information of a reference in an MPEG-G file.
 *
 * The `Version` class stores the major, minor, and patch version numbers for a reference. It provides methods
 * for constructing `Version` objects, reading and writing version data to bitstreams, and comparing versions.
 * This class is integral for managing reference metadata within the MPEG-G standard.
 */
class Version {
 private:
    uint16_t v_major;  //!< @brief Major version number of the reference.
    uint16_t v_minor;  //!< @brief Minor version number of the reference.
    uint16_t v_patch;  //!< @brief Patch version number of the reference.

 public:
    /**
     * @brief Compares two `Version` objects for equality.
     *
     * This operator checks if the major, minor, and patch versions of two `Version` objects are identical.
     *
     * @param other The `Version` object to compare against.
     * @return True if both versions are equal, false otherwise.
     */
    bool operator==(const Version& other) const;

    /**
     * @brief Constructs a `Version` object with specified version numbers.
     *
     * @param _v_major Major version number.
     * @param _v_minor Minor version number.
     * @param _v_patch Patch version number.
     */
    Version(uint16_t _v_major, uint16_t _v_minor, uint16_t _v_patch);

    /**
     * @brief Constructs a `Version` object by reading from a bitstream.
     *
     * This constructor initializes a `Version` object using version data read from the provided bitstream.
     *
     * @param reader The `BitReader` to read the version data from.
     */
    explicit Version(genie::util::BitReader& reader);

    /**
     * @brief Serializes the `Version` object to a bitstream.
     *
     * This method writes the major, minor, and patch version numbers into a bitstream for storage or transmission.
     *
     * @param writer The `BitWriter` to write the serialized version data to.
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief Gets the major version number.
     * @return The major version number.
     */
    [[nodiscard]] uint16_t getMajor() const;

    /**
     * @brief Gets the minor version number.
     * @return The minor version number.
     */
    [[nodiscard]] uint16_t getMinor() const;

    /**
     * @brief Gets the patch version number.
     * @return The patch version number.
     */
    [[nodiscard]] uint16_t getPatch() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_VERSION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
