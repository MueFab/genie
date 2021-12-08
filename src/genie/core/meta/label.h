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
 * @brief Label metadata
 */
class Label {
 private:
    std::string label_ID;         //!< @brief Name of the label
    std::vector<Region> regions;  //!< @brief Regions in the genome the label applies to

 public:
    /**
     * @brief Construct from raw values
     * @param id Name of the label
     */
    explicit Label(std::string id);

    /**
     * @brief Construct from json
     * @param json Json representation
     */
    explicit Label(const nlohmann::json& json);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    nlohmann::json toJson() const;

    /**
     * @brief Apply label to a new region of the genome
     * @param r New region
     */
    void addRegion(Region r);

    /**
     * @brief Return the name of the label
     * @return label id
     */
    const std::string& getID() const;

    /**
     * @brief Return the list of active region on the genome
     * @return Active regions of this label
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
