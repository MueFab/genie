/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_TRANSFORMED_SEQ_H
#define GENIE_TRANSFORMED_SEQ_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <memory>
#include "binarization.h"
#include "support_values.h"
#include "state_vars.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * ISO 23092-2 Section 8.3.1 table lines 9 to 11
 */
class TransformedSeq {
   public:
    TransformedSeq();

    TransformedSeq(SupportValues::TransformIdSubsym _transform_ID_subsym,
                   SupportValues&& _support_values,
                   Binarization&& _cabac_binarization,
                   const core::GenSubIndex _subsequence_ID = core::GenSub::POS_MAPPING_FIRST,
                   const core::AlphabetID _alphabet_ID = core::AlphabetID::ACGTN);

    TransformedSeq(util::BitReader& reader,
                   const core::GenSubIndex _subsequence_ID = core::GenSub::POS_MAPPING_FIRST,
                   const core::AlphabetID _alphabet_ID = core::AlphabetID::ACGTN);

    virtual ~TransformedSeq() = default;

    virtual void write(util::BitWriter& writer) const;

    SupportValues::TransformIdSubsym getTransformID() const;

    const SupportValues& getSupportValues() const;

    const Binarization& getBinarization() const;

    const StateVars& getStateVars() const;

    void setSubsequenceID(const core::GenSubIndex _subsequence_ID) {
        subsequence_ID = _subsequence_ID;
    }

    core::AlphabetID getAlphabetID() const {
        return alphabet_ID;
    }

    void setAlphabetID(core::AlphabetID _alphabet_ID) {
        alphabet_ID = _alphabet_ID;
    }

   private:
    SupportValues::TransformIdSubsym transform_ID_subsym;  //!< : 3; Line 9
    SupportValues support_values;                          //!< Line 10
    Binarization cabac_binarization;                       //!< Line 11
    StateVars state_vars;

    // declaration with default value.
    core::GenSubIndex subsequence_ID = core::GenSub::POS_MAPPING_FIRST;
    core::AlphabetID alphabet_ID = core::AlphabetID::ACGTN;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_TRANSFORMED_SEQ_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
