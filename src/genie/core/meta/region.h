/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_REGION_H_
#define SRC_GENIE_CORE_META_REGION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/core/record/alignment/class-type.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

/**
 * @brief Region on the genome, spanning many loci
 */
class Region {
 private:
    uint16_t seq_ID;                                      //!< @brief Sequence ID name
    std::vector<genie::core::record::ClassType> classes;  //!< @brief Record classes the region applies to
    uint64_t start_pos;                                   //!< @brief Start position of region
    uint64_t end_pos;                                     //!< @brief End position of region

 public:
    /**
     * @brief Construct from raw values
     * @param seqid Reference sequence ID
     * @param start Start locus
     * @param end End locus
     * @param _classes List of classes the region applies to
     */
    Region(uint16_t seqid, uint64_t start, uint64_t end, std::vector<genie::core::record::ClassType> _classes);

    /**
     * @brief Construct form json
     * @param json Json representation
     */
    explicit Region(const nlohmann::json& json);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    nlohmann::json toJson() const;

    /**
     * @brief Get sequence ID
     * @return Sequence ID
     */
    uint16_t getSeqID() const;

    /**
     * @brief Get List of active record classes
     * @return List of record classes
     */
    const std::vector<genie::core::record::ClassType>& getClasses() const;

    /**
     * @brief Return start locus
     * @return Start locus
     */
    uint64_t getStartPos() const;

    /**
     * @brief Return end locus
     * @return End locus
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
