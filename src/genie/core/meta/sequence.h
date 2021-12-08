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
 * @brief Reference sequence information
 */
class Sequence {
 private:
    std::string name;  //!< @brief Sequence name
    uint64_t length;   //!< @brief Sequence length
    uint16_t id;       //!< @brief Sequence ID

 public:
    /**
     * @brief Construct from raw values
     * @param _name Sequence name
     * @param _length  Sequence length
     * @param _id Sequence id
     */
    explicit Sequence(std::string _name, uint64_t _length, uint16_t _id);

    /**
     * @brief Construct from json
     * @param json Json representation
     */
    explicit Sequence(const nlohmann::json& json);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    nlohmann::json toJson() const;

    /**
     * @brief Get Name of Sequence
     * @return Name
     */
    const std::string& getName() const;

    /**
     * @brief Get length of sequence
     * @return Length
     */
    uint64_t getLength() const;

    /**
     * @brief Get Sequence ID
     * @return ID
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
