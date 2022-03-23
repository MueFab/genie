/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <nlohmann/json.hpp>
#include "genie/entropy/paramcabac/binarization.h"
#include "genie/entropy/paramcabac/state_vars.h"
#include "genie/entropy/paramcabac/support_values.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * @brief
 */
class TransformedSubSeq {
 public:
    /**
     * @brief
     */
    TransformedSubSeq();

    /**
     * @brief
     * @param _transform_ID_subsym
     * @param _support_values
     * @param _cabac_binarization
     * @param _subsequence_ID
     * @param _alphabet_ID
     */
    TransformedSubSeq(SupportValues::TransformIdSubsym _transform_ID_subsym, SupportValues&& _support_values,
                      Binarization&& _cabac_binarization, const core::GenSubIndex _subsequence_ID,
                      const core::AlphabetID _alphabet_ID = core::AlphabetID::ACGTN);
    /**
     * @brief
     * @param reader
     * @param _subsequence_ID
     * @param _alphabet_ID
     */
    TransformedSubSeq(util::BitReader& reader, const core::GenSubIndex _subsequence_ID,
                      const core::AlphabetID _alphabet_ID = core::AlphabetID::ACGTN);

    /**
     * @brief
     */
    virtual ~TransformedSubSeq() = default;

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    SupportValues::TransformIdSubsym getTransformIDSubsym() const;

    /**
     * @brief
     * @return
     */
    const SupportValues& getSupportValues() const;

    /**
     * @brief
     * @return
     */
    const Binarization& getBinarization() const;

    /**
     * @brief
     * @return
     */
    const StateVars& getStateVars() const;

    /**
     * @brief
     * @return
     */
    StateVars& getStateVars();

    /**
     * @brief
     * @param _subsequence_ID
     */
    void setSubsequenceID(const core::GenSubIndex _subsequence_ID);

    /**
     * @brief
     * @return
     */
    core::AlphabetID getAlphabetID() const;

    /**
     * @brief
     * @param _alphabet_ID
     */
    void setAlphabetID(core::AlphabetID _alphabet_ID);

    /**
     * @brief
     * @param val
     * @return
     */
    bool operator==(const TransformedSubSeq& val) const;

    /**
     * @brief
     * @param j
     */
    explicit TransformedSubSeq(nlohmann::json j);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;

 private:
    SupportValues::TransformIdSubsym transform_ID_subsym;  //!< @brief
    SupportValues support_values;                          //!< @brief
    Binarization cabac_binarization;                       //!< @brief
    StateVars state_vars;                                  //!< @brief

    // declaration with default value.
    core::GenSubIndex subsequence_ID = core::GenSub::POS_MAPPING_FIRST;  //!< @brief
    core::AlphabetID alphabet_ID = core::AlphabetID::ACGTN;              //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
