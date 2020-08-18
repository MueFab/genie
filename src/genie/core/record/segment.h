/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SEGMENT_H
#define GENIE_SEGMENT_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <memory>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

/**
 *
 */
class Segment {
    std::string sequence;                     //!<
    std::vector<std::string> quality_values;  //!<

   public:
    /**
     *
     */
    Segment();

    /**
     *
     * @param _sequence
     */
    explicit Segment(std::string&& _sequence);

    /**
     *
     * @param length
     * @param qv_depth
     * @param reader
     */
    Segment(uint32_t length, uint8_t qv_depth, util::BitReader& reader);

    /**
     *
     * @return
     */
    const std::string& getSequence() const;

    /**
     *
     * @return
     */
    const std::vector<std::string>& getQualities() const;

    /**
     *
     * @param qv
     */
    void addQualities(std::string&& qv);

    /**
     *
     * @param write
     */
    void write(util::BitWriter& write) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SEGMENT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------