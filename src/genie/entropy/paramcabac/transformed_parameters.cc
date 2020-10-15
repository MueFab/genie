/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "transformed_parameters.h"
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <genie/util/runtime-exception.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

TransformedParameters::TransformedParameters() : TransformedParameters(TransformIdSubseq::NO_TRANSFORM, 0) {}

// ---------------------------------------------------------------------------------------------------------------------

TransformedParameters::TransformedParameters(util::BitReader &reader) {
    transform_ID_subseq = reader.read<TransformIdSubseq>(8);
    switch (transform_ID_subseq) {
        case TransformIdSubseq::NO_TRANSFORM:
            break;
        case TransformIdSubseq::EQUALITY_CODING:
            break;
        case TransformIdSubseq::MATCH_CODING:
            match_coding_buffer_size = reader.read<uint16_t>();
            break;
        case TransformIdSubseq::RLE_CODING:
            rle_coding_guard = reader.read<uint8_t>();
            break;
        case TransformIdSubseq::MERGE_CODING:
            merge_coding_subseq_count = reader.read<uint8_t>(4);
            merge_coding_shift_size.resize(*merge_coding_subseq_count);
            for (int i=0; i < *merge_coding_subseq_count; i++) {
                merge_coding_shift_size[i] = reader.read<uint8_t>(5);
            }
            break;
        default:
            UTILS_THROW_RUNTIME_EXCEPTION("Invalid subseq transformation");
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TransformedParameters::TransformedParameters(const TransformIdSubseq &_transform_ID_subseq, uint16_t param)
    : transform_ID_subseq(_transform_ID_subseq),
      match_coding_buffer_size(),
      rle_coding_guard(),
      merge_coding_subseq_count(),
      merge_coding_shift_size(0) {
    switch (transform_ID_subseq) {
        case TransformIdSubseq::NO_TRANSFORM:
        case TransformIdSubseq::EQUALITY_CODING:
            break;
        case TransformIdSubseq::RLE_CODING:
            rle_coding_guard = param;
            break;
        case TransformIdSubseq::MATCH_CODING:
            match_coding_buffer_size = param;
            break;
        case TransformIdSubseq::MERGE_CODING:
            merge_coding_subseq_count = param;
            break;
        default:
            UTILS_THROW_RUNTIME_EXCEPTION("Invalid subseq transformation");
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t TransformedParameters::getNumStreams() const {
    if (transform_ID_subseq == TransformedParameters::TransformIdSubseq::MERGE_CODING)
        return *merge_coding_subseq_count;
    else {
        static const std::vector<size_t> lut = {1, 2, 3, 2, 1};
        return lut[uint8_t(transform_ID_subseq)];
    }
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

const std::vector<uint8_t> TransformedParameters::getMergeCodingShiftSizes() const { return merge_coding_shift_size; }

// ---------------------------------------------------------------------------------------------------------------------

void TransformedParameters::setMergeCodingShiftSizes(std::vector<uint8_t> mergeCodingshiftSizes) {
    merge_coding_shift_size = mergeCodingshiftSizes;
}

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