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

Subsequence::Subsequence()
    : Subsequence(TransformedParameters(), 0, false, std::vector<TransformedSubSeq>({TransformedSubSeq()})) {}

// ---------------------------------------------------------------------------------------------------------------------

Subsequence::Subsequence(uint16_t _descriptor_subsequence_ID, bool _tokentypeFlag)
    : Subsequence(TransformedParameters(), _descriptor_subsequence_ID, _tokentypeFlag,
                  std::vector<TransformedSubSeq>({TransformedSubSeq()})) {}

// ---------------------------------------------------------------------------------------------------------------------

Subsequence::Subsequence(TransformedParameters&& _transform_subseq_parameters, uint16_t _descriptor_subsequence_ID,
                         bool _tokentypeFlag, std::vector<TransformedSubSeq>&& _transformSubseq_cfgs)
    : descriptor_subsequence_ID(),
      tokentypeFlag(_tokentypeFlag),
      transform_subseq_parameters(std::move(_transform_subseq_parameters)) {
    if (!tokentypeFlag) {
        descriptor_subsequence_ID = _descriptor_subsequence_ID;
    }

    if (_transformSubseq_cfgs.size() == transform_subseq_parameters.getNumStreams()) {
        transformSubseq_cfgs = std::move(_transformSubseq_cfgs);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Subsequence::Subsequence(bool tokentype, util::BitReader& reader) {
    tokentypeFlag = tokentype;
    if (!tokentypeFlag) {
        descriptor_subsequence_ID = reader.read<uint16_t>(10);
    }
    transform_subseq_parameters = TransformedParameters(reader);
    uint8_t numSubseq = 0;
    switch (transform_subseq_parameters.getTransformIdSubseq()) {
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
        transformSubseq_cfgs.emplace_back(reader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Subsequence::setTransformSubseqCfg(size_t index, TransformedSubSeq&& _transformSubseq_cfg) {
    transformSubseq_cfgs[index] = std::move(_transformSubseq_cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

void Subsequence::write(util::BitWriter& writer) const {
    if (descriptor_subsequence_ID) {
        writer.write(*descriptor_subsequence_ID, 10);
    }
    transform_subseq_parameters.write(writer);
    for (auto& i : transformSubseq_cfgs) {
        i.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Subsequence::getDescriptorSubsequenceID() const {
    UTILS_DIE_IF(!descriptor_subsequence_ID, "descriptor_subsequence_ID not present");
    return descriptor_subsequence_ID.get();
}

// ---------------------------------------------------------------------------------------------------------------------

const TransformedParameters& Subsequence::getTransformParameters() const { return transform_subseq_parameters; }

// ---------------------------------------------------------------------------------------------------------------------

const TransformedSubSeq& Subsequence::getTransformSubseqCfg(uint8_t index) const { return transformSubseq_cfgs[index]; }

// ---------------------------------------------------------------------------------------------------------------------

size_t Subsequence::getNumTransformSubseqCfgs() const { return transformSubseq_cfgs.size(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<TransformedSubSeq>& Subsequence::getTransformSubseqCfgs() const { return transformSubseq_cfgs; }

// ---------------------------------------------------------------------------------------------------------------------

bool Subsequence::getTokentypeFlag() const { return tokentypeFlag; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
