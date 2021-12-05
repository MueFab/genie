/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_CORE_META_DATASET_GROUP_H_
#define SRC_GENIE_CORE_META_DATASET_GROUP_H_

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
class DatasetGroup {
 private:
    uint16_t dataset_group_ID;        //!< @brief
    uint8_t version_number;           //!< @brief
    std::string DG_metadata_value;    //!< @brief
    std::string DG_protection_value;  //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    uint16_t getID() const;

    /**
     * @brief
     * @return
     */
    uint8_t getVersion() const;

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

    /**
     * @brief
     * @param id
     * @param version
     * @param meta
     * @param protection
     */
    DatasetGroup(uint16_t id, uint8_t version, std::string meta, std::string protection);

    /**
     * @brief
     * @param json
     */
    explicit DatasetGroup(const nlohmann::json& json);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
