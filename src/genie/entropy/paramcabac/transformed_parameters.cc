/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "transformed_parameters.h"
#include <genie/util/bitwriter.h>
#include <genie/util/exceptions.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

TransformedParameters::TransformedParameters() : TransformedParameters(TransformIdSubseq::NO_TRANSFORM, 0) {}

// ---------------------------------------------------------------------------------------------------------------------

TransformedParameters::TransformedParameters(util::BitReader &reader) {
    transform_ID_subseq = reader.read<TransformIdSubseq>();
    switch (transform_ID_subseq) {
        case TransformIdSubseq::NO_TRANSFORM:
            break;
        case TransformIdSubseq::EQUALITY_CODING:
            break;
        case TransformIdSubseq::MATCH_CODING:
            match_coding_buffer_size = util::make_unique<uint16_t>(reader.read<uint16_t >());
            break;
        case TransformIdSubseq::RLE_CODING:
            rle_coding_guard = util::make_unique<uint8_t>(reader.read<uint8_t >());
            break;
        case TransformIdSubseq::MERGE_CODING:
            UTILS_DIE("Merge core not supported");
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TransformedParameters::TransformedParameters(const TransformIdSubseq &_transform_ID_subseq, uint16_t param)
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
            rle_coding_guard = util::make_unique<uint8_t>(param);
            break;
        case TransformIdSubseq::MATCH_CODING:
            match_coding_buffer_size = util::make_unique<uint16_t>(param);
            break;
        case TransformIdSubseq::MERGE_CODING:
            UTILS_THROW_RUNTIME_EXCEPTION("Merge core not supported");
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<TransformedParameters> TransformedParameters::clone() const {
    auto ret = util::make_unique<TransformedParameters>();
    ret->transform_ID_subseq = transform_ID_subseq;
    if (match_coding_buffer_size) {
        ret->match_coding_buffer_size = util::make_unique<uint16_t>(*match_coding_buffer_size);
    }
    if (rle_coding_guard) {
        ret->rle_coding_guard = util::make_unique<uint8_t>(*rle_coding_guard);
    }
    if (merge_coding_subseq_count) {
        ret->merge_coding_subseq_count = util::make_unique<uint8_t>(*merge_coding_subseq_count);
    }
    ret->merge_coding_shift_size = merge_coding_shift_size;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t TransformedParameters::getNumStreams() const {
    static const std::vector<size_t> lut = {1, 2, 3, 2, 1};
    return lut[uint8_t(transform_ID_subseq)];
}

// ---------------------------------------------------------------------------------------------------------------------

TransformedParameters::TransformIdSubseq TransformedParameters::getTransformIdSubseq() const {
    return transform_ID_subseq;
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t TransformedParameters::getParam() const {
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
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint8_t> *TransformedParameters::getMergeCodingShiftSize() const { return &merge_coding_shift_size; }

// ---------------------------------------------------------------------------------------------------------------------

void TransformedParameters::write(util::BitWriter &writer) const {
    writer.write(uint8_t(transform_ID_subseq), 8);
    if (match_coding_buffer_size) {
        writer.write(*match_coding_buffer_size, 16);
    }
    if (rle_coding_guard) {
        writer.write(*rle_coding_guard, 8);
    }
    if (merge_coding_subseq_count) {
        writer.write(*merge_coding_subseq_count, 4);
    }
    for (auto &i : merge_coding_shift_size) {
        writer.write(i, 5);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------