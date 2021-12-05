/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_ACCESS_UNIT_H_
#define SRC_GENIE_CORE_META_ACCESS_UNIT_H_

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
class AccessUnit {
 private:
    size_t access_unit_ID;             //!< @brief
    std::string AU_information_value;  //!< @brief
    std::string AU_protection_value;   //!< @brief

 public:
    /**
     * @brief
     * @param id
     * @param information
     * @param protection
     */
    AccessUnit(size_t id, std::string information, std::string protection);

    /**
     * @brief
     * @param obj
     */
    explicit AccessUnit(const nlohmann::json& obj);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;

    /**
     * @brief
     * @return
     */
    size_t getID() const;

    /**
     * @brief
     * @return
     */
    const std::string& getInformation() const;

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

#endif  // SRC_GENIE_CORE_META_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
