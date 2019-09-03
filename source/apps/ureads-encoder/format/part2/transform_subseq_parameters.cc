#include "transform_subseq_parameters.h"
#include "make_unique.h"
#include "bitwriter.h"

Transform_subseq_parameters::Transform_subseq_parameters() : Transform_subseq_parameters(
        Transform_ID_subseq::no_transform, 0) {

}

Transform_subseq_parameters::Transform_subseq_parameters(
        const Transform_ID_subseq &_transform_ID_subseq,
        uint16_t param
) : transform_ID_subseq(_transform_ID_subseq),
    match_coding_buffer_size(nullptr),
    rle_coding_guard(nullptr),
    merge_coding_subseq_count(nullptr),
    merge_coding_shift_size(0) {

    switch (transform_ID_subseq) {
        case Transform_ID_subseq::no_transform:
        case Transform_ID_subseq::equality_coding:
            break;
        case Transform_ID_subseq::rle_coding:
            rle_coding_guard = make_unique<uint8_t>(param);
            break;
        case Transform_ID_subseq::match_coding:
            match_coding_buffer_size = make_unique<uint16_t>(param);
            break;
        case Transform_ID_subseq::merge_coding:
            GENIE_THROW_RUNTIME_EXCEPTION("Merge coding not supported");
            break;
    }
}

std::unique_ptr<Transform_subseq_parameters> Transform_subseq_parameters::clone() const {
    auto ret = make_unique<Transform_subseq_parameters>();
    ret->transform_ID_subseq = transform_ID_subseq;
    ret->match_coding_buffer_size = make_unique<uint16_t>(*match_coding_buffer_size);
    ret->rle_coding_guard = make_unique<uint8_t>(*rle_coding_guard);
    ret->merge_coding_subseq_count = make_unique<uint8_t>(*merge_coding_subseq_count);
    ret->merge_coding_shift_size = merge_coding_shift_size;
    return ret;
}

void Transform_subseq_parameters::write(BitWriter *writer) const {
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