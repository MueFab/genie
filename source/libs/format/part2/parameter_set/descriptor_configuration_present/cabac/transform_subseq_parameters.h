#ifndef GENIE_TRANSFORM_SUBSEQ_PARAMETERS_H
#define GENIE_TRANSFORM_SUBSEQ_PARAMETERS_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "util/bitwriter.h"


// -----------------------------------------------------------------------------------------------------------------

namespace format {

namespace desc_conf_pres {
namespace cabac {

/**
 * ISO 23092-2 Section 8.3.4 table
 */
class TransformSubseqParameters {
   public:
    enum class TransformIdSubseq : uint8_t {
        NO_TRANSFORM = 0,
        EQUALITY_CODING = 1,
        MATCH_CODING = 2,
        RLE_CODING = 3,
        MERGE_CODING = 4
    };

    virtual void write(util::BitWriter *writer) const;

    TransformSubseqParameters();

    TransformSubseqParameters(const TransformIdSubseq &_transform_ID_subseq, uint16_t param);

    std::unique_ptr<TransformSubseqParameters> clone() const;

    size_t getNumStreams() const;

   private:
    TransformIdSubseq transform_ID_subseq;               //!< : 8; Line 2
    std::unique_ptr<uint16_t> match_coding_buffer_size;  //!< : 16; Line 6
    std::unique_ptr<uint8_t> rle_coding_guard;           //!< : 8; Line 9
    std::unique_ptr<uint8_t> merge_coding_subseq_count;  //!< : 4; Line 12
    std::vector<uint8_t> merge_coding_shift_size;        //!< : 5; Line 15
};
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_TRANSFORM_SUBSEQ_PARAMETERS_H
