/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_REFBASE_H_
#define SRC_GENIE_CORE_META_REFBASE_H_

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
class RefBase {
 protected:
    /**
     * @brief
     */
    RefBase() = default;

 public:
    /**
     * @brief
     */
    enum class ReferenceType : uint8_t { MPEGG_REF = 0, RAW_REF = 1, FASTA_REF = 2 };

    /**
     * @brief
     */
    virtual ~RefBase() = default;

    /**
     * @brief
     * @return
     */
    virtual nlohmann::json toJson() const = 0;

    /**
     * @brief
     * @return
     */
    virtual const std::string& getKeyName() const = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_REFBASE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
