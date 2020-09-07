/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "transformed-subseq.h"

#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

TransformedSubSeq::TransformedSubSeq()
    : TransformedSubSeq(SupportValues::TransformIdSubsym::NO_TRANSFORM, SupportValues(), Binarization()) {}

// ---------------------------------------------------------------------------------------------------------------------

TransformedSubSeq::TransformedSubSeq(SupportValues::TransformIdSubsym _transform_ID_subsym,
                                     SupportValues&& _support_values, Binarization&& _cabac_binarization,
                                     const core::GenSubIndex _subsequence_ID, const core::AlphabetID _alphabet_ID)
    : transform_ID_subsym(_transform_ID_subsym),
      support_values(std::move(_support_values)),
      cabac_binarization(std::move(_cabac_binarization)),
      subsequence_ID(_subsequence_ID),
      alphabet_ID(_alphabet_ID) {
    state_vars.populate(transform_ID_subsym, support_values, cabac_binarization, subsequence_ID, alphabet_ID);
}

// ---------------------------------------------------------------------------------------------------------------------

TransformedSubSeq::TransformedSubSeq(util::BitReader& reader, const core::GenSubIndex _subsequence_ID,
                                     const core::AlphabetID _alphabet_ID)
    : subsequence_ID(_subsequence_ID), alphabet_ID(_alphabet_ID) {
    transform_ID_subsym = reader.read<SupportValues::TransformIdSubsym>(3);
    support_values = SupportValues(transform_ID_subsym, reader);
    cabac_binarization =
        Binarization(support_values.getCodingSubsymSize(), support_values.getOutputSymbolSize(), reader);
    state_vars.populate(transform_ID_subsym, support_values, cabac_binarization, subsequence_ID, alphabet_ID);
}

// ---------------------------------------------------------------------------------------------------------------------

SupportValues::TransformIdSubsym TransformedSubSeq::getTransformIDSubsym() const { return transform_ID_subsym; }

// ---------------------------------------------------------------------------------------------------------------------

const SupportValues& TransformedSubSeq::getSupportValues() const { return support_values; }

// ---------------------------------------------------------------------------------------------------------------------

const Binarization& TransformedSubSeq::getBinarization() const { return cabac_binarization; }

// ---------------------------------------------------------------------------------------------------------------------

const StateVars& TransformedSubSeq::getStateVars() const { return state_vars; }

// ---------------------------------------------------------------------------------------------------------------------

StateVars& TransformedSubSeq::getStateVars() { return state_vars; }

// ---------------------------------------------------------------------------------------------------------------------

void TransformedSubSeq::write(util::BitWriter& writer) const {
    writer.write(uint8_t(transform_ID_subsym), 3);
    support_values.write(transform_ID_subsym, writer);
    cabac_binarization.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

void TransformedSubSeq::setSubsequenceID(const core::GenSubIndex _subsequence_ID) { subsequence_ID = _subsequence_ID; }

// ---------------------------------------------------------------------------------------------------------------------

core::AlphabetID TransformedSubSeq::getAlphabetID() const { return alphabet_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void TransformedSubSeq::setAlphabetID(core::AlphabetID _alphabet_ID) { alphabet_ID = _alphabet_ID; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
