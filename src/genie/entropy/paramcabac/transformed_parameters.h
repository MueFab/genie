/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_TRANSFORMED_PARAMETERS_H
#define GENIE_TRANSFORMED_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <boost/optional/optional.hpp>
#include <memory>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * ISO 23092-2 Section 8.3.4 table
 */
class TransformedParameters {
   public:
    enum class TransformIdSubseq : uint8_t {
        NO_TRANSFORM = 0,
        EQUALITY_CODING = 1,
        MATCH_CODING = 2,
        RLE_CODING = 3,
        MERGE_CODING = 4
    };

    virtual void write(util::BitWriter &writer) const;

    TransformedParameters();

    TransformedParameters(util::BitReader &reader);

    TransformedParameters(const TransformIdSubseq &_transform_ID_subseq, uint16_t param);

    virtual ~TransformedParameters() = default;

    size_t getNumStreams() const;

    TransformIdSubseq getTransformIdSubseq() const;

    uint16_t getParam() const;

    const std::vector<uint8_t> *getMergeCodingShiftSize() const;

   private:
    TransformIdSubseq transform_ID_subseq;               //!< : 8; Line 2
    boost::optional<uint16_t> match_coding_buffer_size;  //!< : 16; Line 6
    boost::optional<uint8_t> rle_coding_guard;           //!< : 8; Line 9
    boost::optional<uint8_t> merge_coding_subseq_count;  //!< : 4; Line 12
    std::vector<uint8_t> merge_coding_shift_size;        //!< : 5; Line 15
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_TRANSFORMED_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------