/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SEGMENT_H
#define GENIE_SEGMENT_H

// ---------------------------------------------------------------------------------------------------------------------

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <memory>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_rec {

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
    void addQualityValues(std::string&& qv);

    /**
     *
     * @param write
     */
    virtual void write(util::BitWriter& write) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SEGMENT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------