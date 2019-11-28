#include "transform_subseq_parameters.h"
#include "../../../make_unique.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace desc_conf_pres {
namespace cabac {
TransformSubseqParameters::TransformSubseqParameters()
    : TransformSubseqParameters(TransformIdSubseq::NO_TRANSFORM, 0) {}

// -----------------------------------------------------------------------------------------------------------------

TransformSubseqParameters::TransformSubseqParameters(const TransformIdSubseq &_transform_ID_subseq, uint16_t param)
    : transform_ID_subseq(_transform_ID_subseq),
      match_coding_buffer_size(nullptr),
      rle_coding_guard(nullptr),
      merge_coding_subseq_count(nullptr),
      merge_coding_shift_size(0) {
    switch (transform_ID_subseq) {
        case TransformIdSubseq::NO_TRANSFORM:
        case TransformIdSubseq::EQUALITY_CODING:
            break;
        case TransformIdSubseq::RLE_CODING:
            rle_coding_guard = make_unique<uint8_t>(param);
            break;
        case TransformIdSubseq::MATCH_CODING:
            match_coding_buffer_size = make_unique<uint16_t>(param);
            break;
        case TransformIdSubseq::MERGE_CODING:
            UTILS_THROW_RUNTIME_EXCEPTION("Merge coding not supported");
            break;
    }
}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<TransformSubseqParameters> TransformSubseqParameters::clone() const {
    auto ret = make_unique<TransformSubseqParameters>();
    ret->transform_ID_subseq = transform_ID_subseq;
    ret->match_coding_buffer_size = make_unique<uint16_t>(*match_coding_buffer_size);
    ret->rle_coding_guard = make_unique<uint8_t>(*rle_coding_guard);
    ret->merge_coding_subseq_count = make_unique<uint8_t>(*merge_coding_subseq_count);
    ret->merge_coding_shift_size = merge_coding_shift_size;
    return ret;
}

// -----------------------------------------------------------------------------------------------------------------

size_t TransformSubseqParameters::getNumStreams() const {
    static const std::vector<size_t> lut = {1, 2, 3, 2, 1};
    return lut[uint8_t(transform_ID_subseq)];
}

// -----------------------------------------------------------------------------------------------------------------

void TransformSubseqParameters::write(util::BitWriter *writer) const {
    writer->write(uint8_t(transform_ID_subseq), 8);
    if (match_coding_buffer_size) {
        writer->write(*match_coding_buffer_size, 16);
    }
    if (rle_coding_guard) {
        writer->write(*rle_coding_guard, 8);
    }
    if (merge_coding_subseq_count) {
        writer->write(*merge_coding_subseq_count, 4);
    }
    for (auto &i : merge_coding_shift_size) {
        writer->write(i, 5);
    }
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format
