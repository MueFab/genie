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

namespace genie {
namespace core {
namespace meta {

/**
 * @brief
 */
class DescriptorStream {
 private:
    size_t descriptor_ID;             //!< @brief
    std::string DS_protection_value;  //!< @brief

 public:
    /**
     * @brief
     * @param _descriptor_ID
     * @param _ds_protection_value
     */
    DescriptorStream(size_t _descriptor_ID, std::string _ds_protection_value);

    /**
     * @brief
     * @param obj
     */
    explicit DescriptorStream(const nlohmann::json& obj);

    /**
     * @brief
     * @return
     */
    size_t getID() const;

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;

    /**
     * @brief
     * @return
     */
    const std::string& getProtection() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
