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

/**
 * ISO 23092-2 Section 8.3.1 table lines 5 to 12 + 8.3.5 table 102
 */
class Subsequence {
   private:
    std::unique_ptr<uint16_t> descriptor_subsequence_ID;  //!< : 10; line 5, absent in 8.3.5, present in 8.3.1
    std::unique_ptr<TransformedParameters> transform_subseq_parameters;  //!< lines 6 + 7
    std::vector<std::unique_ptr<TransformedSeq>> transformSubseq_cfgs;   //!< lines 8 to 12
    Subsequence() = default;

   public:
    Subsequence(std::unique_ptr<TransformedParameters> _transform_subseq_parameters, uint16_t descriptor_subsequence_ID,
                bool tokentype);

    Subsequence(bool tokentype, util::BitReader& reader);

    virtual ~Subsequence() = default;

    void setTransformSubseqCfg(size_t index, std::unique_ptr<TransformedSeq> _transformSubseq_cfg);

    std::unique_ptr<Subsequence> clone() const;

    virtual void write(util::BitWriter& writer) const;

    uint16_t getDescriptorSubsequenceID() const;

    const TransformedParameters* getTransformParameters() const;

    const TransformedSeq* getTransformSubseqCfg(uint8_t index) const;

    size_t getNumTransformSubseqCfgs() const;

    const std::vector<std::unique_ptr<TransformedSeq>>& getTransformSubseqCfgs() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SUBSEQUENCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------