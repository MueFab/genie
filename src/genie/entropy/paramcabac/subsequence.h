/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SUBSEQUENCE_H
#define GENIE_SUBSEQUENCE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitwriter.h>
#include <memory>
#include <vector>
#include "transformed-seq.h"
#include "transformed_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

class Subsequence {
   public:
    Subsequence();

    Subsequence(TransformedParameters&& _transform_subseq_parameters, uint16_t descriptor_subsequence_ID,
                bool tokentype);

    Subsequence(bool tokentype, util::BitReader& reader);

    virtual ~Subsequence() = default;

    void setTransformSubseqCfg(size_t index, TransformedSeq&& _transformSubseq_cfg);

    virtual void write(util::BitWriter& writer) const;

    uint16_t getDescriptorSubsequenceID() const;

    const TransformedParameters& getTransformParameters() const;

    const TransformedSeq& getTransformSubseqCfg(uint8_t index) const;

    size_t getNumTransformSubseqCfgs() const;

    const std::vector<TransformedSeq>& getTransformSubseqCfgs() const;

   private:
    boost::optional<uint16_t> descriptor_subsequence_ID;
    TransformedParameters transform_subseq_parameters;
    std::vector<TransformedSeq> transformSubseq_cfgs;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SUBSEQUENCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
