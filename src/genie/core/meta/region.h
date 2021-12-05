/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_REGION_H_
#define SRC_GENIE_CORE_META_REGION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/core/record/class-type.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

/**
 * @brief
 */
class Region {
 private:
    uint16_t seq_ID;                                      //!< @brief
    std::vector<genie::core::record::ClassType> classes;  //!< @brief
    uint64_t start_pos;                                   //!< @brief
    uint64_t end_pos;                                     //!< @brief

 public:
    /**
     * @brief
     * @param seqid
     * @param start
     * @param end
     * @param _classes
     */
    Region(uint16_t seqid, uint64_t start, uint64_t end, std::vector<genie::core::record::ClassType> _classes);

    /**
     * @brief
     * @param json
     */
    explicit Region(const nlohmann::json& json);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;

    /**
     * @brief
     * @return
     */
    uint16_t getSeqID() const;

    /**
     * @brief
     * @return
     */
    const std::vector<genie::core::record::ClassType>& getClasses() const;

    /**
     * @brief
     * @return
     */
    uint64_t getStartPos() const;

    /**
     * @brief
     * @return
     */
    uint64_t getEndPos() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_REGION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
