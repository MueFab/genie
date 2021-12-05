/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_LABEL_H_
#define SRC_GENIE_CORE_META_LABEL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/core/meta/region.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

/**
 * @brief
 */
class Label {
 private:
    std::string label_ID;         //!< @brief
    std::vector<Region> regions;  //!< @brief

 public:
    /**
     * @brief
     * @param id
     * @param firstRegion
     */
    explicit Label(std::string id, Region&& firstRegion);

    /**
     * @brief
     * @param json
     */
    explicit Label(const nlohmann::json& json);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;

    /**
     * @brief
     * @param r
     */
    void addRegion(const Region& r);

    /**
     * @brief
     * @return
     */
    const std::string& geID() const;

    /**
     * @brief
     * @return
     */
    const std::vector<Region>& getRegions() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_LABEL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
