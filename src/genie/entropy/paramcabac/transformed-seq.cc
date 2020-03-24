/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "transformed-seq.h"
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

TransformedSeq::TransformedSeq()
    : TransformedSeq(SupportValues::TransformIdSubsym::NO_TRANSFORM, SupportValues(), Binarization()) {}

// ---------------------------------------------------------------------------------------------------------------------

TransformedSeq::TransformedSeq(SupportValues::TransformIdSubsym _transform_ID_subsym,
                               SupportValues&& _support_values,
                               Binarization&& _cabac_binarization,
                               const core::GenDesc _descriptor_ID,
                               const core::GenSubIndex _subsequence_ID,
                               const core::AlphabetID _alphabet_ID)
    : transform_ID_subsym(_transform_ID_subsym),
      support_values(std::move(_support_values)),
      cabac_binarization(std::move(_cabac_binarization)),
      descriptor_ID(_descriptor_ID),
      subsequence_ID(_subsequence_ID),
      alphabet_ID(_alphabet_ID) {
    state_vars.populate(transform_ID_subsym,
                        support_values,
                        cabac_binarization,
                        descriptor_ID,
                        subsequence_ID,
                        alphabet_ID);
}

// ---------------------------------------------------------------------------------------------------------------------

TransformedSeq::TransformedSeq(util::BitReader& reader,
                               const core::GenDesc _descriptor_ID,
                               const core::GenSubIndex _subsequence_ID,
                               const core::AlphabetID _alphabet_ID)
      : descriptor_ID(_descriptor_ID),
        subsequence_ID(_subsequence_ID),
        alphabet_ID(_alphabet_ID) {
    transform_ID_subsym = reader.read<SupportValues::TransformIdSubsym>(3);
    support_values = SupportValues(transform_ID_subsym, reader);
    cabac_binarization =
        Binarization(support_values.getCodingSubsymSize(), support_values.getOutputSymbolSize(), reader);
}

// ---------------------------------------------------------------------------------------------------------------------

SupportValues::TransformIdSubsym TransformedSeq::getTransformID() const { return transform_ID_subsym; }

// ---------------------------------------------------------------------------------------------------------------------

const SupportValues& TransformedSeq::getSupportValues() const { return support_values; }

// ---------------------------------------------------------------------------------------------------------------------

const Binarization& TransformedSeq::getBinarization() const { return cabac_binarization; }

// ---------------------------------------------------------------------------------------------------------------------

const StateVars& TransformedSeq::getStateVars() const { return state_vars; }

// ---------------------------------------------------------------------------------------------------------------------

void TransformedSeq::write(util::BitWriter& writer) const {
    writer.write(uint8_t(transform_ID_subsym), 3);
    support_values.write(writer);
    cabac_binarization.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
