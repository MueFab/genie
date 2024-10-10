/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_SUBSEQUENCE_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "genie/entropy/paramcabac/transformed-subseq.h"
#include "genie/entropy/paramcabac/transformed_parameters.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief
 */
class Subsequence {
 public:
    /**
     * @brief
     */
    Subsequence();

    /**
     * @brief
     * @param _descriptor_subsequence_ID
     * @param _tokentypeFlag
     */
    Subsequence(uint16_t _descriptor_subsequence_ID, bool _tokentypeFlag);

    /**
     * @brief
     * @param _transform_subseq_parameters
     * @param _descriptor_subsequence_ID
     * @param _tokentypeFlag
     * @param transformSubseq_cfgs
     */
    Subsequence(TransformedParameters&& _transform_subseq_parameters, uint16_t _descriptor_subsequence_ID,
                bool _tokentypeFlag, std::vector<TransformedSubSeq>&& transformSubseq_cfgs);

    /**
     * @brief
     * @param tokentype
     * @param desc
     * @param reader
     */
    Subsequence(bool tokentype, core::GenDesc desc, util::BitReader& reader);

    /**
     * @brief
     */
    virtual ~Subsequence() = default;

    /**
     * @brief
     * @param index
     * @param _transformSubseq_cfg
     */
    void setTransformSubseqCfg(size_t index, TransformedSubSeq&& _transformSubseq_cfg);

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getDescriptorSubsequenceID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const TransformedParameters& getTransformParameters() const;

    /**
     * @brief
     * @param index
     * @return
     */
    [[nodiscard]] const TransformedSubSeq& getTransformSubseqCfg(uint8_t index) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] size_t getNumTransformSubseqCfgs() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<TransformedSubSeq>& getTransformSubseqCfgs() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool getTokentypeFlag() const;

    /**
     * @brief
     * @param seq
     * @return
     */
    bool operator==(const Subsequence& seq) const;

    /**
     * @brief
     * @param j
     */
    explicit Subsequence(nlohmann::json j);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] nlohmann::json toJoson() const;

 private:
    std::optional<uint16_t> descriptor_subsequence_ID;    //!< @brief
    TransformedParameters transform_subseq_parameters;    //!< @brief
    std::vector<TransformedSubSeq> transformSubseq_cfgs;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
