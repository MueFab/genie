/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_CORE_META_BLOCKHEADER_H_
#define SRC_GENIE_CORE_META_BLOCKHEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

/**
 * @brief
 */
class BlockHeader {
 public:
    /**
     * @brief
     */
    enum class HeaderType : uint8_t { DISABLED = 0, ENABLED = 1 };

 private:
    HeaderType type;  //!< @brief

 protected:
    /**
     * @brief
     * @param _type
     */
    explicit BlockHeader(HeaderType _type);

 public:
    /**
     * @brief
     */
    virtual ~BlockHeader() = default;

    /**
     * @brief
     * @return
     */
    virtual nlohmann::json toJson() const = 0;

    /**
     * @brief
     * @return
     */
    HeaderType getType() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_BLOCKHEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
