/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_CORE_META_DESCRIPTOR_STREAM_H_
#define SRC_GENIE_CORE_META_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Meta information for descriptor stream
 */
class DescriptorStream {
 private:
    size_t descriptor_ID;             //!< @brief ID of descriptor stream
    std::string DS_protection_value;  //!< @brief Part 3 protection information

 public:
    /**
     * @brief Construct from raw values
     * @param _descriptor_ID Descriptor ID
     * @param _ds_protection_value Part 3 protection information
     */
    DescriptorStream(size_t _descriptor_ID, std::string _ds_protection_value);

    /**
     * @brief Construct from json
     * @param obj Json representation
     */
    explicit DescriptorStream(const nlohmann::json& obj);

    /**
     * @brief Get descriptor ID
     * @return Descriptor ID
     */
    [[nodiscard]] size_t getID() const;

    /**
     * @brief Convert to json
     * @return Json representation
     */
    [[nodiscard]] nlohmann::json toJson() const;

    /**
     * @brief Return Part 3 protection information
     * @return Part 3 protection information
     */
    [[nodiscard]] const std::string& getProtection() const;

    /**
     * @brief
     * @return
     */
    std::string& getProtection();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
