/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_SEQUENCE_H_
#define SRC_GENIE_CORE_META_SEQUENCE_H_

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
class Sequence {
 private:
    std::string name;  //!< @brief
    uint64_t length;   //!< @brief
    uint16_t id;       //!< @brief

 public:
    /**
     * @brief
     * @param _name
     * @param _length
     * @param _id
     */
    explicit Sequence(std::string _name, uint64_t _length, uint16_t _id);

    /**
     * @brief
     * @param json
     */
    explicit Sequence(const nlohmann::json& json);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;

    /**
     * @brief
     * @return
     */
    const std::string& getName() const;

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @return
     */
    uint16_t getID() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_SEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
