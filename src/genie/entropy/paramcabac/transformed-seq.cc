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
    : TransformedSeq(SupportValues::TransformIdSubsym::NO_TRANSFORM, util::make_unique<SupportValues>(),
                     util::make_unique<Binarization>()) {}

// ---------------------------------------------------------------------------------------------------------------------

TransformedSeq::TransformedSeq(SupportValues::TransformIdSubsym _transform_ID_subsym,
                               std::unique_ptr<SupportValues> _support_values,
                               std::unique_ptr<Binarization> _cabac_binarization)
    : transform_ID_subsym(_transform_ID_subsym),
      support_values(std::move(_support_values)),
      cabac_binarization(std::move(_cabac_binarization)) {}

// ---------------------------------------------------------------------------------------------------------------------

TransformedSeq::TransformedSeq(util::BitReader& reader) {
    transform_ID_subsym = reader.read<SupportValues::TransformIdSubsym>(3);
    support_values = util::make_unique<SupportValues>(transform_ID_subsym, reader);
    cabac_binarization = util::make_unique<Binarization>(support_values->getCodingSubsymSize(),
                                                         support_values->getOutputSymbolSize(), reader);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<TransformedSeq> TransformedSeq::clone() const {
    auto ret = util::make_unique<TransformedSeq>();
    ret->transform_ID_subsym = transform_ID_subsym;
    ret->support_values = support_values->clone();
    ret->cabac_binarization = cabac_binarization->clone();
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

SupportValues::TransformIdSubsym TransformedSeq::getTransformID() const { return transform_ID_subsym; }

// ---------------------------------------------------------------------------------------------------------------------

const SupportValues* TransformedSeq::getSupportValues() const { return support_values.get(); }

// ---------------------------------------------------------------------------------------------------------------------

const Binarization* TransformedSeq::getBinarization() const { return cabac_binarization.get(); }

// ---------------------------------------------------------------------------------------------------------------------

void TransformedSeq::write(util::BitWriter& writer) const {
    writer.write(uint8_t(transform_ID_subsym), 3);
    support_values->write(writer);
    cabac_binarization->write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------