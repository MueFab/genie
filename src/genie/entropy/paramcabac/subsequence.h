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
#include "transformed-subseq.h"
#include "transformed_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

class Subsequence {
   public:
    Subsequence();

    Subsequence(uint16_t _descriptor_subsequence_ID, bool _tokentypeFlag);

    Subsequence(TransformedParameters&& _transform_subseq_parameters, uint16_t _descriptor_subsequence_ID,
                bool _tokentypeFlag, std::vector<TransformedSubSeq>&& transformSubseq_cfgs);

    Subsequence(bool tokentype, util::BitReader& reader);

    virtual ~Subsequence() = default;

    void setTransformSubseqCfg(size_t index, TransformedSubSeq&& _transformSubseq_cfg);

    virtual void write(util::BitWriter& writer) const;

    uint16_t getDescriptorSubsequenceID() const;

    const TransformedParameters& getTransformParameters() const;

    const TransformedSubSeq& getTransformSubseqCfg(uint8_t index) const;

    size_t getNumTransformSubseqCfgs() const;

    const std::vector<TransformedSubSeq>& getTransformSubseqCfgs() const;

    bool getTokentypeFlag() const;

   private:
    boost::optional<uint16_t> descriptor_subsequence_ID;
    bool tokentypeFlag;  // helper field - not written to bitstream
    TransformedParameters transform_subseq_parameters;
    std::vector<TransformedSubSeq> transformSubseq_cfgs;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SUBSEQUENCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
