/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_SEGMENT_H_
#define SRC_GENIE_CORE_RECORD_SEGMENT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief
 */
class Segment {
    std::string sequence;                     //!< @brief
    std::vector<std::string> quality_values;  //!< @brief

 public:
    /**
     * @brief
     */
    Segment();

    /**
     * @brief
     * @param _sequence
     */
    explicit Segment(std::string&& _sequence);

    /**
     * @brief
     * @param length
     * @param qv_depth
     * @param reader
     */
    Segment(uint32_t length, uint8_t qv_depth, util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::string& getSequence() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<std::string>& getQualities() const;

    /**
     * @brief
     * @param qv
     */
    void addQualities(std::string&& qv);

    /**
     * @brief
     * @param write
     */
    void write(util::BitWriter& write) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_SEGMENT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
