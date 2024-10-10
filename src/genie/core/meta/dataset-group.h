/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */
#ifndef SRC_GENIE_CORE_META_DATASET_GROUP_H_
#define SRC_GENIE_CORE_META_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Information about a dataset group
 */
class DatasetGroup {
 private:
    uint16_t dataset_group_ID;        //!< @brief ID of dataset group
    uint8_t version_number;           //!< @brief Current iteration, counting up for every change
    std::string DG_metadata_value;    //!< @brief MPEG-G part 3 meta information
    std::string DG_protection_value;  //!< @brief MPEG-G part 3 protection information

 public:
    /**
     * @brief
     * @param meta
     */
    void setMetadata(std::string meta);

    /**
     * @brief
     * @param prot
     */
    void setProtection(std::string prot);

    /**
     * @brief Return dataset group ID
     * @return dataset group ID
     */
    [[nodiscard]] uint16_t getID() const;

    /**
     * @brief Return current iteration version
     * @return Current version
     */
    [[nodiscard]] uint8_t getVersion() const;

    /**
     * @brief Return MPEG-G part 3 meta information
     * @return MPEG-G part 3 meta information
     */
    [[nodiscard]] const std::string& getInformation() const;

    /**
     * @brief
     * @return
     */
    std::string& getInformation();

    /**
     * @brief
     * @return
     */
    std::string& getProtection();

    /**
     * @brief Return MPEG-G part 3 meta information
     * @return MPEG-G part 3 protection information
     */
    [[nodiscard]] const std::string& getProtection() const;

    /**
     * @brief Construct from raw values
     * @param id ID of dataset group
     * @param version Current version number
     * @param meta MPEG-G part 3 meta information
     * @param protection MPEG-G part 3 protection information
     */
    DatasetGroup(uint16_t id, uint8_t version, std::string meta, std::string protection);

    /**
     * @brief Construct from json
     * @param json Json representation
     */
    explicit DatasetGroup(const nlohmann::json& json);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    [[nodiscard]] nlohmann::json toJson() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
