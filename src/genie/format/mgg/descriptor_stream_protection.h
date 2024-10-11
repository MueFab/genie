/**
 * @file
 * @brief Defines the `DescriptorStreamProtection` class, representing the protection information of descriptor streams.
 * @details The `DescriptorStreamProtection` class is used to store and manipulate the protection value of descriptor
 *          streams in an MPEG-G file. This includes serialization and deserialization methods for reading and writing
 *          the protection value from and to bitstreams, as well as comparison and access methods.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_PROTECTION_H_
#define SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_PROTECTION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing the protection value associated with a descriptor stream.
 * @details The `DescriptorStreamProtection` class is used to encapsulate the protection value of descriptor streams.
 *          It provides functionality to read and write this value to bitstreams, access it as a string, and compare it
 *          against other instances of protection values. The protection value is stored as a simple string.
 */
class DescriptorStreamProtection : public GenInfo {
 private:
    std::string DSProtectionValue;  //!< @brief The protection value associated with the descriptor stream.

 public:
    /**
     * @brief Extracts and returns the protection value.
     * @details This function decapsulates the protection value stored in the `DescriptorStreamProtection` object and returns it.
     * @return The protection value as a string.
     */
    std::string decapsulate();

    /**
     * @brief Constructor that initializes the protection value with a given string.
     * @details This constructor sets the `DSProtectionValue` member to the provided string value.
     * @param _DSProtectionValue The string value to initialize the protection value with.
     */
    explicit DescriptorStreamProtection(std::string _DSProtectionValue);

    /**
     * @brief Constructs a `DescriptorStreamProtection` object by reading from a `BitReader`.
     * @details This constructor initializes the `DSProtectionValue` by extracting the data from the provided `BitReader`.
     * @param reader The `BitReader` to read the protection value from.
     */
    explicit DescriptorStreamProtection(genie::util::BitReader& reader);

    /**
     * @brief Retrieves the protection value stored in the object.
     * @details This function returns a constant reference to the string representing the protection value.
     * @return A constant reference to the string containing the protection value.
     */
    [[nodiscard]] const std::string& getProtectionValue() const;

    /**
     * @brief Returns the unique key identifier for the `DescriptorStreamProtection` object.
     * @details This key is used to identify this type of metadata box within an MPEG-G file.
     * @return A constant reference to the string representing the key identifier.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Write the protection value to a `BitWriter`.
     * @details Serializes the protection value into the specified `BitWriter` object, writing it in the required format.
     * @param bitWriter The `BitWriter` to write the serialized protection value to.
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief Compares two `DescriptorStreamProtection` objects for equality.
     * @details This function checks if the protection values in two `DescriptorStreamProtection` instances are identical.
     * @param info The other `GenInfo` object to compare against.
     * @return True if both objects have the same protection value; otherwise, false.
     */
    bool operator==(const GenInfo& info) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_PROTECTION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
