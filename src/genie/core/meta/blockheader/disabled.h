/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_CORE_META_BLOCKHEADER_DISABLED_H_
#define SRC_GENIE_CORE_META_BLOCKHEADER_DISABLED_H_

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
class Disabled : public BlockHeader {
 private:
    bool ordered_blocks_flag;  //!< @brief

 public:
    /**
     * @brief
     * @param ordered
     */
    explicit Disabled(bool ordered);

    /**
     * @brief
     * @param json
     */
    explicit Disabled(const nlohmann::json& json);

    /**
     * @brief
     * @return
     */
    bool getOrderedFlag() const;

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace blockheader
}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_BLOCKHEADER_DISABLED_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
