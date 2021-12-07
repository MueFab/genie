/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_INTERNAL_REF_H_
#define SRC_GENIE_CORE_META_INTERNAL_REF_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/meta/refbase.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

/**
 * @brief Information baout internal references
 */
class InternalRef : public RefBase {
 private:
    uint16_t internal_dataset_group_id;  //!< @brief In which dataset group the reference is located
    uint16_t internal_dataset_ID;        //!< @brief In which dataset the reference is located

 public:
    /**
     * @brief Return dataset group id
     * @return Dataset group id
     */
    uint16_t getGroupID() const;

    /**
     * @brief Return dataset id
     * @return Datset id
     */
    uint16_t getID() const;

    /**
     * @brief Construct from json
     * @param json Json representation
     */
    explicit InternalRef(const nlohmann::json& json);

    /**
     * @brief Construct from raw values
     * @param group_id Dataset group id
     * @param id Dataset id
     */
    InternalRef(uint16_t group_id, uint16_t id);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    nlohmann::json toJson() const override;

    /**
     * @brief Returns "internal_ref"
     * @return "internal_ref"
     */
    const std::string& getKeyName() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_INTERNAL_REF_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
