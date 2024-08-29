/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_REFBASE_H_
#define SRC_GENIE_CORE_META_REFBASE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Base class for reference information (internal and external)
 */
class RefBase {
 protected:
    /**
     * @brief Do not allow construction of interface
     */
    RefBase() = default;

 public:
    /**
     * @brief Reference types supported by the standard
     */
    enum class ReferenceType : uint8_t { MPEGG_REF = 0, RAW_REF = 1, FASTA_REF = 2 };

    /**
     * @brief Virtual destructor to allow inheritance
     */
    virtual ~RefBase() = default;

    /**
     * @brief Convert to json
     * @return Json representation
     */
    [[nodiscard]] virtual nlohmann::json toJson() const = 0;

    /**
     * @brief Get Json Key of this base (internal vs external ref)
     * @return Json key
     */
    [[nodiscard]] virtual const std::string& getKeyName() const = 0;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] virtual std::unique_ptr<RefBase> clone() const = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_REFBASE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
