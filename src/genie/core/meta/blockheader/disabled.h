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
 * @brief Represents block continuity without block headers
 */
class Disabled : public BlockHeader {
 private:
    bool ordered_blocks_flag;  //!< @brief True: Blocks are ordered by mapping position

 public:
    /**
     * @brief Constructor from raw values
     * @param ordered ordered_blocks_flag
     */
    explicit Disabled(bool ordered);

    /**
     * @brief Constructor from json
     * @param json Json input
     */
    explicit Disabled(const nlohmann::json& json);

    /**
     * @brief Return ordered_blocks_flag
     * @return ordered_blocks_flag
     */
    bool getOrderedFlag() const;

    /**
     * @brief Convert to json
     * @return Json representation
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
