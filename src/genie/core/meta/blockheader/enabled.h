/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_BLOCKHEADER_ENABLED_H_
#define SRC_GENIE_CORE_META_BLOCKHEADER_ENABLED_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/meta/blockheader.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta::blockheader {

/**
 * @brief Represents block continuity with block headers
 */
class Enabled : public BlockHeader {
 private:
    bool MIT_flag;      //!< @brief If true, master index table is present
    bool CC_mode_flag;  //!< @brief If true, access units are ordered by class instead mapping position

 public:
    /**
     * @brief Construct from raw values
     * @param mit MIT_flag
     * @param cc CC_mode_flag
     */
    Enabled(bool mit, bool cc);

    /**
     * @brief Construct from json
     * @param json Json representation
     */
    explicit Enabled(const nlohmann::json& json);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    [[nodiscard]] nlohmann::json toJson() const override;

    /**
     * @brief Return MIT_flag
     * @return MIT_flag
     */
    [[nodiscard]] bool getMITFlag() const;

    /**
     * @brief Return CC_mode_flag
     * @return CC_mode_flag
     */
    [[nodiscard]] bool getCCFlag() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta::blockheader

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_BLOCKHEADER_ENABLED_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
