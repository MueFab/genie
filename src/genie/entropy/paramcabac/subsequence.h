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

/**
 *
 */
class Subsequence {
   public:
    /**
     *
     */
    Subsequence();

    /**
     *
     * @param _descriptor_subsequence_ID
     * @param _tokentypeFlag
     */
    Subsequence(uint16_t _descriptor_subsequence_ID, bool _tokentypeFlag);

    /**
     *
     * @param _transform_subseq_parameters
     * @param _descriptor_subsequence_ID
     * @param _tokentypeFlag
     * @param transformSubseq_cfgs
     */
    Subsequence(TransformedParameters&& _transform_subseq_parameters, uint16_t _descriptor_subsequence_ID,
                bool _tokentypeFlag, std::vector<TransformedSubSeq>&& transformSubseq_cfgs);

    /**
     *
     * @param tokentype
     * @param reader
     */
    Subsequence(bool tokentype, util::BitReader& reader);

    /**
     *
     */
    virtual ~Subsequence() = default;

    /**
     *
     * @param index
     * @param _transformSubseq_cfg
     */
    void setTransformSubseqCfg(size_t index, TransformedSubSeq&& _transformSubseq_cfg);

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     *
     * @return
     */
    uint16_t getDescriptorSubsequenceID() const;

    /**
     *
     * @return
     */
    const TransformedParameters& getTransformParameters() const;

    /**
     *
     * @param index
     * @return
     */
    const TransformedSubSeq& getTransformSubseqCfg(uint8_t index) const;

    /**
     *
     * @return
     */
    size_t getNumTransformSubseqCfgs() const;

    /**
     *
     * @return
     */
    const std::vector<TransformedSubSeq>& getTransformSubseqCfgs() const;

    /**
     *
     * @return
     */
    bool getTokentypeFlag() const;

    bool operator==(const Subsequence& seq) const {
        return descriptor_subsequence_ID == seq.descriptor_subsequence_ID && tokentypeFlag == seq.tokentypeFlag &&
               transform_subseq_parameters == seq.transform_subseq_parameters &&
               transformSubseq_cfgs == seq.transformSubseq_cfgs;
    }

   private:
    boost::optional<uint16_t> descriptor_subsequence_ID;  //!<
    bool tokentypeFlag;                                   //!< helper field - not written to bitstream
    TransformedParameters transform_subseq_parameters;    //!<
    std::vector<TransformedSubSeq> transformSubseq_cfgs;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SUBSEQUENCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
