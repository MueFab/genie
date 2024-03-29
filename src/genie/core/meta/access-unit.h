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

    /**
     * @brief
     * @param id
     */
    explicit AccessUnit(size_t id);

    /**
     * @brief
     * @param information
     */
    void setInformation(std::string information);

    /**
     * @brief
     * @param protection
     */
    void setProtection(std::string protection);

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

    /**
     * @brief
     * @return
     */
    std::string& getInformation();

    /**
     * @brief MPEG-G Part 3 protection data
     * @return MPEG-G Part 3 protection data
     */
    const std::string& getProtection() const;

    /**
     * @brief
     * @return
     */
    std::string& getProtection();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
