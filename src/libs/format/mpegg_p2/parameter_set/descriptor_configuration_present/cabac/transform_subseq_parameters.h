#ifndef GENIE_TRANSFORM_SUBSEQ_PARAMETERS_H
#define GENIE_TRANSFORM_SUBSEQ_PARAMETERS_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "util/bitwriter.h"
#include "util/bitreader.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {
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

    virtual void write(util::BitWriter &writer) const;

    TransformSubseqParameters();

    TransformSubseqParameters(util::BitReader &reader) {
        transform_ID_subseq = TransformIdSubseq(reader.read(8));
        switch (transform_ID_subseq) {
            case TransformIdSubseq::NO_TRANSFORM:
                break;
            case TransformIdSubseq::EQUALITY_CODING:
                break;
            case TransformIdSubseq::MATCH_CODING:
                match_coding_buffer_size = util::make_unique<uint16_t >(reader.read(16));
                break;
            case TransformIdSubseq::RLE_CODING:
                rle_coding_guard = util::make_unique<uint8_t >(reader.read(8));
                break;
            case TransformIdSubseq::MERGE_CODING:
                UTILS_DIE("Merge coding not supported");
                break;
        }
    }

    TransformSubseqParameters(const TransformIdSubseq &_transform_ID_subseq, uint16_t param);

    virtual ~TransformSubseqParameters() = default;

    std::unique_ptr<TransformSubseqParameters> clone() const;

    size_t getNumStreams() const;

    TransformIdSubseq getTransformIdSubseq() const { return transform_ID_subseq; }

    uint16_t getParam() const {
        switch (transform_ID_subseq) {
            case TransformIdSubseq::NO_TRANSFORM:
            case TransformIdSubseq::EQUALITY_CODING:
                return 0;
            case TransformIdSubseq::MATCH_CODING:
                return *match_coding_buffer_size;
            case TransformIdSubseq::RLE_CODING:
                return *rle_coding_guard;
            case TransformIdSubseq::MERGE_CODING:
                return *merge_coding_subseq_count;
        }
    }

    const std::vector<uint8_t>* getMergeCodingShiftSize() const {
        return &merge_coding_shift_size;
    }

   private:
    TransformIdSubseq transform_ID_subseq;               //!< : 8; Line 2
    std::unique_ptr<uint16_t> match_coding_buffer_size;  //!< : 16; Line 6
    std::unique_ptr<uint8_t> rle_coding_guard;           //!< : 8; Line 9
    std::unique_ptr<uint8_t> merge_coding_subseq_count;  //!< : 4; Line 12
    std::vector<uint8_t> merge_coding_shift_size;        //!< : 5; Line 15
};
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_TRANSFORM_SUBSEQ_PARAMETERS_H
