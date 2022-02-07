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
 * @brief Holds metadata associated with an access unit
 */
class AccessUnit {
 private:
    size_t access_unit_ID;             //!< @brief ID of associated access unit
    std::string AU_information_value;  //!< @brief MPEG-G Part 3 metadata
    std::string AU_protection_value;   //!< @brief MPEG-G Part 3 protection data

 public:
    /**
     * @brief Construct from raw data
     * @param id ID of associated access unit
     * @param information MPEG-G Part 3 metadata
     * @param protection MPEG-G Part 3 protection data
     */
    AccessUnit(size_t id, std::string information, std::string protection);

    explicit AccessUnit(size_t id) : access_unit_ID(id) {

    }

    void setInformation(std::string information) {
        AU_information_value = std::move(information);
    }

    void setProtection(std::string protection) {
        AU_protection_value = std::move(protection);
    }

    /**
     * @brief Construct form json
     * @param obj Json representation
     */
    explicit AccessUnit(const nlohmann::json& obj);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    nlohmann::json toJson() const;

    /**
     * @brief Get ID of associated access unit
     * @return AU ID
     */
    size_t getID() const;

    /**
     * @brief Get MPEG-G Part 3 metadata
     * @return MPEG-G Part 3 metadata
     */
    const std::string& getInformation() const;


    std::string& getInformation() {
        return AU_information_value;
    }

    /**
     * @brief MPEG-G Part 3 protection data
     * @return MPEG-G Part 3 protection data
     */
    const std::string& getProtection() const;

    std::string& getProtection() {
        return AU_protection_value;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
