/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "subsequence.h"
#include <genie/util/bitwriter.h>
#include <genie/util/exceptions.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Subsequence> Subsequence::clone() const {
    auto ret = util::make_unique<Subsequence>(transform_subseq_parameters->clone(), 0, false);
    if (descriptor_subsequence_ID) {
        ret->descriptor_subsequence_ID = util::make_unique<uint16_t>(*descriptor_subsequence_ID);
    } else {
        ret->descriptor_subsequence_ID = nullptr;
    }
    ret->transform_subseq_parameters = transform_subseq_parameters->clone();
    ret->transformSubseq_cfgs.clear();
    for (const auto& c : transformSubseq_cfgs) {
        ret->transformSubseq_cfgs.push_back(c->clone());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

Subsequence::Subsequence(std::unique_ptr<TransformedParameters> _transform_subseq_parameters,
                         uint16_t _descriptor_subsequence_ID, bool tokentype)
    : descriptor_subsequence_ID(nullptr),
      transform_subseq_parameters(std::move(_transform_subseq_parameters)),
      transformSubseq_cfgs(transform_subseq_parameters->getNumStreams()) {
    if (!tokentype) {
        descriptor_subsequence_ID = util::make_unique<uint16_t>(_descriptor_subsequence_ID);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Subsequence::Subsequence(bool tokentype, util::BitReader& reader) {
    if (!tokentype) {
        descriptor_subsequence_ID = util::make_unique<uint16_t>(reader.read(10));
    }
    transform_subseq_parameters = util::make_unique<TransformedParameters>(reader);
    uint8_t numSubseq = 0;
    switch (transform_subseq_parameters->getTransformIdSubseq()) {
        case TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
            numSubseq = 1;
            break;
        case TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
            numSubseq = 2;
            break;
        case TransformedParameters::TransformIdSubseq::RLE_CODING:
            numSubseq = 2;
            break;
        case TransformedParameters::TransformIdSubseq::MATCH_CODING:
            numSubseq = 3;
            break;
        case TransformedParameters::TransformIdSubseq::MERGE_CODING:
            UTILS_DIE("MERGE core unsupported");
            break;
    }
    for (size_t i = 0; i < numSubseq; ++i) {
        transformSubseq_cfgs.emplace_back(util::make_unique<TransformedSeq>(reader));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Subsequence::setTransformSubseqCfg(size_t index, std::unique_ptr<TransformedSeq> _transformSubseq_cfg) {
    transformSubseq_cfgs[index] = std::move(_transformSubseq_cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

void Subsequence::write(util::BitWriter& writer) const {
    if (descriptor_subsequence_ID) {
        writer.write(*descriptor_subsequence_ID, 10);
    }
    transform_subseq_parameters->write(writer);
    for (auto& i : transformSubseq_cfgs) {
        i->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Subsequence::getDescriptorSubsequenceID() const {
    if (!descriptor_subsequence_ID) {
        UTILS_DIE("descriptor_subsequence_ID not present");
    }
    return *descriptor_subsequence_ID.get();
}

// ---------------------------------------------------------------------------------------------------------------------

const TransformedParameters* Subsequence::getTransformParameters() const { return transform_subseq_parameters.get(); }

// ---------------------------------------------------------------------------------------------------------------------

const TransformedSeq* Subsequence::getTransformSubseqCfg(uint8_t index) const {
    return transformSubseq_cfgs[index].get();
}

// ---------------------------------------------------------------------------------------------------------------------

size_t Subsequence::getNumTransformSubseqCfgs() const { return transformSubseq_cfgs.size(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::unique_ptr<TransformedSeq>>& Subsequence::getTransformSubseqCfgs() const {
    return transformSubseq_cfgs;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------