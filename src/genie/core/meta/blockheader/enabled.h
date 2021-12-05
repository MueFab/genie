/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_BLOCKHEADER_ENABLED_H_
#define SRC_GENIE_CORE_META_BLOCKHEADER_ENABLED_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/meta/blockheader.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {
namespace blockheader {

/**
 * @brief
 */
class Enabled : public BlockHeader {
 private:
    bool MIT_flag;      //!< @brief
    bool CC_mode_flag;  //!< @brief

 public:
    /**
     * @brief
     * @param mit
     * @param cc
     */
    Enabled(bool mit, bool cc);

    /**
     * @brief
     * @param json
     */
    explicit Enabled(const nlohmann::json& json);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const override;

    /**
     * @brief
     * @return
     */
    bool getMITFlag() const;

    /**
     * @brief
     * @return
     */
    bool getCCFlag() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace blockheader
}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_BLOCKHEADER_ENABLED_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
