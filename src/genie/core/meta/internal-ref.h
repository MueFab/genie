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
 * @brief
 */
class InternalRef : public RefBase {
 private:
    uint16_t internal_dataset_group_id;  //!< @brief
    uint16_t internal_dataset_ID;        //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    uint16_t getGroupID() const;

    /**
     * @brief
     * @return
     */
    uint16_t getID() const;

    /**
     * @brief
     * @param json
     */
    explicit InternalRef(nlohmann::json json);

    /**
     * @brief
     * @param group_id
     * @param id
     */
    InternalRef(uint16_t group_id, uint16_t id);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const override;

    /**
     * @brief
     * @return
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
