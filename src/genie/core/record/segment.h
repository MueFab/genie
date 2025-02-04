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
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

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
    const std::string& getSequence() const;

    /**
     * @brief
     * @return
     */
    const std::vector<std::string>& getQualities() const;

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

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_SEGMENT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
