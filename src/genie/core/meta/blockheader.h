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

namespace genie::core::meta {

/**
 * @brief Block header presence interface
 */
class BlockHeader {
 public:
    /**
     * @brief Block header modes
     */
    enum class HeaderType : uint8_t { DISABLED = 0, ENABLED = 1 };

 private:
    HeaderType type;  //!< @brief Active type

 protected:
    /**
     * @brief Construct from type
     * @param _type Block header type
     */
    explicit BlockHeader(HeaderType _type);

 public:
    /**
     * @brief Virtual destructor for inheritance
     */
    virtual ~BlockHeader() = default;

    /**
     * @brief Convert to json
     * @return Json representation
     */
    [[nodiscard]] virtual nlohmann::json toJson() const = 0;

    /**
     * @brief Return block header mode
     * @return Block header mode
     */
    [[nodiscard]] HeaderType getType() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_BLOCKHEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
